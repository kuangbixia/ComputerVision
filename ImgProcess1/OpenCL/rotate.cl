
double cubicHermite(double A, double B, double C, double D, double t)
{
	double a = -A / 2.0 + (3.0 * B) / 2.0 - (3.0 * C) / 2.0 + D / 2.0;
	double b = A - (5.0 * B) / 2.0 + 2.0 * C - D / 2.0;
	double c = -A / 2.0 + C / 2.0;
	double d = B;
	return a * t * t * t + b * t * t + c * t + d;
}
#pragma OPENCL EXTENSION cl_amd_printf : enable
#define SOFF(x, y, ch, bc) ((y) * sp + (x) * bc + ch)
#define DOFF(x, y, ch, bc) ((y) * dp + (x) * bc + ch)
__kernel void Rotate(__global uchar *in, __global uchar *out, int sw, int sh, int dw, int dh, int sp, int dp, double sina, double cosa, int bitcount)
{
	__global uchar *src = in - sp * (sh - 1);
	__global uchar *dst = out - dp * (dh - 1);
	int x = get_global_id(0);
	int y = get_global_id(1);
	if(x >= dw || y >= dh)
		return;
	double dcenterx = dw / 2.0, dcentery = dh / 2.0;
	double scenterx = sw / 2.0, scentery = sh / 2.0;
	int xx = (int)(x - dcenterx), yy = (int)(y - dcentery);
	double sx = xx * cosa - yy * sina + scenterx;
	double sy = xx * sina + yy * cosa + scentery;
	int fx = (int)sx, fy = (int)sy;
	
	if(fx - 1 <= 0 || fx + 2 >= sw - 1 || fy - 1 <= 0 || fy + 2 >= sh - 1)
	{
		if (fx < 0 || fx >= sw || fy < 0 || fy >= sh)
			return;
		int soffset = fy * sp + fx * bitcount; 
		int doffset = y * dp + x * bitcount; // 3: bytepp
		dst[doffset + 0] = src[soffset + 0];
		dst[doffset + 1] = src[soffset + 1];
		dst[doffset + 2] = src[soffset + 2];
		return;
	}
	
	for(int ch = 0; ch < 3; ++ch)
	{
		
		double col0=cubicHermite(src[SOFF(fx - 1, fy - 1, ch, bitcount)], src[SOFF(fx + 0, fy - 1, ch, bitcount)], src[SOFF(fx + 1, fy - 1, ch, bitcount)], src[SOFF(fx + 2, fy - 1, ch, bitcount)], sx-fx);
		double col1=cubicHermite(src[SOFF(fx - 1, fy + 0, ch, bitcount)], src[SOFF(fx + 0, fy + 0, ch, bitcount)], src[SOFF(fx + 1, fy + 0, ch, bitcount)], src[SOFF(fx + 2, fy + 0, ch, bitcount)], sx-fx);
		double col2=cubicHermite(src[SOFF(fx - 1, fy + 1, ch, bitcount)], src[SOFF(fx + 0, fy + 1, ch, bitcount)], src[SOFF(fx + 1, fy + 1, ch, bitcount)], src[SOFF(fx + 2, fy + 1, ch, bitcount)], sx-fx);
		double col3=cubicHermite(src[SOFF(fx - 1, fy + 2, ch, bitcount)], src[SOFF(fx + 0, fy + 2, ch, bitcount)], src[SOFF(fx + 1, fy + 2, ch, bitcount)], src[SOFF(fx + 2, fy + 2, ch, bitcount)], sx-fx);
		
		double val = cubicHermite(col0, col1, col2, col3, sy - fy);

		if(val > 255.0)
			val = 255.0;
		if(val < 0.0)
			val = 0.0;
		dst[DOFF(x, y, ch, bitcount)] = (uchar)val;
	}
}