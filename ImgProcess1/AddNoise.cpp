#include "pch.h"
#include "framework.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>
using namespace std;


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
