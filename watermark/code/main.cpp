#define _CRT_SECURE_NO_WARNINGS

#include"bmp.h"
#include"RandomWord.h"

#include<iostream>
#include<Windows.h>
#include<cmath>
#include<random>
#include<vector>

using namespace std;

vector<int> w(MAX, 1);//�����01����

void init()
{
	/*��input���������*/
	for (int i = MIN; i < MAX; i++)
		w[i] = -1;
	RandomGen(w);
}

int main()
{
	char bmpPath[] = "lena.bmp";
	readPixel(bmpPath);
	char logoPath[] = "tj-logo.bmp";
	logo2txt(logoPath);

	init();
	assDecode(w);
	convertPixel();
	return 0;
}