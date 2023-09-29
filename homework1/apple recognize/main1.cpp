#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace cv;

Mat apple_result;
vector<vector<Point>> contours;
vector<Vec4i> hierarchy;

void to_result(Mat apple_src) {
    int area_max = 0, id_max = 0;
    findContours(apple_src, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
    for (int i = 0; i < contours.size(); ++i) {
        if (contourArea(contours[i]) > area_max) {
            area_max = contourArea(contours[i]);
            id_max = i;
        }
    }

    drawContours(apple_result, contours, id_max, Scalar(255, 255, 255), -1);
}

int main() {
    Mat apple, apple_grey, apple_red, apple_yellow, apple_red_result, apple_yellow_result, apple_grey_result;
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));

    namedWindow("apple");
    apple = imread("../apple.png");
    cvtColor(apple, apple_grey, COLOR_BGR2GRAY);
    threshold(apple_grey, apple_result, 0, 0, THRESH_BINARY);
    Mat channels[3];
    split(apple, channels);
    apple_red = channels[2] - channels[1];

    threshold(apple_red, apple_red_result, 76, 255, THRESH_BINARY);
    morphologyEx(apple_red_result, apple_red_result, MORPH_OPEN, element);
    to_result(apple_red_result);

    inRange(apple_grey, 60, 120, apple_grey_result);
    inRange(apple_red, 20, 76, apple_yellow);
    apple_yellow_result = apple_yellow & apple_grey_result;
    morphologyEx(apple_yellow_result, apple_yellow_result, MORPH_OPEN, element);
    to_result(apple_yellow_result);

    medianBlur(apple_result.clone(), apple_result, 3);
    morphologyEx(apple_result, apple_result, MORPH_CLOSE, element);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(apple_result,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point());
    drawContours(apple, contours, 0, Scalar(255,255,255), 1);
    Rect rect = boundingRect(contours[0]);
    rectangle(apple, rect, Scalar{255, 255, 255}, 2);

    imshow("apple", apple);
    waitKey(0);
    return 0;
}