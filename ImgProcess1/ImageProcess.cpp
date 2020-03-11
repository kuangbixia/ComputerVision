#include "pch.h"
#include "framework.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>
using namespace std;

static double BicubicWeight(double x)
{
	// 一般设为-0.5或-0.75
	constexpr double a = -0.5;
	x = std::abs(x);
	if (x < 1.0)
		return (a + 2.0) * x * x * x - (a + 3.0) * x * x + 1.0;
	else if (x < 2.0)
		return a * x * x * x - 5.0 * a * x * x + 8.0 * a * x - 4.0 * a;
	return 0.0;
}

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

UINT ImageProcess::cubicScale(LPVOID p, float xscale, float yscale, CImage* src)
{
	ThreadParam* param = (ThreadParam*)p;
	
	for (int i = param->startIndex; i < param->endIndex; ++i)
	{
		int imgWidth = param->img->GetWidth();
		int imgHeight = param->img->GetHeight();
		int srcWidth = src->GetWidth();
		int srcHeight = src->GetHeight();
		int ix = i % imgWidth;
		int iy = i / imgWidth;
		double x = ix / ((double)imgWidth / srcWidth);
		double y = iy / ((double)imgHeight / srcHeight);
		int fx = (int)x, fy = (int)y;

		// Handle the border
		if (fx - 1 <= 0 || fx + 2 >= srcWidth - 1 || fy - 1 <= 0 || fy + 2 >= srcHeight - 1)
		{
			fx = fx < 0 ? 0 : fx;
			fx = fx >= srcWidth ? srcWidth - 1 : fx;
			fy = fy < 0 ? 0 : fy;
			fy = fy >= srcHeight ? srcHeight - 1 : fy;
			param->img->SetPixel(ix, iy, src->GetPixel(fx, fy));
			continue;
		}
		// Calc w
		double wx[4], wy[4];
		wx[0] = BicubicWeight(fx - 1 - x);
		wx[1] = BicubicWeight(fx + 0 - x);
		wx[2] = BicubicWeight(fx + 1 - x);
		wx[3] = BicubicWeight(fx + 2 - x);
		wy[0] = BicubicWeight(fy - 1 - y);
		wy[1] = BicubicWeight(fy + 0 - y);
		wy[2] = BicubicWeight(fy + 1 - y);
		wy[3] = BicubicWeight(fy + 2 - y);

		// Get pixels
		byte* p[4][4];
		byte* pSrcData = (byte*)src->GetBits();
		// GetBPP()每个像素的位数
		// 每个像素的字节数
		int srcBitCount = src->GetBPP() / 8;
		// GetPitch()图像的间距
		int srcPit = src->GetPitch();
#define FILLPX(x, y, i, j) p[i][j]=pSrcData + srcPit * fy + fx * srcBitCount
		FILLPX(fx - 1, fy - 1, 0, 0);
		FILLPX(fx - 1, fy + 0, 0, 1);
		FILLPX(fx - 1, fy + 1, 0, 2);
		FILLPX(fx - 1, fy + 2, 0, 3);
		FILLPX(fx + 0, fy - 1, 1, 0);
		FILLPX(fx + 0, fy + 0, 1, 1);
		FILLPX(fx + 0, fy + 1, 1, 2);
		FILLPX(fx + 0, fy + 2, 1, 3);
		FILLPX(fx + 1, fy - 1, 2, 0);
		FILLPX(fx + 1, fy + 0, 2, 1);
		FILLPX(fx + 1, fy + 1, 2, 2);
		FILLPX(fx + 1, fy + 2, 2, 3);
		FILLPX(fx + 2, fy - 1, 3, 0);
		FILLPX(fx + 2, fy + 0, 3, 1);
		FILLPX(fx + 2, fy + 1, 3, 2);
		FILLPX(fx + 2, fy + 2, 3, 3);
#undef FILLPX

		double rgb[3];
		rgb[0] = rgb[1] = rgb[2] = 0.0;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
			{
				rgb[0] += p[i][j][0] * wx[i] * wy[j];
				rgb[1] += p[i][j][1] * wx[i] * wy[j];
				rgb[2] += p[i][j][2] * wx[i] * wy[j];
			}
		/*for (int i = 0; i < 3; ++i)
			rgb[i] = std::clamp(rgb[i], 0.0, 255.0);*/
		byte* pImgData = (byte*)param->img->GetBits();
		// GetBPP()每个像素的位数
		// 每个像素的字节数
		int imgBitCount = param->img->GetBPP() / 8;
		// GetPitch()图像的间距
		int imgPit = param->img->GetPitch();
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (imgBitCount == 1) {
					*(pImgData + imgPit * iy + ix * imgBitCount) = p[i][j][0] * wx[i] * wy[j];
				}
				else {
					*(pImgData + imgPit * iy + ix * imgBitCount) = p[i][j][0] * wx[i] * wy[j];
					*(pImgData + imgPit * iy + ix * imgBitCount + 1) = p[i][j][1] * wx[i] * wy[j];
					*(pImgData + imgPit * iy + ix * imgBitCount + 2) = p[i][j][2] * wx[i] * wy[j];
				}
			}
		}
	}
	::PostMessageW(AfxGetMainWnd()->GetSafeHwnd(), WM_SCALE, 1, NULL);
	return 0;
}

// 椒盐噪声
UINT ImageProcess::saltNoise(LPVOID  p)
{
	ThreadParam* param = (ThreadParam*)p;
	// 像素为单位 
	int maxWidth = param->img->GetWidth();
	int maxHeight = param->img->GetHeight();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex;
	// GetBits()指向位图实际像素值的指针
	byte* pRealData = (byte*)param->img->GetBits();
	// GetBPP()每个像素的位数
	// 每个像素的字节数
	int bitCount = param->img->GetBPP() / 8;
	// GetPitch()图像的间距
	int pit = param->img->GetPitch();

	// 要设置随机数种子，否则短时间内产生的随机数一样
	srand(GetCurrentThreadId());
	for (int i = startIndex; i <= endIndex; ++i)
	{
		int x = i % maxWidth;
		int y = i / maxWidth;
		// NOISE 0.2
		if ((rand() % 1000) * 0.001 < NOISE)
		{
			int value;
			if (rand() % 1000 < 500)
			{
				value = 0;
			}
			else
			{
				value = 255;
			}
			if (bitCount == 1)
			{
				*(pRealData + pit * y + x * bitCount) = value;
			}
			else // RGB
			{
				*(pRealData + pit * y + x * bitCount) = value;
				*(pRealData + pit * y + x * bitCount + 1) = value;
				*(pRealData + pit * y + x * bitCount + 2) = value;
			}
		}
	}

	// 传递消息
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_NOISE, 1, NULL);
	return 0;
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
