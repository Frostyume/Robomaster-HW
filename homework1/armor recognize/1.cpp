#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


bool compare_x(Point2f &a, Point2f &b){
    return a.x < b.x;
}


int main() {
    VideoCapture cap("../armor.mp4"); // 替换为你的视频文件路径

    Mat frame;
    while (cap.read(frame)) {
        // 转换图像为HSV颜色空间
        Mat hsvframe;
        cvtColor(frame, hsvframe, COLOR_BGR2HSV);

        // 定义亮灯的颜色范围（在HSV空间中）
        Scalar lowerLight(18, 80, 120);
        Scalar upperLight(34, 255, 255);

        // 创建一个mask，在图像中找到亮灯
        Mat mask;
        inRange(hsvframe, lowerLight, upperLight, mask);

        GaussianBlur(mask, mask, Size(7, 7), 0, 0);
        // 进行形态学操作以去除噪音
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        morphologyEx(mask, mask, MORPH_CLOSE, kernel);

        // 寻找并筛选图像中的轮廓
        vector<vector<Point>> contours;
        Point2f lights[4][4];
        vector<Point2f> centers(4);
        int idx = 0;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for (int i = 0; i < contours.size(); i++) {
            RotatedRect rect = minAreaRect(contours[i]);
            Rect boundingBox = boundingRect(contours[i]);
            if((double(boundingBox.height / boundingBox.width) > 1.5) && (contourArea(contours[i]) > 30)){
                Mat approx;
                approxPolyDP(contours[i], approx, 0.018 * arcLength(contours[i], true), true);
                polylines(frame, approx, true, Scalar(0, 255, 0), 3);
                rect.points(lights[idx]);
                centers[idx] = rect.center;
                idx++;
            }
            if(idx == 4){
                idx = 0;
                break;
            }
        }
        // drawContours(frame, contours, -1, Scalar(0, 0, 255), 3);
        sort(lights[0], lights[4], compare_x);
        for (int i = 0; i < 3; i+=2){
                if((norm(lights[i][0] - lights[i+1][0]) < 100) && ((lights[i][0].y / lights[i+1][0].y) < 1.2 || lights[i][0].y / lights[i+1][0].y > 0.8)){
                    line(frame, lights[i][1], lights[i+1][3], Scalar(0, 0, 255), 2);
                    line(frame, lights[i][0], lights[i+1][2], Scalar(0, 0, 255), 2);
                    line(frame, lights[i][0], lights[i][1], Scalar(0, 0, 255), 2);
                    line(frame, lights[i+1][2], lights[i+1][3], Scalar(0, 0, 255), 2);
                }
            
        }

        // 显示结果
        imshow("Apple", frame);

        int key = waitKey(20);
        if (key == 'q') break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}
