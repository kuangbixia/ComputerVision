#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <iostream>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

int main(int argc, char** argv) {
    Mat src = imread("girl.jpg", IMREAD_GRAYSCALE);
    if (src.empty()) {
        printf("could not load image...\n");
        return -1;
    }
    namedWindow("input image", CV_WINDOW_AUTOSIZE);
    imshow("input image", src);

    // SIFT特征点检测
    int minHessian = 100;
    Ptr<SIFT> detector = SIFT::create(minHessian);//和surf的区别：只是SURF→SIFT
    vector<KeyPoint> keypoints;
    detector->detect(src, keypoints, Mat());//找出关键点

    for (int i = 0; i < keypoints.size(); i++) {
        cout << keypoints[i].pt.x << " " << keypoints[i].pt.y << endl;
    }

    // 绘制关键点
    Mat keypoint_img;
    drawKeypoints(src, keypoints, keypoint_img, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    imshow("KeyPoints Image", keypoint_img);

    waitKey(0);
    return 0;
}