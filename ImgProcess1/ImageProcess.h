#pragma once

#define NOISE 0.2
#define FOURIER_FACTOR 14

struct ThreadParam
{
	CImage* img;
	CImage* src;
	int startIndex;
	int endIndex;

	float xscale;
	float yscale;
	float alpha;

	float mean;
	float stddev; // 标准差

	int maxSpan; // 为模板中心到边缘的距离

	float sigma_d;
	float sigma_r;
};

static double cubicWeight(double x);
static double cubicHermite(double A, double B, double C, double D, double t);
static bool GetValue(int p[], int size, int& value);
static double BoxMullerGenerator(double mean, double stddev);
static void GetGaussianTemplate(double t[3][3], double stddev);

class ImageProcess
{
public:
	static UINT cubicScale(LPVOID p);
	static UINT cubicRotate(LPVOID p);
	static UINT saltNoise(LPVOID P);
	static UINT gaussianNoise(LPVOID p);
	static UINT medianFilter(LPVOID P);
	static UINT meanFilter(LPVOID p);
	static UINT gaussianFilter(LPVOID p);
	static UINT wienerFilter(LPVOID p);
	static UINT bilateralFilter(LPVOID p);
	static UINT fourierTransform(LPVOID p);
};