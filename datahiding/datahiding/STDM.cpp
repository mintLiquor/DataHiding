#define _CRT_SECURE_NO_WARNINGS
#include"STDM.hpp"

STDM::STDM(){
    generator.seed(2000);
}

//参数为STD时是生成宿主函数
//参数为attack_std时是生成高斯噪声
void STDM::GaussGen(double x[], double std) {
    normal_distribution<double> distribution(0.0, std);
    for (int i = 0; i < NUM; i++) {
        x[i] = distribution(generator);
    }
    //normal_distribution::stddev=4;
}

void STDM::inset(double x[], double delta, double s[]) {//X为宿主信号,嵌入后得到s
    double q;
    double temp = 0;
    //计算x0的值
    for (int i = 0; i < NUM; i++) {
        temp += x[i] * w[i % 2];
    }
    x0 = temp / NUM;
    int b = 1;
    //计算q的值
    //嵌入1
    if (b)
        q = floor((x0 - (delta / 4)) / delta + 0.5) * delta + delta / 4;
    //嵌入0
    else
        q = floor((x0 + (delta / 4)) / delta + 0.5) * delta - delta / 4;

    //计算s的值
    for (int i = 0; i < NUM; i++) {
        s[i] = x[i] + w[i % 2] * (q - x0);
    }
}

double STDM::decode(double y[],double delta) {
    //在执行addGuassAttack之后的话，y就是s
    //double temp = 0;
    double _y;
    double q,de;
    double summary = 0.0;
    for (int i = 0; i < NUM; i++) {
        summary += w[i % 2] * y[i];
    }
    _y = summary / NUM;
    q = floor(((_y + delta / 4) / delta + 0.5)) * delta - delta / 4;

    if (fabs(q - _y) < delta / 4)
        de = 0;
    else
        de = 1;
    return de;
}

void STDM::addGaussAttack(double std,double s[],double v[],double y[]) {
    //在数组v里生成NUM个高斯随机数
    GaussGen(v, std);
    //将噪声添加到嵌入后的宿主信号中
    for (int i = 0; i < NUM; i++) {
        //x[i] += v[i];//嵌入前加噪声
        y[i] = s[i] + v[i];//嵌入后加噪声
    }
    //delete[]v;
}



//计算理论错误
double STDM::CalculateTheo(double v[],double delta,double std) {
    double sigma=std;
    
    double res=0.0;
    
    for (int m=-10;m<=10;m++){
        res+=Q((sqrt(NUM) * (m*delta+delta / 4)) / sigma)-Q((sqrt(NUM) *(m*delta+delta * 0.75)) / sigma);
    }
    return res;
}

double STDM::Q(double x){
    return erfc(x/sqrt(2.0))/2;
}
