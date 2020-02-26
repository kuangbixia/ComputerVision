#include<opencv2\core.hpp>
#include<opencv2\highgui.hpp>

using namespace cv;

int main()
{
    Mat img = imread("./test.png");
    namedWindow("test");
    imshow("test", img);

    // µÈ´ıÊ±¼ä 5000ms
    waitKey(5000);

    return 0;
    system("pause");
}
