#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// 比较横坐标
bool compare_x(vector<Point2f> &a, vector<Point2f> &b){
    return a[0].x < b[0].x;
}

// 储存矩形顶点坐标
void rect_points(Rect rect, vector<Point2f> &tmp){
    tmp[0].x = rect.x;
    tmp[0].y = rect.y;
    tmp[1].x = rect.x + rect.width;
    tmp[1].y = rect.y;
    tmp[2].x = rect.x + rect.width;
    tmp[2].y = rect.y + rect.height;
    tmp[3].x = rect.x;
    tmp[3].y = rect.y + rect.height;
}

int main() {
    VideoCapture cap("../armor.mp4"); 

    Mat frame;
    while (cap.read(frame)) {
        // 转换图像为HSV颜色空间
        Mat hsvframe;
        cvtColor(frame, hsvframe, COLOR_BGR2HSV);

        // 定义灯的颜色范围（在HSV空间中）
        Scalar lowerLight(18, 80, 120);
        Scalar upperLight(34, 255, 255);

        // 创建一个mask，在图像中找到灯
        Mat mask;
        inRange(hsvframe, lowerLight, upperLight, mask);
        // 高斯滤波
        GaussianBlur(mask, mask, Size(7, 7), 0, 0);
        // 进行形态学操作以去除噪音
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        morphologyEx(mask, mask, MORPH_CLOSE, kernel);

        // 寻找并筛选图像中的轮廓
        vector<vector<Point>> contours;
        vector<vector<Point2f>> lights(4, vector<Point2f>(4));
        int idx = 0;

        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for (int i = 0; i < contours.size(); i++) {
            Rect rect = boundingRect(contours[i]);
            if((double(rect.height / rect.width) > 1.2) && (contourArea(contours[i]) > 30)){
                Mat approx;
                // 近似多边形
                approxPolyDP(contours[i], approx, 0.018 * arcLength(contours[i], true), true);
                // 给装甲板的灯描边
                polylines(frame, approx, true, Scalar(0, 255, 0), 2);
                // 将每个灯的外界矩形顶点坐标用新的数组储存
                rect_points(rect, lights[idx]);
                idx++;
            }
            if(idx == 4){
                idx = 0;
                break;
            }
        }
        // drawContours(frame, contours, -1, Scalar(0, 0, 255), 3);

        // 将灯按从左到右排序
        sort(lights.begin(), lights.end(), compare_x);
        for (int i = 0; i < 3; i+=2){
            // 筛选出同一组灯
            if((norm(lights[i][0] - lights[i+1][0]) < 80) && ((lights[i][0].y / lights[i+1][0].y) < 1.1 || lights[i][0].y / lights[i+1][0].y > 0.91)){
                // 框出装甲板
                line(frame, lights[i][0], lights[i+1][1], Scalar(0, 0, 255), 3);
                line(frame, lights[i][3], lights[i+1][2], Scalar(0, 0, 255), 3);
                line(frame, lights[i][0], lights[i][3], Scalar(0, 0, 255), 3);
                line(frame, lights[i+1][1], lights[i+1][2], Scalar(0, 0, 255), 3);
            }
        }

        // 显示结果
        imshow("Armor", frame);
        // 按Q退出
        int key = waitKey(10);
        if (key == 'q') break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}
