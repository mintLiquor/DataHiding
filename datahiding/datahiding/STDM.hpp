#pragma once

#include<iostream>
#include<cmath>
#include<math.h>
#include<random>
#include<ctime>
#include<vector>

using namespace std;

#define PI 3.1415926
#define NUM 4


//定义w数组的值


class STDM {
private:
    int w[2] = { 1,-1 };

    std::random_device rd;  //获取随机数种子
    std::mt19937 generator; //Standard mersenne_twister_engine seeded with rd()
    double x0;
    
    //double de;//解码后的结果
    double attack_std;//高斯噪声的STD

public:
    STDM();
    //生成符合高斯分布的随机数
    void GaussGen(double x[], double std);
    void inset(double x[], double delta,double s[]);
    //stdm解码
    double decode(double y[], double delta);
    //高斯噪声攻击
    void addGaussAttack(double std, double s[], double v[],double y[]);
    //解码后统计错误率
    //
    double Serfc(double x);
    //计算理论性能
    double CalculateTheo(double v[], double delta,double std);
    //Q函数
    double Q(double x);
};

