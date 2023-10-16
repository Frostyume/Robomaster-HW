#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <vector>

using namespace std;
using namespace Eigen;

int main()
{
    // 卡尔曼滤波参数
    Matrix<double, 2, 1> X;
    Matrix<double, 2, 2> A;
    Matrix<double, 2, 2> P;
    Matrix<double, 2, 2> Q;
    Matrix<double, 2, 1> K;
    Matrix<double, 1, 2> H;
    Matrix<double, 1, 1> R;

    // 读取dollar.txt文件，加载最近30个交易日的美元的收盘汇率
    vector<double> datas(30);
    freopen("../dollar.txt", "r", stdin);
    for(int i = 0; i < 30; ++i){
        double data;
        cin >> data;
        datas[i] = data;
    }
    
    
    // 初始化卡尔曼滤波器
    X << datas[0], 0;
    A << 1, 1, 0, 1;
    H << 1, 0;
    Q << 2000, 0, 0, 100;
    R << 100;

    // 预测未来10天的美元收盘汇率
    cout << "预测未来10天的美元收盘汇率：" << endl;

    for (int i = 1; i < 30; ++i){
        Matrix<double, 2, 1> X_k = A * X;
        P = A * P * A.transpose() + Q;

        K = P * H.transpose() * (H * P * H.transpose() + R).inverse();

        // 更新状态估计
        Matrix<double, 1, 1> Z{datas[i]};
        X = X_k + K * (Z - H * X_k);
        P = (Matrix<double, 2, 2>::Identity() - K * H) * P;
    }

    // 输出结果
    for (int i = 1; i <= 10; ++i){
        X = A * X;
        cout << "第" << i << "天：" << X[0] << endl;
    }
    return 0;
}