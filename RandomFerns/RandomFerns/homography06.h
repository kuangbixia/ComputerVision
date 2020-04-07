#ifndef homography06_h
#define homography06_h

#include <iostream>
using namespace std;

#include <cv.h>

class homography06 : public CvMat
{
public:
    homography06(void);
    homography06(const float h11, const float h12, const float h13,
        const float h21, const float h22, const float h23,
        const float h31, const float h32, const float h33);
    homography06(const double h11, const double h12, const double h13,
        const double h21, const double h22, const double h23,
        const double h31, const double h32, const double h33);

    ~homography06();

    void transform_point(const int u, const int v, int& up, int& vp);
    void transform_point(const float u, const float v, float& up, float& vp);
    void transform_point(const double u, const double v, double& up, double& vp);

    friend ostream& operator<< (ostream& o, const homography06& H);

    float cvmGet(const int i, const int j);
    void  cvmSet(const int i, const int j, const float val);
    void  cvmSet(const int i, const int j, const double val);

    // private:
    void initialize(void);
};

ostream& operator<< (ostream& o, const homography06& H);

inline float homography06::cvmGet(const int i, const int j)
{
    return data.fl[3 * i + j];
}

inline void  homography06::cvmSet(const int i, const int j, const float val)
{
    data.fl[3 * i + j] = val;
}

inline void  homography06::cvmSet(const int i, const int j, const double val)
{
    data.fl[3 * i + j] = float(val);
}

#endif

