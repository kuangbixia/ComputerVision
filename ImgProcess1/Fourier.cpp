#include "pch.h"
#include "framework.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>
#include "CLAgent.h"

using namespace std;

UINT ImageProcess::fourierTransform(LPVOID p) {
	const double PI = acos(-1);
	ThreadParam* param = (ThreadParam*)p;

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();
	byte* pImgData = (byte*)param->img->GetBits();
	int imgBitCount = param->img->GetBPP() / 8;
	int imgPit = param->img->GetPitch();

	int srcWidth = param->src->GetWidth();
	int srcHeight = param->src->GetHeight();
	byte* pSrcData = (byte*)param->src->GetBits();
	int srcBitCount = param->src->GetBPP() / 8;
	int srcPit = param->src->GetPitch();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex;

	for (int i = startIndex; i <= endIndex; ++i) {
		int ix = i % imgWidth;
		int iy = i / imgWidth;
		double real = 0.0, imag = 0.0;
		for (int sy = 0; sy < srcHeight; ++sy)
		{
			for (int sx = 0; sx < srcWidth; ++sx)
			{
				auto pixel = (byte*)(pSrcData + srcPit * sy + sx * srcBitCount);
				double gray = 0.299 * pixel[0];
				if (imgBitCount != 1) {
					gray += (0.587 * pixel[1] + 0.114 * pixel[2]);
				}
				if ((sx + sy) & 1) // centralize
					gray = -gray;
				double A = 2 * PI * ((double)ix * (double)sx / (double)srcWidth + (double)iy * (double)sy / (double)srcHeight);
				real += gray * cos(A); // Êµ²¿
				imag -= gray * sin(A); // Ðé²¿
			}
		}
		double value = sqrt(real * real + imag * imag);
		value = FOURIER_FACTOR * log(value + 1);
		value = value < 0 ? 0 : ((value > 255) ? 255 : value);
		if (imgBitCount == 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)(value);
		}
		else {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)(value);
			*(pImgData + imgPit * iy + ix * imgBitCount + 1) = (byte)(value);
			*(pImgData + imgPit * iy + ix * imgBitCount + 2) = (byte)(value);
		}
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_FOURIER, 1, NULL);
	return 0;
}

UINT ImageProcess::fourierTransformCL(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();
	byte* pImgData = (byte*)param->img->GetBits();
	int imgBitCount = param->img->GetBPP() / 8;
	int imgPit = param->img->GetPitch();
	size_t imgMemSize = (-imgPit) * imgHeight * sizeof(byte);
	byte* imgMemStartAt = pImgData + imgPit * (imgHeight - 1);

	int srcWidth = param->src->GetWidth();
	int srcHeight = param->src->GetHeight();
	byte* pSrcData = (byte*)param->src->GetBits();
	int srcBitCount = param->src->GetBPP() / 8;
	int srcPit = param->src->GetPitch();
	size_t srcMemSize = (-srcPit) * srcHeight * sizeof(byte);
	byte* srcMemStartAt = pSrcData + srcPit * (srcHeight - 1);

	DECLARE_CLA(cla);
	VERIFY(cla->LoadKernel("Fourier"));
	//DA->StartTick();
	auto inmem = cla->CreateMemoryBuffer(srcMemSize, srcMemStartAt);
	VERIFY(inmem != nullptr);
	auto outmem = cla->CreateMemoryBuffer(imgMemSize, imgMemStartAt);
	VERIFY(outmem != nullptr);
	cla->SetKernelArg(0, sizeof(inmem), &inmem);
	cla->SetKernelArg(1, sizeof(outmem), &outmem);
	cla->SetKernelArg(2, sizeof(int), &srcWidth);
	cla->SetKernelArg(3, sizeof(int), &srcHeight);
	cla->SetKernelArg(4, sizeof(int), &srcPit);
	cla->SetKernelArg(5, sizeof(int), &srcBitCount);
	constexpr auto WORKDIM = 2;
	size_t localws[WORKDIM] = { 16, 16 };
	size_t globalws[WORKDIM] = {
		_RoundUp(localws[0], imgWidth),
		_RoundUp(localws[1], imgHeight),
	};
	VERIFY(cla->RunKernel(WORKDIM, localws, globalws));
	cla->ReadBuffer(outmem, imgMemSize, imgMemStartAt);
	cla->Cleanup();

	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_FOURIER, 1, NULL);
	return 0;
}
