#include <Eigen/Core>
#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>
#include "big_armor_scale.hpp"
                                           
using namespace cv;
using namespace Eigen;
using namespace std;

int main() {
    // 装甲板四点像素坐标
    vector< Point2f > armor_corners = {
                                        {575.508,282.175},
                                        {573.93,331.819},
                                        {764.518,337.652},
                                        {765.729,286.741}
                                    };

    // 读取相机内参矩阵和畸变矩阵
    Mat f_mat, c_mat;
    FileStorage reader("../f_mat_and_c_mat.yml", FileStorage::READ);
    reader["F"] >> f_mat;
    reader["C"] >> c_mat;

    Mat r_vec, t_vec;
    solvePnP(PW_BIG, armor_corners, f_mat, c_mat, r_vec, t_vec);

    Quaterniond q(-0.0816168, 0.994363, -0.0676645, -0.00122528);
    Matrix3d q_matrix = q.matrix();
    Vector3d t;
    cv2eigen(t_vec, t);
    t = q_matrix * t;
    cout << t << endl;
}