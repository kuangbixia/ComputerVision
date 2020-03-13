#pragma once
#define NOISE 0.2
struct ThreadParam
{
	CImage* img;
	CImage* src;
	int startIndex;
	int endIndex;
	int maxSpan; // Ϊģ�����ĵ���Ե�ľ���
	float xscale;
	float yscale;
};

static double BicubicWeight(double x);
static bool GetValue(int p[], int size, int& value);


class ImageProcess
{
public:
	static UINT cubicScale(LPVOID p);
	static UINT medianFilter(LPVOID p);
	static UINT saltNoise(LPVOID p);
};