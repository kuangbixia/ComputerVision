#pragma once
#define NOISE 0.2
struct ThreadParam
{
	CImage* img;
	int startIndex;
	int endIndex;
	int maxSpan;//Ϊģ�����ĵ���Ե�ľ���
};

static double BicubicWeight(double x);
static bool GetValue(int p[], int size, int& value);


class ImageProcess
{
public:
	static UINT cubicScale(LPVOID p, float xscale, float yscale, CImage* temp);
	static UINT medianFilter(LPVOID p);
	static UINT saltNoise(LPVOID p);
};