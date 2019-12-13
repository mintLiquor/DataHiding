#include<iostream>
#include<ctime>
#include<cmath>
#include<random>

#define error -1

using namespace std;

/*--------------------------------------------------------------------------*/
/*ʵ��������ĺ���*/
long double signal(long double d)
{
	if (d > 0)
		return 1.0;
	else if (d == 0)
		return 0.0;
	else if (d < 0)
		return -1.0;
	else
		return error;
}


/*÷ɭ��ת�㷨����0-1֮��������(double)*/
long double MTRand()
{
	/*-------------�����ⲿ����--------------*/
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


/*÷ɭ��ת�㷨���������(int)*/
int MTRandInt()
{
	/*-------------�����ⲿ����--------------*/
	random_device rd;
	mt19937_64 gen(rd());
	uniform_int_distribution<int> dis(0, 1000);

	int res = dis(gen);
	return res;
}



/*--------------------------------------------------------------------------*/
/*c=1.0ʱ��Ҫ�õ��ĺ���*/
/*c=1.0ʱʵ���Ͼ���laplace�ֲ�*/
long double ILaplace(long double exp, long double beta, long double rand01)
{
	return exp - beta * signal(rand01 - 0.5) * log(1 - 2 * fabs(rand01 - 0.5));
}

/*����ָ���ֲ�����ѧ����*/
long double ExponentialCalculate(long double* num, int amount)
{
	int n;
	long double sum = 0;
	for (n = 0; n < amount; n++)
	{
		sum += num[n];
	}
	return sum / amount;
}


long double ExponentialAbs(long double* num, int amount)
{
	int n;
	long double sum = 0;
	for (n = 0; n < amount; n++)
		sum = fabs(sum) + num[n];
	return sum / amount;
}


long double VarCalculate(long double* num, int amount)
{
	int n;
	long double sum = 0;
	for (n = 0; n < amount; n++)
	{
		sum = sum + num[n] * num[n];
	}
	return sum / amount - ExponentialCalculate(num, amount) * ExponentialCalculate(num, amount);
}


/*--------------------------------------------------------------------------*/
/*c=0.5ʱ��Ҫ�õ��ĺ���*/
/*���ɷ���ָ���ֲ��������*/
long double ExponentialRand(double b)
{
	double rand01 = MTRand();
	return (-1.0 / b) * log(1 - rand01);
	//return - b * log(1 - rand01);

}


/*���ɷ��ϲ�Ŭ���ֲ��������*/
bool BernoulliRand()
{
	/*-------------�����ⲿ����--------------*/
	long double prob = 0.5;
	default_random_engine generator;
	bernoulli_distribution b(prob);
	bool Z = 0;
	for (int i = 0; i < 1000; i++)
		Z = b(generator);
	return Z;
}


/*���ɷ���gamma�ֲ��������*/
long double  GammaRand(long double alpha, long double lambda)
{
	long double x, y;
	long double u, v;
	long double b, c;
	long double w, z;
	bool ac = false;

	if (alpha > 1.0)
	{
		b = alpha - 1;
		c = 3 * alpha - 0.75;
		do
		{
			u = MTRand();
			v = MTRand();

			w = u * (1 - u);
			y = sqrt(c / w) * (u - 0.5);
			x = b + y;
			if (x == 0)
			{
				z = 64 * w * w * w * v * v;
				long double zz = 1 - 2 * y * y / x;
				if (z - zz < 1e-10) {
					ac = true;
				}
				else {
					ac = false;
				}
				//ac=(z=1-2*y*y/x);
				//ac=(z==(1-2*y*y/x));
				if (!ac) {
					double logz = log(z);
					double zzz = 2 * (b * log(x / b) - y);
					if (logz - zzz < 1e-10) {
						ac = true;
					}
					else {
						ac = false;
					}
					//ac=(log(z)==2 * (b*log(x/b)-y));
				}
			}
		} while (!ac);
		return lambda * x;
	}
	else if (alpha == 1.0)
	{
		return lambda * ExponentialRand(MTRand());
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


/*���ɹ����˹�ֲ�Z*/
long double GGDRand(long double c)
{
	bool w = BernoulliRand();
	double E = GammaRand(MTRand(), MTRand());
	double Z;
	if (w == 1)    Z = pow(E, c);
	else    Z = -1 * pow(E, c);
	return Z;
}


int main()
{

	int n, amount;
	long double* num;
	long double rand01, exp, var, beta;
	long double CacExp, CacVar;
	long double total = 0.0;


	//for (int i = 0; i < 100; i++)
	//	cout << MTRand() << " ";


	/*����Ϊ���ɷ���ָ���ֲ��������*/
	
	cout << "********************Exponential Distribution Test********************" << endl;
	cout << "please input scale parameter beta:" << endl;
	cout << "scale parameter beta=";
	cin >> beta;
	if (beta <= 0)
		return error;
	cout << "please input the amount of numbers matching exponential distribution:" << endl;
	cin >> amount;
	num = (long double*)malloc(sizeof(long double) * amount);
	if (!num)
		return error;
	for (n = 0; n < amount; n++)
	{
		num[n] = ExponentialRand(1.0/beta);
		cout << num[n] << endl;
	}
	cout << endl;
	CacExp = ExponentialCalculate(num, amount);
	CacVar = VarCalculate(num, amount);
	cout << "scale parameter(obtained from samples)=" << ExponentialCalculate(num, amount) << endl << endl;
	

	/*����Ϊ���ɷ���GGD�ֲ��������-->��=1.0*/
	cout << "********************GGD (c=1.0) Test********************" << endl;
	cout << "please input the standard deviation(c=1.0):" << endl;
	exp = 0;  //GGD�ֲ�c=1ʱλ�ò���Ĭ��Ϊ0
	cin >> var;
	beta = var / sqrt(2.0);
	cout << "please input the amount of numbers matching GGD:" << endl;
	cin >> amount;
	for (n = 0; n < amount; n++)
	{
		rand01 = MTRand();
		cout << ILaplace(exp, beta, rand01) << endl;
	}
	cout << endl << endl;
	


	/*����Ϊ���ɷ���GGD�ֲ������������>c=0.5*/	
	cout << "********************GGD (c=0.5) Test********************" << endl;
	long double alpha;
	cout << "please input the scale parameter:" << endl;
	cin >> alpha;
	cout << "please input the amount of numbers matching GGD(c=0.5):" << endl;
	cin >> amount;
	
	for (int j = 0; j < amount; j++)
		cout << GGDRand(alpha) << endl;
	cout << endl;

	return 0;
}