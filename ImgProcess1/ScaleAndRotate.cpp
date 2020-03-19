#include "pch.h"
#include "framework.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>

#include "CLAgent.h"
using namespace std;

static double cubicWeight(double x)
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

double cubicHermite(double A, double B, double C, double D, double t)
{
	double a = -A / 2.0 + (3.0 * B) / 2.0 - (3.0 * C) / 2.0 + D / 2.0;
	double b = A - (5.0 * B) / 2.0 + 2.0 * C - D / 2.0;
	double c = -A / 2.0 + C / 2.0;
	double d = B;
	return a * t * t * t + b * t * t + c * t + d;
}


UINT ImageProcess::cubicScale(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;

	// img data
	//int imgWidth = param->img->GetWidth();
	//int imgHeight = param->img->GetHeight();
	int imgWidth = (int)((param->xscale<1?param->xscale:1) * param->src->GetWidth());
	int imgHeight = (int)((param->yscale<1?param->yscale:1) * param->src->GetHeight());
	byte* pImgData = (byte*)param->img->GetBits();
	// 每个像素的字节数
	int imgBitCount = param->img->GetBPP() / 8;
	// GetPitch()图像的间距
	int imgPit = param->img->GetPitch();
	
	int startIndex = param->startIndex;
	int endIndex = param->endIndex;

	// src data
	int srcWidth = param->src->GetWidth();
	int srcHeight = param->src->GetHeight();
	byte* pSrcData = (byte*)param->src->GetBits();
	// 每个像素的字节数
	int srcBitCount = param->src->GetBPP() / 8;
	// GetPitch()图像的间距
	int srcPit = param->src->GetPitch();
	
	for (int i = startIndex; i <= endIndex; ++i)
	{
		int ix = i % imgWidth;
		int iy = i / imgWidth;

		double x = ix / (double)param->xscale;
		double y = iy / (double)param->yscale;
		int sx = (int)x, sy = (int)y;

		if (sx - 1 <= 0 || sx + 2 >= srcWidth - 1 || sy - 1 <= 0 || sy + 2 >= srcHeight - 1)
		{
			sx = sx < 0 ? 0 : sx;
			sx = sx > srcWidth - 1 ? srcWidth - 1 : sx; 
			sy = sy < 0 ? 0 : sy;
			sy = sy > srcHeight - 1 ? srcHeight - 1 : sy;
			*(pImgData + imgPit * iy + ix * imgBitCount) = *(pSrcData + srcPit * sy + sx * srcBitCount);
			continue;
		}

		// Calc w
		double wx[4], wy[4];
		wx[0] = cubicWeight(sx - 1 - x);
		wx[1] = cubicWeight(sx + 0 - x);
		wx[2] = cubicWeight(sx + 1 - x);
		wx[3] = cubicWeight(sx + 2 - x);
		wy[0] = cubicWeight(sy - 1 - y);
		wy[1] = cubicWeight(sy + 0 - y);
		wy[2] = cubicWeight(sy + 1 - y);
		wy[3] = cubicWeight(sy + 2 - y);

		// Get 4*4 pixels
		byte* p[4][4];
#define FILLPX(x, y, i, j) p[i][j]=(byte*)pSrcData + srcPit * sy + sx * srcBitCount
		FILLPX(sx - 1, sy - 1, 0, 0);
		FILLPX(sx - 1, sy + 0, 0, 1);
		FILLPX(sx - 1, sy + 1, 0, 2);
		FILLPX(sx - 1, sy + 2, 0, 3);
		FILLPX(sx + 0, sy - 1, 1, 0);
		FILLPX(sx + 0, sy + 0, 1, 1);
		FILLPX(sx + 0, sy + 1, 1, 2);
		FILLPX(sx + 0, sy + 2, 1, 3);
		FILLPX(sx + 1, sy - 1, 2, 0);
		FILLPX(sx + 1, sy + 0, 2, 1);
		FILLPX(sx + 1, sy + 1, 2, 2);
		FILLPX(sx + 1, sy + 2, 2, 3);
		FILLPX(sx + 2, sy - 1, 3, 0);
		FILLPX(sx + 2, sy + 0, 3, 1);
		FILLPX(sx + 2, sy + 1, 3, 2);
		FILLPX(sx + 2, sy + 2, 3, 3);
#undef FILLPX

		double rgb[3] = { 0.0 };
		for (int m = 0; m < 4; ++m) {
			for (int n = 0; n < 4; ++n) {
				rgb[0] += p[m][n][0] * wx[m] * wy[n];
				rgb[1] += p[m][n][1] * wx[m] * wy[n];
				rgb[2] += p[m][n][2] * wx[m] * wy[n];
			}
		}

		for (int j = 0; j < 3; j++) {
			if (rgb[j] < 0) {
				rgb[j] = 0;
			}
			else if (rgb[j] > 255) {
				rgb[j] = 255;
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
	::PostMessageW(AfxGetMainWnd()->GetSafeHwnd(), WM_INTERPOLATION, 1, NULL);
	return 0;
}

UINT ImageProcess::cubicScaleCL(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;
	// img data
	//int imgWidth = param->img->GetWidth();
	//int imgHeight = param->img->GetHeight();
	int imgWidth = (int)((param->xscale < 1 ? param->xscale : 1) * param->src->GetWidth());
	int imgHeight = (int)((param->yscale < 1 ? param->yscale : 1) * param->src->GetHeight());
	byte* pImgData = (byte*)param->img->GetBits();
	// 每个像素的字节数
	int imgBitCount = param->img->GetBPP() / 8;
	// GetPitch()图像的间距
	int imgPit = param->img->GetPitch();
	size_t imgMemSize = (-imgPit) * imgHeight * sizeof(byte);
	byte* imgMemStartAt = pImgData + imgPit * (imgHeight - 1);

	// src data
	int srcWidth = param->src->GetWidth();
	int srcHeight = param->src->GetHeight();
	byte* pSrcData = (byte*)param->src->GetBits();
	// 每个像素的字节数
	int srcBitCount = param->src->GetBPP() / 8;
	// GetPitch()图像的间距
	int srcPit = param->src->GetPitch();
	size_t srcMemSize = (-srcPit) * srcHeight * sizeof(byte);
	byte* srcMemStartAt = pSrcData + srcPit * (srcHeight - 1);

	DECLARE_CLA(cla);
	VERIFY(cla->LoadKernel("Scale"));
	auto inmem = cla->CreateMemoryBuffer(srcMemSize, srcMemStartAt);
	VERIFY(inmem != nullptr);
	auto outmem = cla->CreateMemoryBuffer(imgMemSize, imgMemStartAt);
	VERIFY(outmem != nullptr);
	cla->SetKernelArg(0, sizeof(inmem), &inmem);
	cla->SetKernelArg(1, sizeof(outmem), &outmem);
	cla->SetKernelArg(2, sizeof(int), &srcWidth);
	cla->SetKernelArg(3, sizeof(int), &srcHeight);
	cla->SetKernelArg(4, sizeof(int), &imgWidth);
	cla->SetKernelArg(5, sizeof(int), &imgHeight);
	cla->SetKernelArg(6, sizeof(int), &srcPit);
	cla->SetKernelArg(7, sizeof(int), &imgPit);
	cla->SetKernelArg(8, sizeof(float), &param->xscale);
	cla->SetKernelArg(9, sizeof(float), &param->yscale);
	cla->SetKernelArg(10, sizeof(int), &srcBitCount);
	constexpr auto WORKDIM = 2;
	size_t localws[WORKDIM] = { 16, 16 };
	size_t globalws[WORKDIM] = {
		_RoundUp(localws[0], imgWidth),
		_RoundUp(localws[1], imgHeight),
	};
	// 真正的开始时间！！
	//DA->StartTick();
	VERIFY(cla->RunKernel(WORKDIM, localws, globalws));
	cla->ReadBuffer(outmem, imgMemSize, imgMemStartAt);
	cla->Cleanup();

	::PostMessageW(AfxGetMainWnd()->GetSafeHwnd(), WM_INTERPOLATION, 1, NULL);
	return 0;
}

UINT ImageProcess::cubicRotate(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();
	byte* pImgData = (byte*)param->img->GetBits();
	// 每个像素的字节数
	int imgBitCount = param->img->GetBPP() / 8;
	// GetPitch()图像的间距
	int imgPit = param->img->GetPitch();

	int srcWidth = param->src->GetWidth();
	int srcHeight = param->src->GetHeight();
	byte* pSrcData = (byte*)param->src->GetBits();
	// 每个像素的字节数
	int srcBitCount = param->src->GetBPP() / 8;
	// GetPitch()图像的间距
	int srcPit = param->src->GetPitch();

	const double sina = sin(param->alpha), cosa = cos(param->alpha);
	int imgCenter[2] = { imgWidth / 2,imgHeight / 2 };
	int srcCenter[2] = { srcWidth / 2,srcHeight / 2 };

	for (int i = param->startIndex; i < param->endIndex; ++i)
	{
		int ix = i % imgWidth;
		int iy = i / imgWidth;

		int xx = ix - imgCenter[0];
		int yy = iy - imgCenter[1];

		double x = xx * cosa - yy * sina + srcCenter[0];
		double y = xx * sina + yy * cosa + srcCenter[1];
		int sx = (int)x, sy = (int)y;

		if (sx - 1 <= 0 || sx + 2 >= srcWidth - 1 || sy - 1 <= 0 || sy + 2 >= srcHeight - 1)
		{
			if (sx >= 0 && sx < srcWidth && sy >= 0 && sy < srcHeight) {
				*(pImgData + imgPit * iy + ix * imgBitCount) = *(pSrcData + srcPit * sy + sx * srcBitCount);
			}
			continue;
		}

		// Get 4*4 pixels
		// 1st row
		auto p00 = (byte*)pSrcData + srcPit * (sy - 1) + (sx - 1) * srcBitCount;
		auto p01 = (byte*)pSrcData + srcPit * (sy - 1) + (sx + 0) * srcBitCount;
		auto p02 = (byte*)pSrcData + srcPit * (sy - 1) + (sx + 1) * srcBitCount;
		auto p03 = (byte*)pSrcData + srcPit * (sy - 1) + (sx + 2) * srcBitCount;

		// 2nd row
		auto p10 = (byte*)pSrcData + srcPit * (sy + 0) + (sx - 1) * srcBitCount;
		auto p11 = (byte*)pSrcData + srcPit * (sy + 0) + (sx + 0) * srcBitCount;
		auto p12 = (byte*)pSrcData + srcPit * (sy + 0) + (sx + 1) * srcBitCount;
		auto p13 = (byte*)pSrcData + srcPit * (sy + 0) + (sx + 2) * srcBitCount;

		// 3rd row
		auto p20 = (byte*)pSrcData + srcPit * (sy + 1) + (sx - 1) * srcBitCount;
		auto p21 = (byte*)pSrcData + srcPit * (sy + 1) + (sx + 0) * srcBitCount;
		auto p22 = (byte*)pSrcData + srcPit * (sy + 1) + (sx + 1) * srcBitCount;
		auto p23 = (byte*)pSrcData + srcPit * (sy + 1) + (sx + 2) * srcBitCount;

		// 4th row
		auto p30 = (byte*)pSrcData + srcPit * (sy + 2) + (sx - 1) * srcBitCount;
		auto p31 = (byte*)pSrcData + srcPit * (sy + 2) + (sx + 0) * srcBitCount;
		auto p32 = (byte*)pSrcData + srcPit * (sy + 2) + (sx + 1) * srcBitCount;
		auto p33 = (byte*)pSrcData + srcPit * (sy + 2) + (sx + 2) * srcBitCount;

		

		double rgb[3] = {};
		for (int j = 0; j < 3; j++) {
			double col0 = cubicHermite(p00[j], p10[j], p20[j], p30[j], x - sx);
			double col1 = cubicHermite(p01[j], p11[j], p21[j], p31[j], x - sx);
			double col2 = cubicHermite(p02[j], p12[j], p22[j], p32[j], x - sx);
			double col3 = cubicHermite(p03[j], p13[j], p23[j], p33[j], x - sx);
			
			rgb[j] = cubicHermite(col0, col1, col2, col3, y - sy);
			if (rgb[j] < 0) {
				rgb[j] = 0;
			}
			else if (rgb[j] > 255) {
				rgb[j] = 255;
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
	::PostMessageW(AfxGetMainWnd()->GetSafeHwnd(), WM_INTERPOLATION, 1, NULL);
	return 0;
}

UINT ImageProcess::cubicRotateCL(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();
	byte* pImgData = (byte*)param->img->GetBits();
	// 每个像素的字节数
	int imgBitCount = param->img->GetBPP() / 8;
	// GetPitch()图像的间距
	int imgPit = param->img->GetPitch();
	size_t imgMemSize = (-imgPit) * imgHeight * sizeof(byte);
	byte* imgMemStartAt = pImgData + imgPit * (imgHeight - 1);

	int srcWidth = param->src->GetWidth();
	int srcHeight = param->src->GetHeight();
	byte* pSrcData = (byte*)param->src->GetBits();
	// 每个像素的字节数
	int srcBitCount = param->src->GetBPP() / 8;
	// GetPitch()图像的间距
	int srcPit = param->src->GetPitch();
	size_t srcMemSize = (-srcPit) * srcHeight * sizeof(byte);
	byte* srcMemStartAt = pSrcData + srcPit * (srcHeight - 1);

	const double sina = sin(param->alpha), cosa = cos(param->alpha);

	DECLARE_CLA(cla);
	VERIFY(cla->LoadKernel("Rotate"));
	auto inmem = cla->CreateMemoryBuffer(srcMemSize, srcMemStartAt);
	VERIFY(inmem != nullptr);
	auto outmem = cla->CreateMemoryBuffer(imgMemSize, imgMemStartAt);
	VERIFY(outmem != nullptr);
	cla->SetKernelArg(0, sizeof(inmem), &inmem);
	cla->SetKernelArg(1, sizeof(outmem), &outmem);
	cla->SetKernelArg(2, sizeof(int), &srcWidth);
	cla->SetKernelArg(3, sizeof(int), &srcHeight);
	cla->SetKernelArg(4, sizeof(int), &imgWidth);
	cla->SetKernelArg(5, sizeof(int), &imgHeight);
	cla->SetKernelArg(6, sizeof(int), &srcPit);
	cla->SetKernelArg(7, sizeof(int), &imgPit);
	cla->SetKernelArg(8, sizeof(double), &sina);
	cla->SetKernelArg(9, sizeof(double), &cosa);
	cla->SetKernelArg(10, sizeof(int), &srcBitCount);
	constexpr auto WORKDIM = 2;
	size_t localws[WORKDIM] = { 16, 16 };
	size_t globalws[WORKDIM] = {
		_RoundUp(localws[0], imgWidth),
		_RoundUp(localws[1], imgHeight),
	};
	//DA->StartTick();
	VERIFY(cla->RunKernel(WORKDIM, localws, globalws));
	cla->ReadBuffer(outmem, imgMemSize, imgMemStartAt);
	cla->Cleanup();

	::PostMessageW(AfxGetMainWnd()->GetSafeHwnd(), WM_INTERPOLATION, 1, NULL);
	return 0;
}



