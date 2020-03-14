#include "pch.h"
#include "framework.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>
using namespace std;

static double BoxMullerGenerator(double mean, double stddev) {
	static const double twopi = 2.0 * acos(-1);
	double u1, u2;
	static double z0, z1;
	static bool generate = false;
	generate = !generate;
	if (!generate)
		return z1 * stddev + mean;
	do
	{
		u1 = (double)rand() / RAND_MAX;
		u2 = (double)rand() / RAND_MAX;
	} while (u1 <= DBL_MIN);
	z0 = sqrt(-2.0 * log(u1)) * cos(twopi * u2);
	z1 = sqrt(-2.0 * log(u1)) * sin(twopi * u2);
	return z0 * stddev + mean;
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

UINT ImageProcess::gaussianNoise(LPVOID p)
{
	// 要设置随机数种子，否则短时间内产生的随机数一样
	srand(GetCurrentThreadId());
	ThreadParam* param = (ThreadParam*)p;
	int startIndex = param->startIndex;
	int endIndex = param->endIndex;

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();

	byte* pRealData = (byte*)param->img->GetBits();
	int bitCount = param->img->GetBPP() / 8;
	int pit = param->img->GetPitch();

	for (int i = startIndex; i<=endIndex; ++i)
	{
		int x = i % imgWidth;
		int y = i / imgWidth;
		auto pixel = (byte*)(pRealData + pit * y + x * bitCount);
		for (int j = 0; j < 3; ++j)
		{
			double val = pixel[j] + BoxMullerGenerator(param->mean, param->stddev);
			if (val > 255.0) {
				val = 255.0;
			}
			else if (val < 0.0) {
				val = 0.0;
			}
			pixel[j] = (byte)val;

			if (bitCount == 1) {
				break;
			}
		}
	}
	// 传递消息
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_NOISE, 1, NULL);
	return 0;
}
