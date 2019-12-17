#pragma once

#include<iostream>
#include<time.h>
#include<cmath>
#include<vector>
#include<random>
#include"DCT.h"
using namespace std;

#define MAX 10000
#define MIN 5000

int randint(int i) {
	std::random_device rd;
	std::mt19937 gen(rd());

	uniform_int_distribution<int> dis(i, MAX - 1);
	int res = dis(gen);

	return res;
}

void RandomGen(vector<int>& r)
{
	for (int i = 0; i < r.size(); ++i)
		swap(r[i], r[randint(i)]);
}