#ifndef pyr_yape06_h
#define pyr_yape06_h

#include "keypoint.h"
#include "fine_gaussian_pyramid.h"

class pyr_yape06
{
public:
    pyr_yape06(void);
    ~pyr_yape06(void);

    int detect(fine_gaussian_pyramid* pyramid, keypoint* keypoints, int max_number_of_keypoints);

    IplImage* draw_keypoints(fine_gaussian_pyramid* pyramid, keypoint* keypoints, int number_of_keypoints);

    static void find_second_derivatives_sigma(void);
    static void find_dog_sigma(void);

    void set_laplacian_threshold(int T) { lap_threshold = T; }
    void set_min_eigenvalue_threshold(int T) { min_ev_threshold = T; }

    //private:
    void compute_Ds(IplImage* smoothed_image);
    void compute_laplacian(IplImage* smoothed_image, IplImage* laplacian,
        const int w, const int h,
        const int Dxx, const int Dyy,
        const int Dxy, const int Dyx);
    void compute_laplacian(IplImage* smoothed_image);
    void add_local_extrema(fine_gaussian_pyramid* pyramid, IplImage* smoothed_image, int scale);
    int hessian_min_eigen_value(IplImage* smoothed_image, const int tr, const int x, const int y);
    bool laplacian_hessian_criteria(IplImage* laplacian, const int x, const int y);
    void sort_keypoints(void);
    int copy_keypoints(keypoint* keypoints, int max_number_of_keypoints);

    static const int Maximum_number_of_points;
    keypoint* all_keypoints;
    int number_of_points;

    int lap_threshold, min_ev_threshold;

    IplImage* laplacian;
    static const int R, Rp;
    int Dxx, Dyy, Dxy, Dyx, DXY, DYX;

    // For debugging:
    void save_eigen_value1(IplImage* smoothed_image, IplImage* laplacian);
    void save_eigen_value2(IplImage* smoothed_image, IplImage* laplacian);
};

#endif
