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
    Scalar lowerOrange(0, 160, 100);
    Scalar upperOrange(23, 255, 255);
    Scalar lowerRed(156, 150, 150);
    Scalar upperRed(180, 255, 255);

    // 创建两个mask，将分别在图像中找到在红色和橙色范围内的区域
    Mat mask1, mask2;
    inRange(hsvImage, lowerRed, upperRed, mask1);
    inRange(hsvImage, lowerOrange, upperOrange, mask2);
    // 对mask1和mask2取并集
    Mat ones_mat = Mat::ones(Size(image.cols, image.rows), CV_8UC1);
    Mat mask = 255 * (ones_mat - (ones_mat - mask1 /255).mul(ones_mat - mask2/ 255));

    // 进行形态学操作以去除噪音
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);

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
    drawContours(image, contours, id_max, Scalar(255, 0, 0), 3);
    // 显示结果
    imshow("Apple", image);
    // imshow("Apple", mask);
    waitKey(0);

    return 0;
}