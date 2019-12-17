#include<iostream>
#include<fstream>
#include<vector>
#include"randword.h"

using namespace std;


void Gauss()
{
	vector<double> dis = GaussDisGen(NUM);
	ofstream outfile;
	outfile.open("Gauss.txt");
	for (int i = 0; i < NUM; i++) {
		outfile << dis[i] << " ";
		cout << dis[i] << " ";
	}
	cout << endl;
	outfile << endl;
}

void GaussTest()
{
	vector<double> dis;
	ifstream infile;
	infile.open("Gauss.txt");
}

void exp()
{
	double beta = 2.0;
	vector<double> dis = ExpDisGen(NUM, beta);
	ofstream outfile;
	outfile.open("exp.txt");
	for (unsigned int i = 0; i < dis.size(); i++) {
		outfile << dis[i] << " ";
		cout << dis[i] << " ";
	}
	outfile << endl;
	cout << endl;
}

void expTest()
{
	vector<double> dis;
	ifstream infile;
	infile.open("exp.txt");

}

int main()
{
	vector<int> w(NUM, 1);
	for (int i = 500; i < NUM; i++) {
		w[i] = -1;
	}

	rand(w);
	cout << "生成高斯分布参数：" << endl;
	Gauss();
	cout << "生成指数分布参数：" << endl;
	exp();
	cout << "生成广义高斯分布参数：" << endl;

	return 0;
}