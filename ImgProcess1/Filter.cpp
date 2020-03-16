#include "pch.h"
#include "framework.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>
using namespace std;


static bool GetValue(int p[], int size, int& value)
{
	//数组中间的值，即滤波器窗口的中间位置，也就是被检测的像素位置
	int zxy = p[(size - 1) / 2];
	//用于记录原数组的下标
	int* a = new int[size];
	int index = 0;
	for (int i = 0; i < size; ++i)
		a[index++] = i;

	for (int i = 0; i < size - 1; i++)
		for (int j = i + 1; j < size; j++)
			if (p[i] > p[j]) {
				int tempA = a[i];
				a[i] = a[j];
				a[j] = tempA;
				int temp = p[i];
				p[i] = p[j];
				p[j] = temp;

			}
	int zmax = p[size - 1];
	int zmin = p[0];
	int zmed = p[(size - 1) / 2];

	if (zmax > zmed&& zmin < zmed) { // 1. zmed不是噪声
		if (zxy > zmin&& zxy < zmax) // 2.1 zxy不是噪声点，不替换
			value = (size - 1) / 2;
		else
			value = a[(size - 1) / 2]; // 2.2 zxy是噪声，用zmed替换
		delete[]a;
		return true;
	}
	else { // 退出，增大滤波器的窗口尺寸，继续寻找不是噪声的zmed
		delete[]a;
		return false;
	}

}


// 自适应中值滤波
// 改变窗口的大小；判断当前像素是不是噪声，是则替换，不是则不替换
UINT ImageProcess::medianFilter(LPVOID  p)
{
	ThreadParam* param = (ThreadParam*)p;

	int maxWidth = param->img->GetWidth();
	int maxHeight = param->img->GetHeight();
	int startIndex = param->startIndex;
	int endIndex = param->endIndex;
	// MAX_SPAN 15
	int maxSpan = param->maxSpan;
	// 移动窗口 最大尺寸 31*31像素
	int maxLength = (maxSpan * 2 + 1) * (maxSpan * 2 + 1);

	byte* pRealData = (byte*)param->img->GetBits();
	int pit = param->img->GetPitch();
	int bitCount = param->img->GetBPP() / 8;

	int* pixel = new int[maxLength];//存储每个像素点的灰度
	int* pixelR = new int[maxLength];
	int* pixelB = new int[maxLength];
	int* pixelG = new int[maxLength];
	int index = 0;
	for (int i = startIndex; i <= endIndex; ++i)
	{
		int Sxy = 1;
		int med = 0;
		int state = 0;
		// 像素为单位
		int x = i % maxWidth;
		int y = i / maxWidth;
		while (Sxy <= maxSpan)
		{
			index = 0;
			for (int tmpY = y - Sxy; tmpY <= y + Sxy && tmpY < maxHeight; tmpY++)
			{
				if (tmpY < 0) continue;
				for (int tmpX = x - Sxy; tmpX <= x + Sxy && tmpX < maxWidth; tmpX++)
				{
					if (tmpX < 0) continue;
					if (bitCount == 1)
					{
						pixel[index] = *(pRealData + pit * (tmpY)+(tmpX)*bitCount);
						pixelR[index++] = pixel[index];

					}
					else
					{
						pixelR[index] = *(pRealData + pit * (tmpY)+(tmpX)*bitCount + 2);
						pixelG[index] = *(pRealData + pit * (tmpY)+(tmpX)*bitCount + 1);
						pixelB[index] = *(pRealData + pit * (tmpY)+(tmpX)*bitCount);
						pixel[index++] = int(pixelB[index] * 0.299 + 0.587 * pixelG[index] + pixelR[index] * 0.144);

					}
				}

			}
			if (index <= 0)
				break;
			// 已经确定med，即为zxy（中心，即该像素）或zmed（邻域中值）
			if ((state = GetValue(pixel, index, med)) == 1) // index即pixel的元素个数
				break;

			// 扩大滤波器窗口尺寸，找不是噪声点的zmed
			Sxy++;
		};

		if (state) // 此像素是噪声
		{
			if (bitCount == 1)
			{
				*(pRealData + pit * y + x * bitCount) = pixelR[med];

			}
			else
			{
				*(pRealData + pit * y + x * bitCount + 2) = pixelR[med];
				*(pRealData + pit * y + x * bitCount + 1) = pixelG[med];
				*(pRealData + pit * y + x * bitCount) = pixelB[med];

			}
		}

	}

	delete[]pixel;
	delete[]pixelR;
	delete[]pixelG;
	delete[]pixelB;

	// 传递消息
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_FILTER, 1, NULL);
	return 0;
}

UINT ImageProcess::meanFilter(LPVOID  p) {
	ThreadParam* param = (ThreadParam*)p;

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();
	byte* pImgData = (byte*)param->img->GetBits();
	// 每个像素的字节数
	int imgBitCount = param->img->GetBPP() / 8;
	// GetPitch()图像的间距
	int imgPit = param->img->GetPitch();

	byte* pSrcData = (byte*)param->src->GetBits();
	// 每个像素的字节数
	int srcBitCount = param->src->GetBPP() / 8;
	// GetPitch()图像的间距
	int srcPit = param->src->GetPitch();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex;

	// 3*3 模板
	const int TEMPLATE_SIZE = 3 * 3;
	for (int i = startIndex; i <= endIndex; ++i){
		int ix = i % imgWidth;
		int iy = i / imgWidth;

		if (ix - 1 < 0 || iy - 1 < 0 || ix + 1 > imgWidth - 1 || iy + 1 > imgHeight - 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = *(pSrcData + srcPit * iy + ix * srcBitCount);
			continue;
		}

		double rgb[3] = {};
		for (int q = 0; q < 3; q++) {
			rgb[q] += *(pSrcData + srcPit * (iy - 1) + (ix - 1) * srcBitCount + q);
			rgb[q] += *(pSrcData + srcPit * (iy - 1) + (ix) * srcBitCount + q);
			rgb[q] += *(pSrcData + srcPit * (iy - 1) + (ix + 1) * srcBitCount + q);
			rgb[q] += *(pSrcData + srcPit * (iy)+ (ix - 1) * srcBitCount + q);
			rgb[q] += *(pSrcData + srcPit * (iy)+ (ix)* srcBitCount + q);
			rgb[q] += *(pSrcData + srcPit * (iy)+ (ix + 1) * srcBitCount + q);
			rgb[q] += *(pSrcData + srcPit * (iy + 1) + (ix - 1) * srcBitCount + q);
			rgb[q] += *(pSrcData + srcPit * (iy + 1) + (ix) * srcBitCount + q);
			rgb[q] += *(pSrcData + srcPit * (iy + 1) + (ix + 1) * srcBitCount + q);
			if (srcBitCount == 1) {
				break;
			}
		}
		/* 慎用！
#define ACCUMULATE(_x, _y) { \
		auto pixel=(byte*)(pSrcData + srcPit * _y + _x * srcBitCount); \
		rgb[0] += (double)pixel[0]; \
		rgb[1] += (double)pixel[1]; \
		rgb[2] += (double)pixel[2]; \
		}
		ACCUMULATE(ix - 1, iy - 1); 
		ACCUMULATE(ix, iy - 1); 
		ACCUMULATE(ix + 1, iy - 1);
		ACCUMULATE(ix - 1, iy);     
		ACCUMULATE(ix, iy);    
		ACCUMULATE(ix + 1, iy);
		ACCUMULATE(ix - 1, iy + 1); 
		ACCUMULATE(ix, iy + 1); 
		ACCUMULATE(ix + 1, iy + 1);
#undef ACCUMULATE*/

		for (int k = 0; k < 3; k++) {
			rgb[k] /= TEMPLATE_SIZE;
			if (rgb[k] < 0) {
				rgb[k] = 0;
			}
			else if (rgb[k] > 255) {
				rgb[k] = 255;
			}
		}
		if (imgBitCount == 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)(rgb[0]);
		}
		else {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)(rgb[0]);
			*(pImgData + imgPit * iy + ix * imgBitCount + 1) = (byte)(rgb[1]);
			*(pImgData + imgPit * iy + ix * imgBitCount + 2) = (byte)(rgb[2]);
		}
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_FILTER, 1, NULL);
	return 0;
}

static void GetGaussianTemplate(double t[3][3], double stddev)
{
	const int center = 1; // [[0, 1, 2], [0, 1, 2], [0, 1, 2]], center is (1, 1)
	double total = 0;
	static const double PI = acos(-1);
	for (int i = 0; i < 3; ++i)
	{
		double xsq = pow(i - center, 2.0);
		for (int j = 0; j < 3; ++j)
		{
			double ysq = pow(j - center, 2.0);
			double f = 1 / (2.0 * PI * stddev);
			double e = exp(-(xsq + ysq) / (2.0 * stddev * stddev));
			t[i][j] = f * e;
			total += t[i][j];
		}
	}
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			t[i][j] /= total;
}

UINT ImageProcess::gaussianFilter(LPVOID  p) {
	ThreadParam* param = (ThreadParam*)p;
	// 3*3模板
	const int SIZE = 3;
	double m[SIZE][SIZE];
	GetGaussianTemplate(m, param->stddev);

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();
	byte* pImgData = (byte*)param->img->GetBits();
	// 每个像素的字节数
	int imgBitCount = param->img->GetBPP() / 8;
	// GetPitch()图像的间距
	int imgPit = param->img->GetPitch();

	byte* pSrcData = (byte*)param->src->GetBits();
	// 每个像素的字节数
	int srcBitCount = param->src->GetBPP() / 8;
	// GetPitch()图像的间距
	int srcPit = param->src->GetPitch();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex;

	for (int i = startIndex; i <= endIndex; ++i) {
		int ix = i % imgWidth;
		int iy = i / imgWidth;

		if (ix - 1 < 0 || iy - 1 < 0 || ix + 1 > imgWidth - 1 || iy + 1 > imgHeight - 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = *(pSrcData + srcPit * iy + ix * srcBitCount);
			continue;
		}

		double rgb[3] = {};
		for (int q = 0; q < 3; q++) {
			rgb[q] += *(pSrcData + srcPit * (iy - 1) + (ix - 1) * srcBitCount + q) * m[0][0];
			rgb[q] += *(pSrcData + srcPit * (iy - 1) + (ix)*srcBitCount + q) * m[0][1];
			rgb[q] += *(pSrcData + srcPit * (iy - 1) + (ix + 1) * srcBitCount + q) * m[0][2];
			rgb[q] += *(pSrcData + srcPit * (iy)+(ix - 1) * srcBitCount + q) * m[1][0];
			rgb[q] += *(pSrcData + srcPit * (iy)+(ix)*srcBitCount + q) * m[1][1];
			rgb[q] += *(pSrcData + srcPit * (iy)+(ix + 1) * srcBitCount + q) * m[1][2];
			rgb[q] += *(pSrcData + srcPit * (iy + 1) + (ix - 1) * srcBitCount + q) * m[2][0];
			rgb[q] += *(pSrcData + srcPit * (iy + 1) + (ix)*srcBitCount + q) * m[2][1];
			rgb[q] += *(pSrcData + srcPit * (iy + 1) + (ix + 1) * srcBitCount + q) * m[2][2];
			if (srcBitCount == 1) {
				break;
			}
		}
		/* 慎用！
#define ACCUMULATE(_x, _y, _a, _b) { \
		auto _t = (byte*)(pSrcData + srcPit * _y + _x * srcBitCount); \
		rgb[0] += (double)_t[0]*m[_a][_b]; \
		rgb[1] += (double)_t[1]*m[_a][_b]; \
		rgb[2] += (double)_t[2]*m[_a][_b]; \
		}
		ACCUMULATE(ix - 1, iy - 1, 0, 0);
		ACCUMULATE(ix, iy - 1, 0, 1);
		ACCUMULATE(ix + 1, iy - 1, 0, 2);
		ACCUMULATE(ix - 1, iy, 1, 0);
		ACCUMULATE(ix, iy, 1, 1);
		ACCUMULATE(ix + 1, iy, 1, 2);
		ACCUMULATE(ix - 1, iy + 1, 2, 0);
		ACCUMULATE(ix, iy + 1, 2, 1);
		ACCUMULATE(ix + 1, iy + 1, 2, 2);
#undef ACCUMULATE*/
		for (int j = 0; j < 3; j++) {
			if (rgb[j] < 0) {
				rgb[j] = 0.0;
			}
			else if (rgb[j] > 255) {
				rgb[j] = 255.0;
			}
		}
		if (imgBitCount == 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)rgb[0];
		}
		else {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)rgb[0];
			*(pImgData + imgPit * iy + ix * imgBitCount + 1) = (byte)rgb[1];
			*(pImgData + imgPit * iy + ix * imgBitCount + 2) = (byte)rgb[2];
		}
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_FILTER, 1, NULL);
	return 0;
}

UINT ImageProcess::wienerFilter(LPVOID  p) {
#define OFFSET(x, y) (y * imgWidth + x - startIndex)
	ThreadParam* param = (ThreadParam*)p;

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();
	byte* pImgData = (byte*)param->img->GetBits();
	// 每个像素的字节数
	int imgBitCount = param->img->GetBPP() / 8;
	// GetPitch()图像的间距
	int imgPit = param->img->GetPitch();

	byte* pSrcData = (byte*)param->src->GetBits();
	// 每个像素的字节数
	int srcBitCount = param->src->GetBPP() / 8;
	// GetPitch()图像的间距
	int srcPit = param->src->GetPitch();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex;

	int len = endIndex - startIndex;
	double noise[3];
	double* mean[3], * variance[3];
	for (int ch = 0; ch < 3; ++ch)
	{
		mean[ch] = new double[len];
		variance[ch] = new double[len];
	}
	// loop #1: calc mean, var, and noise
	for (int idx = startIndex; idx < endIndex; ++idx)
	{
		int ix = idx % imgWidth;
		int iy = idx / imgWidth;
		auto offset = OFFSET(ix, iy);
		
		if (ix - 1 < 0 || iy - 1 < 0 || ix + 1 > imgWidth - 1 || iy + 1 > imgHeight - 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = *(pSrcData + srcPit * iy + ix * srcBitCount);
			continue;
		}

		byte* pixels[9];
		for (int i = -1, c = 0; i <= 1; ++i)
			for (int j = -1; j <= 1; ++j, ++c)
				pixels[c] = (byte*)(pSrcData + srcPit * (iy + j) + (ix + i) * srcBitCount);
		for (int ch = 0; ch < 3; ++ch) // RGB channels
		{
			mean[ch][offset] = 0.0;
			variance[ch][offset] = 0.0;
			for (int i = 0; i < 9; ++i)
				mean[ch][offset] += pixels[i][ch];
			mean[ch][offset] /= 9;
			for (int i = 0; i < 9; ++i)
				variance[ch][offset] += pow(pixels[i][ch] - mean[ch][offset], 2.0);
			variance[ch][offset] /= 9;
			noise[ch] += variance[ch][offset];
		}
	}
	for (int ch = 0; ch < 3; ++ch)
		noise[ch] /= len;
	// loop #2: do Wiener filter
	for (int idx = startIndex; idx < endIndex; ++idx)
	{
		int ix = idx % imgWidth;
		int iy = idx / imgWidth;
		auto offset = OFFSET(ix, iy);
		if (ix - 1 < 0 || iy - 1 < 0 || ix + 1 > imgWidth - 1 || iy + 1 > imgHeight - 1) {
			continue;
		}
		double rgb[3];
		auto pixel = (byte*)(pSrcData + srcPit * (iy) + (ix) * srcBitCount);
		for (int ch = 0; ch < 3; ++ch)
		{
			rgb[ch] = pixel[ch] - mean[ch][offset];
			double t = variance[ch][offset] - noise[ch];
			if (t < 0.0)
				t = 0.0;
			variance[ch][offset] = fmax(variance[ch][offset], noise[ch]);
			rgb[ch] = rgb[ch] / variance[ch][offset] * t + mean[ch][offset];
		}
		for (int k = 0; k < 3; k++) {
			if (rgb[k] < 0) {
				rgb[k] = 0;
			}
			else if (rgb[k] > 255) {
				rgb[k] = 255;
			}
		}
		if (imgBitCount == 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)(rgb[0]);
		}
		else {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)(rgb[0]);
			*(pImgData + imgPit * iy + ix * imgBitCount + 1) = (byte)(rgb[1]);
			*(pImgData + imgPit * iy + ix * imgBitCount + 2) = (byte)(rgb[2]);
		}
	}
	for (int ch = 0; ch < 3; ++ch)
	{
		delete[] mean[ch];
		delete[] variance[ch];
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_FILTER, 1, NULL);
	return 0;
#undef OFFSET
}

UINT ImageProcess::bilateralFilter(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;

	const int r = 1; // 滤波器中心到边缘的距离
	const int w_filter = 2 * r + 1; // 滤波器边长

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();
	byte* pImgData = (byte*)param->img->GetBits();
	// 每个像素的字节数
	int imgBitCount = param->img->GetBPP() / 8;
	// GetPitch()图像的间距
	int imgPit = param->img->GetPitch();

	byte* pSrcData = (byte*)param->src->GetBits();
	// 每个像素的字节数
	int srcBitCount = param->src->GetBPP() / 8;
	// GetPitch()图像的间距
	int srcPit = param->src->GetPitch();
	
	// spatial weight
	double** d_matrix = new double* [w_filter];
	for (int q = 0; q < w_filter; q++) {
		d_matrix[q] = new double[w_filter];
	}  

	// compute spatial weight
	for (int i = -r; i <= r; i++)
		for (int j = -r; j <= r; j++)
		{
			int x = j + r;
			int y = i + r;

			d_matrix[y][x] = exp((i * i + j * j) * (-0.5 / (param->sigma_d * param->sigma_d)));
		}

	// bilateral filter
	int startIndex = param->startIndex;
	int endIndex = param->endIndex;
	for (int i = startIndex; i <= endIndex; ++i) {
		int ix = i % imgWidth;
		int iy = i / imgWidth;

		// 3*3模板
		if (ix - 1 < 0 || iy - 1 < 0 || ix + 1 > imgWidth - 1 || iy + 1 > imgHeight - 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = *(pSrcData + srcPit * iy + ix * srcBitCount);
			continue;
		}

		double weight_sum[3] = {};
		double pixel_sum[3] = {};
		int pixel_diff[3] = {};

		for (int m = -r; m <= r; m++) // 行
		{
			for (int n = -r; n <= r; n++) // 列
			{
				if (m * m + n * n > r* r) continue;

				int x = ix + n;
				int y = iy + m;

				for (int q = 0; q < 3; q++) {
					pixel_diff[q] = abs(*(pSrcData + srcPit * iy + ix * srcBitCount + q) - *(pSrcData + srcPit * y + x * srcBitCount + q));
					// 复合权重
					double weight_tmp = d_matrix[m + r][n + r] * exp(-0.5 * pixel_diff[q] * pixel_diff[q] / (param->sigma_r * param->sigma_r));
					pixel_sum[q] += *(pSrcData + srcPit * y + x * srcBitCount + q) * weight_tmp;
					weight_sum[q] += weight_tmp;
				}
				
			}
		}
		for (int q = 0; q < 3; q++) {
			pixel_sum[q] = pixel_sum[q] / weight_sum[q];
			if (pixel_sum[q] < 0) {
				pixel_sum[q] = 0.0;
			}
			else if (pixel_sum[q] > 255) {
				pixel_sum[q] = 255.0;
			}
		}
		if (imgBitCount == 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)pixel_sum[0];
		}
		else {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)pixel_sum[0];
			*(pImgData + imgPit * iy + ix * imgBitCount + 1) = (byte)pixel_sum[1];
			*(pImgData + imgPit * iy + ix * imgBitCount + 2) = (byte)pixel_sum[2];
		}
	}

	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_FILTER, 1, NULL);
	return 0;
}

