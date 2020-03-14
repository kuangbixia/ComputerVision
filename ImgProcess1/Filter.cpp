#include "pch.h"
#include "framework.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>
using namespace std;


static bool GetValue(int p[], int size, int& value)
{
	//�����м��ֵ�����˲������ڵ��м�λ�ã�Ҳ���Ǳ���������λ��
	int zxy = p[(size - 1) / 2];
	//���ڼ�¼ԭ������±�
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

	if (zmax > zmed&& zmin < zmed) { // 1. zmed��������
		if (zxy > zmin&& zxy < zmax) // 2.1 zxy���������㣬���滻
			value = (size - 1) / 2;
		else
			value = a[(size - 1) / 2]; // 2.2 zxy����������zmed�滻
		delete[]a;
		return true;
	}
	else { // �˳��������˲����Ĵ��ڳߴ磬����Ѱ�Ҳ���������zmed
		delete[]a;
		return false;
	}

}


// ����Ӧ��ֵ�˲�
// �ı䴰�ڵĴ�С���жϵ�ǰ�����ǲ��������������滻���������滻
UINT ImageProcess::medianFilter(LPVOID  p)
{
	ThreadParam* param = (ThreadParam*)p;

	int maxWidth = param->img->GetWidth();
	int maxHeight = param->img->GetHeight();
	int startIndex = param->startIndex;
	int endIndex = param->endIndex;
	// MAX_SPAN 15
	int maxSpan = param->maxSpan;
	// �ƶ����� ���ߴ� 31*31����
	int maxLength = (maxSpan * 2 + 1) * (maxSpan * 2 + 1);

	byte* pRealData = (byte*)param->img->GetBits();
	int pit = param->img->GetPitch();
	int bitCount = param->img->GetBPP() / 8;

	int* pixel = new int[maxLength];//�洢ÿ�����ص�ĻҶ�
	int* pixelR = new int[maxLength];
	int* pixelB = new int[maxLength];
	int* pixelG = new int[maxLength];
	int index = 0;
	for (int i = startIndex; i <= endIndex; ++i)
	{
		int Sxy = 1;
		int med = 0;
		int state = 0;
		// ����Ϊ��λ
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
			// �Ѿ�ȷ��med����Ϊzxy�����ģ��������أ���zmed��������ֵ��
			if ((state = GetValue(pixel, index, med)) == 1) // index��pixel��Ԫ�ظ���
				break;

			// �����˲������ڳߴ磬�Ҳ����������zmed
			Sxy++;
		};

		if (state) // ������������
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

	// ������Ϣ
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_FILTER, 1, NULL);
	return 0;
}

UINT ImageProcess::meanFilter(LPVOID  p) {
	ThreadParam* param = (ThreadParam*)p;

	int imgWidth = param->img->GetWidth();
	int imgHeight = param->img->GetHeight();
	byte* pImgData = (byte*)param->img->GetBits();
	// ÿ�����ص��ֽ���
	int imgBitCount = param->img->GetBPP() / 8;
	// GetPitch()ͼ��ļ��
	int imgPit = param->img->GetPitch();

	byte* pSrcData = (byte*)param->src->GetBits();
	// ÿ�����ص��ֽ���
	int srcBitCount = param->src->GetBPP() / 8;
	// GetPitch()ͼ��ļ��
	int srcPit = param->src->GetPitch();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex;

	// 3*3 ģ��
	const int TEMPLATE_SIZE = 3 * 3;
	for (int i = startIndex; i <= endIndex; ++i){
		int ix = i % imgWidth;
		int iy = i / imgWidth;

		if (ix - 1 < 0 || iy - 1 < 0 || ix + 1 > imgWidth - 1 || iy + 1 > imgHeight - 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = *(pSrcData + srcPit * iy + ix * srcBitCount);
			continue;
		}

		double rgb[3] = {};
#define ACCUMULATE(_x, _y) { \
		auto pixel=(byte*)(pSrcData + srcPit * _y + _x * srcBitCount); \
		rgb[0] += pixel[0]; \
		rgb[1] += pixel[1]; \
		rgb[2] += pixel[2]; \
		}
		ACCUMULATE(ix - 1, iy - 1); 
		ACCUMULATE(ix, iy - 1); 
		ACCUMULATE(ix + 1, iy - 1);
		ACCUMULATE(ix - 1, iy);     
		ACCUMULATE(ix, iy);    
		ACCUMULATE(ix + 1, iy);
		ACCUMULATE(ix - 1, iy + 1); 
		ACCUMULATE(ix, iy + 1); 
		ACCUMULATE(ix + 1, iy - 1);
#undef ACCUMULATE

		if (imgBitCount == 1) {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)(rgb[0] / TEMPLATE_SIZE);
		}
		else {
			*(pImgData + imgPit * iy + ix * imgBitCount) = (byte)(rgb[0] / TEMPLATE_SIZE);
			*(pImgData + imgPit * iy + ix * imgBitCount + 1) = (byte)(rgb[1] / TEMPLATE_SIZE);
			*(pImgData + imgPit * iy + ix * imgBitCount + 2) = (byte)(rgb[2] / TEMPLATE_SIZE);
		}
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_FILTER, 1, NULL);
	return 0;
}
