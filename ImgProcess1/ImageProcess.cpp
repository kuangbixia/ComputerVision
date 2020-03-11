#include "pch.h"
#include "framework.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>
using namespace std;

static double BicubicWeight(double x)
{
	// һ����Ϊ-0.5��-0.75
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
		// GetBPP()ÿ�����ص�λ��
		// ÿ�����ص��ֽ���
		int srcBitCount = src->GetBPP() / 8;
		// GetPitch()ͼ��ļ��
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
		// GetBPP()ÿ�����ص�λ��
		// ÿ�����ص��ֽ���
		int imgBitCount = param->img->GetBPP() / 8;
		// GetPitch()ͼ��ļ��
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

// ��������
UINT ImageProcess::saltNoise(LPVOID  p)
{
	ThreadParam* param = (ThreadParam*)p;
	// ����Ϊ��λ 
	int maxWidth = param->img->GetWidth();
	int maxHeight = param->img->GetHeight();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex;
	// GetBits()ָ��λͼʵ������ֵ��ָ��
	byte* pRealData = (byte*)param->img->GetBits();
	// GetBPP()ÿ�����ص�λ��
	// ÿ�����ص��ֽ���
	int bitCount = param->img->GetBPP() / 8;
	// GetPitch()ͼ��ļ��
	int pit = param->img->GetPitch();

	// Ҫ������������ӣ������ʱ���ڲ����������һ��
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

	// ������Ϣ
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_NOISE, 1, NULL);
	return 0;
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
