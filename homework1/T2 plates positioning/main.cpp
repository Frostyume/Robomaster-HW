#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


void plate_postioning(Mat image){
    resize(image, image, Size(620, 480));

    // 转换图像为HSV颜色空间
    Mat hsvImage;
    cvtColor(image, hsvImage, COLOR_BGR2HSV);

    // 定义蓝色的颜色范围（在HSV空间中）
    Scalar lowerBlue(90, 100, 100);
    Scalar upperBlue(120, 255, 255); 

    // 创建一个mask，将在图像中找到在蓝色范围内的区域
    Mat mask;
    inRange(hsvImage, lowerBlue, upperBlue, mask);

    // 进行形态学操作以去除噪音
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);

    // 寻找轮廓
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 筛选车牌轮廓
    vector<Point> approx;
    for (const auto& contour : contours) {
        if (contourArea(contour) > 1000) { 
            double epsilon = 0.05 * arcLength(contour, true);
            approxPolyDP(contour, approx, epsilon, true);
            break;
        }
    }

    // 绘制车牌框
    polylines(image, approx, true, Scalar(0, 0, 255), 3);
    
    // 显示结果
    imshow("Plate Positioning", image);
    waitKey(0);
}
int main() {
    // 读取车辆图片
    Mat image1 = imread("../001.jpg");
    Mat image2 = imread("../002.jpg");
    Mat image3 = imread("../003.jpg");
    Mat image4 = imread("../004.jpg");
    Mat image5 = imread("../005.jpg");
    //定位车牌
    plate_postioning(image1);
    plate_postioning(image2);
    plate_postioning(image3);
    plate_postioning(image4);
    plate_postioning(image5);
    return 0;
}