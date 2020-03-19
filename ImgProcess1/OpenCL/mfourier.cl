#pragma OPENCL EXTENSION cl_amd_printf : enable
#define PI (3.14159265358)
#define FOURIER_FACTOR (14.0)

__kernel void myFourierCL(__global int *In, __global int *Out, int width, int height)
{
	float real=0.0, imag = 0.0;

	int u = get_global_id(0);
	int v = get_global_id(1);

	for(int y=0; y<height; ++y)
	{
		for(int x=0;x<width; ++x)
		{
			float gray = In[y*width + x];
			float A = 2* PI*((float)u * (float)x / (float)width + (float)v * (float)y / (float)height);
			real += gray*cos(A);
			imag -= gray*sin(A);
		}
	}
	double mag = sqrt(real * real + imag * imag);
	mag = FOURIER_FACTOR * log(mag + 1);
	if(mag > 255.0)
		mag = 255.0;
	if (mag < 0.0)
		mag = 0.0;
	Out[v*width+u] = mag;
}