#ifndef template_matching_based_tracker_h
#define template_matching_based_tracker_h

#include <cv.h>
#include "homography06.h"
#include "homography_estimator.h"

class template_matching_based_tracker
{
public:
    template_matching_based_tracker(void);

    bool load(const char* filename);
    void save(const char* filename);

    void learn(IplImage* image,
        int number_of_levels, int max_motion, int nx, int ny,
        int xUL, int yUL,
        int xBR, int yBR,
        int bx, int by,
        int Ns);

    void initialize(void);
    void initialize(int u0, int v0,
        int u1, int v1,
        int u2, int v2,
        int u3, int v3);

    bool track(IplImage* input_frame);

    homography06 f;

    //private:
    void find_2d_points(IplImage* image, int bx, int by);
    void compute_As_matrices(IplImage* image, int max_motion, int Ns);
    void move(int x, int y, float& x2, float& y2, int amp);
    bool normalize(CvMat* V);
    void add_noise(CvMat* V);
    IplImage* compute_gradient(IplImage* image);
    void get_local_maximum(IplImage* G,
        int xc, int yc, int w, int h,
        int& xm, int& ym);

    homography_estimator he;

    int* m;
    CvMat** As;
    CvMat* U0, * U, * I0, * DU, * DI, * I1;
    float* u0, * u, * i0, * du, * i1;
    int number_of_levels, nx, ny;
};


#endif
