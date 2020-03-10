#include "pch.h"
#include "framework.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>

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

	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	int startIndex = param->startIndex;
	int endIndex = param->endIndex;
	// MAX_SPAN 15
	int maxSpan = param->maxSpan;
	// 移动窗口 最大尺寸 31*31像素
	int maxLength = (maxSpan * 2 + 1) * (maxSpan * 2 + 1);

	byte* pRealData = (byte*)param->src->GetBits();
	int pit = param->src->GetPitch();
	int bitCount = param->src->GetBPP() / 8;

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
	/*
	// 传递消息
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MEDIAN_FILTER, 1, NULL);*/
	return 0;
}

// 椒盐噪声
UINT ImageProcess::saltNoise(LPVOID  p)
{
	ThreadParam* param = (ThreadParam*)p;
	// 像素为单位 
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex;
	// GetBits()指向位图实际像素值的指针
	byte* pRealData = (byte*)param->src->GetBits();
	// GetBPP()每个像素的位数
	// 每个像素的字节数
	int bitCount = param->src->GetBPP() / 8;
	// GetPitch()图像的间距
	int pit = param->src->GetPitch();

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
	/*
	// 传递消息
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_NOISE, 1, NULL);*/
	return 0;
}