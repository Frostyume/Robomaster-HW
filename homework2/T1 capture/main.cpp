#include <iostream>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

using namespace cv;
using namespace std;
using namespace Eigen;

Mat img;

// 创建一个相机类
class camera{
private:
    Vector3d cam_position;
    Quaterniond cam_posture;
    Matrix<double, 3, 4> cam_in;

public:
    camera(){
        // 传入相机的内参
        this->cam_in << 400., 0., 190., 0.,
                    0., 400., 160., 0.,
                    0., 0., 1., 0.;
    }

    // 在给定坐标设置相机
    void set(double x, double y, double z, Quaterniond p){
        cam_position << x, y, z;
        cam_posture = p;
    }

    // 相机坐标系——>世界坐标系
    Matrix<double, 3, 4> pos_cam2world(){
        Matrix4d converter = Matrix4d::Zero();
        Matrix3d posture_matrix = cam_posture.matrix();
        converter(3, 3) = 1.;
        converter.block(0, 0, 3, 3) = posture_matrix.transpose().cast<double>();
        converter.block(0, 3, 3, 1) = -posture_matrix.transpose().cast<double>() * cam_position;
        return cam_in * converter;
    }
};


int main() {
    freopen("../points.txt", "r", stdin);

    img = Mat::zeros(Size(1920,1080),CV_8UC1);

    camera Cam;
    // 设置相机
    Cam.set(2., 2., 2.,Quaterniond{-0.5,0.5, 0.5, -0.5});
    Matrix<double, 3, 4> cam_matrix = Cam.pos_cam2world();
    //读取点的数量
    int point_sum;
    cin >> point_sum;
    // 模拟拍摄
    for(int i = 0; i < point_sum; i++){
        double x, y, z;
        cin >> x >> y >> z;
        Vector4d point_pos;
        point_pos << x, y, z, 1;
        Vector3d point_img = cam_matrix * point_pos;
        Point2d point_img_pos;
        point_img /= point_img(2);
        point_img_pos.x = point_img(0);
        point_img_pos.y = point_img(1);
        circle(img, point_img_pos, 2, Scalar(255, 255, 255), 3);
    }

    imshow("point", img);
    waitKey(0);

    imwrite("../result.jpg", img);
    return 0;
}