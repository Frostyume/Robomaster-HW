#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
    // 读取图像
    Mat image = imread("../apple.png");

    // 转换图像为HSV颜色空间
    Mat hsvImage;
    cvtColor(image, hsvImage, COLOR_BGR2HSV);

    // 定义红色和橙色的颜色范围（在HSV空间中）
    Scalar lowerOrange(11, 172, 99);
    Scalar upperOrange(25, 255, 255);
    Scalar lowerRed1(156, 100, 100);
    Scalar upperRed1(180, 255, 255);
    Scalar lowerRed2(0, 100, 100);
    Scalar upperRed2(10, 255, 255);

    // 创建三个mask，在图像中找到在红色和橙色范围内的区域
    Mat mask1, mask2, mask3;
    inRange(hsvImage, lowerRed1, upperRed1, mask1);
    inRange(hsvImage, lowerRed2, upperRed2, mask2);
    inRange(hsvImage, lowerOrange, upperOrange, mask3);
    // 对mask1和mask2取并集
    Mat ones_mat = Mat::ones(Size(image.cols, image.rows), CV_8UC1);
    Mat mask = 255 * (ones_mat - (ones_mat - mask1 /255).mul(ones_mat - mask2/ 255).mul(ones_mat - mask3/ 255));
    // 中值滤波
    medianBlur(mask, mask, 3);
    // 进行形态学操作以去除噪音
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(6, 6));
    morphologyEx(mask, mask, MORPH_OPEN, kernel, Point(-1,-1), 1);

    // 寻找图像中的轮廓
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    int max_area = 0, id_max = 0;
    for (int i = 0; i < contours.size(); ++i) {
        if(contourArea(contours[i]) > max_area){
            max_area = contourArea(contours[i]);
            id_max = i;
        }
    }
    
    // 绘制苹果轮廓并框出来
    Rect boundingBox = boundingRect(contours[id_max]);
    rectangle(image, boundingBox, Scalar(0, 255, 0), 2);
    drawContours(image, contours, id_max, Scalar(255, 0, 0), 2);
    // 显示结果
    imshow("Apple", image);
    // imwrite("../result.png", image);
    waitKey(0);

    return 0;
}
