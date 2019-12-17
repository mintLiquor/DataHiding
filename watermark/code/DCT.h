#pragma once

#include <iostream>
#include <stdlib.h>
#include "windows.h"
#include <memory>
#include <cmath>
#include<vector>
#include<fstream>
using namespace std;

#define PI  3.14159262
#define N 8
#define a 100//嵌入强度

vector<int> dct;
vector<int> s;



/*
int image[256][256] = { 0 };//读入的图像
float DCTimage[256][256] = { 0 };//DCT变换后的图像数据 
int SCHimage[256][256] = { 0 };//量化后的图像数据 
int _SCHimage[256][256] = { 0 };//反量化后的图像数据 
int IDCTimage[256][256] = { 0 };//逆DCT变换后的图像数据 

float m[8][8] = { 0.0 };
int n[8][8] = { 0 }, a[8][8] = { 0 };//8*8图像块;
int Fy[8][8] = { {16,11,10,16,24,40,51,61},//色差量化表 
			  {12,12,14,19,26,58,60,55},
			  {14,13,16,24,40,57,69,56},
			  {14,17,22,29,51,87,80,62},
			  {18,22,37,56,68,109,103,77},
			  {24,35,55,64,81,104,113,92},
			  {49,64,78,87,103,121,120,101},
			  {72,92,95,98,112,100,103,99} };

double round(double r) { return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5); }
*/


void printMatrix2(vector<vector<double>> input)
{
	for (int i = 0; i < input.size(); i++) {
		for (int j = 0; j < input[0].size(); j++) {
			cout << input[i][j] << "    ";
		}
		cout << endl;
	}
}

/*DCT和IDCT代码参考网络*/
/*https ://blog.csdn.net/u014518566/article/details/46432621*/
double** DCT(double** input)
{
	double** output = { 0 };
	double alpha, beta;
	for (int u = 0; u < N; u++) {
		for (int v = 0; v < N; v++) {
			if (u == 0)
				alpha = sqrt(1.0 / N);
			else
				alpha = sqrt(2.0 / N);
			if (v == 0)
				beta = sqrt(1.0 / N);
			else
				beta = sqrt(2.0 / N);

			double temp = 0.0;
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					temp += input[i][j] * cos((2 * i + 1) * u * PI / (2.0 * N)) * cos((2 * j + 1) * v * PI / (2.0 * N));
				}
			}
			output[u][v] = alpha * beta * temp;
		}
		
	}
	return output;
}

double** IDCT(double** input)
{
	double** output = { 0 };
	double alpha, beta;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			double temp = 0.0;
			for (int u = 0; u < N; u++) {
				for (int v = 0; v < N; v++) {
					if (u == 0)
						alpha = sqrt(1.0 / N);
					else
						alpha = sqrt(2.0 / N);
					if (v == 0)
						beta = sqrt(1.0 / N);
					else
						beta = sqrt(2.0 / N);

					temp+= alpha * beta * input[u][v] * cos((2 * i + 1) * u * PI / (2.0 * N)) * cos((2 * j + 1) * v * PI / (2.0 * N));
				}
			}

			output[i][j] = min(temp, 255);
			output[i][j] = max(temp, 0);
			output[i][j] = floor(output[i][j]);
		}
	}
	return output;
}

/*核心代码*/
void assDecode(vector<int> w)
{
	ifstream infile1;
	infile1.open("pixel.txt");
	ifstream infile2;
	infile2.open("logo.txt");
	int logo[1024];
	for (int k = 0; k < 1024; k++)
		infile2 >> logo[k];

	ofstream outfile;
	outfile.open("DCT.txt");

	double** bmp = { 0 };
	double **middle;//中频系数

	/*感觉这个地方写的有点不好 或许可以用vector实现更好*/
	for (int t = 0; t < 4096; t++) {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				infile1 >> bmp[i][j];
			}
		}
		middle = DCT(bmp);

		double k1 = middle[3][4];
		double k2 = middle[4][3];
		double s1 = k1 + logo[t / 4] * (int)a * w[t * 2];
		double s2 = k2 + logo[t / 4] * (int)a * w[t * 2 + 1];
		
		/*这个地方想了好久也不知道怎么实现 参考了网上代码才知道可以用vector实现*/
		/*原本s是普通的int*，后来修改成vector<int> */
		s.push_back(s1);
		s.push_back(s2);
		dct.push_back(k1);
		dct.push_back(k2);

		middle[3][4] = s1;
		middle[4][3] = s2;

		bmp = IDCT(middle);
		for (int p = 0; p < N; p++) {
			for (int q = 0; q < N; q++) {
				outfile << bmp[p][q] << " ";
			}
			outfile << endl;
		}
	}
}