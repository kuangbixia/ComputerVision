#pragma once
#define NOISE 0.2
struct ThreadParam
{
	CImage* img;
	CImage* src;
	int startIndex;
	int endIndex;

	float xscale;
	float yscale;
	float alpha;

	int maxSpan; // 为模板中心到边缘的距离
};

static double cubicWeight(double x);
static double cubicHermite(double A, double B, double C, double D, double t);
static bool GetValue(int p[], int size, int& value);


class ImageProcess
{
public:
	static UINT cubicScale(LPVOID p);
	static UINT cubicRotate(LPVOID p);
	static UINT saltNoise(LPVOID P);
	static UINT medianFilter(LPVOID P);
};