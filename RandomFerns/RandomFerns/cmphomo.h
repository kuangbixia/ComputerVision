#ifndef CMPHOMO_H
#define CMPHOMO_H

void homography_transform(const float a[2], const float H[3][3], float r[2]);

/* computes the homography sending [0,0] , [0,1], [1,1] and [1,0]
 * to x,y,z and w.
 */
void homography_from_4pt(const float* x, const float* y, const float* z, const float* w, float cgret[8]);

/*
 * computes the homography sending a,b,c,d to x,y,z,w
 */
void homography_from_4corresp(
    const float* a, const float* b, const float* c, const float* d,
    const float* x, const float* y, const float* z, const float* w, float R[3][3]);

#endif
