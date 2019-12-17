#pragma once

#include<iostream>
#include<time.h>
#include<cmath>
#include<random>

using namespace std;

#define PI 3.14159265353846
#define a 1.8
#define N 10000
#define NUM 1000


int randint(int i)
{
	std::random_device rd;  //获取随机数种子
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

	uniform_int_distribution<int> dis(i, NUM - 1);

	int res = dis(gen);

	return res;
}

/*梅森旋转算法生成0-1之间的随机数(double)*/
long double MTRand()
{
	/*-------------引用外部代码--------------*/
	random_device rd;
	mt19937_64 mt(rd());
	uniform_real_distribution<long double> dis(0.0, 1.0);

	int count = 0;
	double res = dis(rd);
	while (count < 1000) {
		res = dis(rd);
		++count;
	}
	return res;
}


/*梅森旋转算法生成随机数(int)*/
int MTRandInt()
{
	/*-------------引用外部代码--------------*/
	random_device rd;
	mt19937_64 gen(rd());
	uniform_int_distribution<int> dis(0, 1000);

	int res = dis(gen);
	return res;
}

/*生成符合高斯分布的数据 Box-Muller*/
double GaussRand() {
	double U1, U2;
	U1 = MTRand();
	U2 = MTRand();
	while ((-2) * log(U1) * sin(2 * PI * U2) < 0) {
		U1 = MTRand();
		U2 = MTRand();
	}
	double Z = sqrt((-2) * log(U1) * sin(2 * PI * U2));
	return Z;
}

/*生成指数分布Z*/
double ExpRand(double beita) {
	double pV = MTRand();
	double Z = (-1.0 / beita) * log(1 - pV);
	//cout<<Z<<endl;
	return Z;
}

/*生成伯努利分布Z*/
bool BonuliRand() {
	double prob = 0.5;
	default_random_engine generator;
	bernoulli_distribution b(prob);
	bool Z = 0;
	for (int i = 0; i < MTRandInt(); ++i) {
		Z = b(generator);
	}
	return Z;
}



//生成伽玛分布Z
double GammaRand(double alpha, double lambda) {
	double u, v;
	double x, y;
	double b, c;
	double w, z;
	bool accept = false;

	if (alpha > 1.0) {
		b = alpha - 1;
		c = 3 * alpha - 0.75;
		do {
			u = MTRand();
			v = MTRand();

			w = u * (1 - u);
			y = sqrt(c / w) * (u - 0.5);
			x = b + y;
			if (x >= 0) {
				z = 64 * w * w * w * v * v;
				double zz = 1 - 2 * y * y / x;
				if (z - zz < 1e-10) {
					accept = true;
				}
				else {
					accept = false;
				}
				//accept=(z=1-2*y*y/x);
				//accept=(z==(1-2*y*y/x));
				if (!accept) {
					double logz = log(z);
					double zzz = 2 * (b * log(x / b) - y);
					if (logz - zzz < 1e-10) {
						accept = true;
					}
					else {
						accept = false;
					}
					//accept=(log(z)==2 * (b*log(x/b)-y));
				}
			}
		} while (!accept);
		return lambda * x;
	}
	else if (alpha == 1.0) {
		return lambda * ExpRand(MTRand());
	}
	else if (alpha < 1.0) {
		double dv = 0.0;
		double b = (exp(1.0) + alpha) / exp(1.0);
		do {
			double u1 = MTRand();
			double p = b * MTRand();
			double y;
			if (p > 1) {
				y = -log((b - p) / alpha);
				if (u1 < pow(y, alpha - 1)) {
					dv = y;
					break;
				}
			}
			else {
				y = pow(p, 1 / alpha);
				if (u1 < exp(-y)) {
					dv = y;
					break;
				}
			}
		} while (1);
		return dv * lambda;
	}
	return -1;
}

/*生成广义高斯分布Z*/
double GGDRand(double c) {
	bool w = BonuliRand();
	double E = GammaRand(2.0, 3.0);
	double Z;
	if (w == 1)    Z = pow(E, c);
	else    Z = -1 * pow(E, c);
	return Z;
}


vector<double>  GaussDisGen(int n) {
	vector<double> res(n);
	for (int i = 0; i < n; ++i) {
		res[i] = GaussRand();
	}
	return res;
}

vector<double>  ExpDisGen(int n, double beta) {
	vector<double> res(n);
	for (int i = 0; i < n; ++i) {
		res[i] = ExpRand(beta);
	}
	return res;
}

void rand(vector<int>& w)
{
	for (int i = 0; i < w.size(); i++)
		swap(w[i], w[randint(i)]);
}