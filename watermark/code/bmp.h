#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<cmath>
#include<vector>
#include<stdlib.h>
#include<fstream>
#include<Windows.h>

using namespace std;

unsigned char* pBmpBuf;//��ȡͼ���ָ��
int bmpHeight, bmpWidth;
int biBitCount;//ÿ����λ��
RGBQUAD* pColorTable;


/*BMP�ļ���ȡ�����ο�����*/
/*�ο���ַΪ https://blog.csdn.net/wobuzhidao8/article/details/67638107 */
/*����һ��ͼ��λͼ���ݡ����ߡ���ɫ��ָ�뼰ÿ������ռ��λ������Ϣ������д��ָ���ļ�*/
bool readBmp(char* bmpName)
{
	/*�����ƴ�ָ���ļ�*/
	FILE* fp = fopen(bmpName, "rb");
	if (fp == 0)
		return 0;

	/*����λͼ�ļ�ͷ�ṹbitmapfileheader*/
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	/*����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣ���ڴ棬�����head��*/
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);//��ȡͼ����Ϣ
	bmpWidth = head.biWidth;
	bmpHeight = head.biHeight;
	biBitCount = head.biBitCount;//�������������ͼ��ÿ��������ռ���ֽ�����������4�ı���
	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;//�Ҷ�ͼ������ɫ������ɫ�����Ϊ256

	if (biBitCount == 8){
		//����ռ��ȡ��ɫ����ڴ�
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	//����λͼ��������Ҫ�Ŀռ䣬��λͼ���ݽ��ڴ�
	pBmpBuf = new unsigned char[lineByte * bmpHeight];
	fread(pBmpBuf, 1, lineByte * bmpHeight, fp);
	fclose(fp);
	return 1;
}


bool saveBmp(char* bmpName, unsigned char* imgBuf, int width, int height, int biBitCount, RGBQUAD* pColorTable)
{
	if (!imgBuf)
		return 0;

	/*��ɫ���С�����ֽ�Ϊ��λ*/
	/*�Ҷ�ͼ����ɫ��Ϊ1024�ֽڣ���ɫͼ����ɫ���СΪ0*/
	int colorTablesize = 0;
	if (biBitCount == 8) 
		colorTablesize = 1024;

	/*���洢ͼ������ÿ���ֽ���Ϊ4�ı���*/
	int lineByte = (width + biBitCount / 8 + 3) / 4 * 4;

	/*������д�ķ�ʽ���ļ�*/
	FILE* fp = fopen(bmpName, "wb");
	if (fp == 0)
		return 0;

	/*����λͼ�ļ�ͷ�ṹ��������д�ļ�ͷ��Ϣ*/
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 0x4D42;//bmp����
	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte * height;
	fileHead.bfReserved1 = 1;
	fileHead.bfReserved2 = 1;

	/*bfoffbits:ͼ���ļ�ǰ������������ռ�֮��*/
	fileHead.bfOffBits = 54 + colorTablesize;

	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

	BITMAPINFOHEADER head;
	head.biBitCount = biBitCount;
	head.biClrImportant = 0;
	head.biClrUsed = 0;
	head.biCompression = 0;
	head.biHeight = height;
	head.biPlanes = 1;
	head.biSize = 40;
	head.biSizeImage = lineByte*height;
	head.biWidth = width;
	head.biXPelsPerMeter = 0;
	head.biYPelsPerMeter = 0;

	/*��λͼ��Ϣͷд���ڴ�*/
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp);
	/*��λͼ����д���ļ�*/
	fwrite(imgBuf, height * lineByte, 1, fp);

	fclose(fp);
	return 1;
}

/*��ȡ����*/
/*bmp->txt*/
void readPixel(char path[])
{
	readBmp(path);
	cout << "width=" << bmpWidth << endl << "height=" << bmpHeight << endl << "biBitCount=" << biBitCount << endl;
	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	int PixelCount = 0;
	ofstream outfile("pixel.txt", ios::in | ios::trunc);
	for (int i = 0; i < bmpHeight; i += 8) {
		for (int j = 0; j < bmpHeight; j += 8) {
			for (int k = 0; k < 8; k++) {
				for (int t = 0; t < 8; t++) {
					outfile << (int) * (pBmpBuf + lineByte * (i + k) + j + t) << " ";
				}
				outfile << endl;
			}
		}
	}
	char pathout[] = "convert.bmp";
	saveBmp(pathout, pBmpBuf, bmpWidth, bmpHeight, biBitCount, pColorTable);
}

/*�溯��*/
/*txt->bmp*/
void convertPixel()
{
	ifstream infile;
	infile.open("DCT.txt");

	unsigned char* pBmpBuf;
	pBmpBuf = new unsigned char[512 * 512];//lena�Ҷ�ͼ�Ĵ�С
	int lineByte= (512 * biBitCount / 8 + 3) / 4 * 4;
    int PixelCount = 0;
	int temp;

	for (int i = 0; i < 512; i += 8) {
		for (int j = 0; j < 512; j += 8) {
			for (int k = 0; k < 8; k++) {
				for (int t = 0; t < 8; t++) {
					infile >> temp;
					*(pBmpBuf + lineByte * (i + k) + j + t) = temp;
				}
			}
		}
	}

	char pathout[] = "watermark.bmp";
	saveBmp(pathout, pBmpBuf, bmpWidth, bmpHeight, biBitCount, pColorTable);
}


/*��ȡ�ڰ�bmp�ļ�*/
/*������Դ���磬��ԭ��*/
/*�ο���ַ��https://blog.csdn.net/tounaobun/article/details/17377777 */
unsigned char* read_1bmp(char* fname, int* _w, int* _h)
{
	unsigned char head[54];
	FILE* f = fopen(fname, "rb");

	// BMP header is 54 bytes
	fread(head, 1, 54, f);

	int w = head[18] + (((int)head[19]) << 8) + (((int)head[20]) << 16) + (((int)head[21]) << 24);
	int h = head[22] + (((int)head[23]) << 8) + (((int)head[24]) << 16) + (((int)head[25]) << 24);

	// lines are aligned on 4-byte boundary
	int lineSize = (w / 8 + (w / 8) % 4);
	int fileSize = lineSize * h;

	unsigned char* img = (unsigned char*)malloc(w * h), * data = (unsigned char*)malloc(fileSize);

	// skip the header
	fseek(f, 54, SEEK_SET);

	// skip palette - two rgb quads, 8 bytes
	fseek(f, 8, SEEK_CUR);

	// read data
	fread(data, 1, fileSize, f);

	// decode bits
	int i, j, k, rev_j;
	for (j = 0, rev_j = h - 1; j < h; j++, rev_j--) {
		for (i = 0; i < w / 8; i++) {
			int fpos = j * lineSize + i, pos = rev_j * w + i * 8;
			for (k = 0; k < 8; k++)
				img[pos + (7 - k)] = (data[fpos] >> k) & 1;
		}
	}

	free(data);
	*_w = w; *_h = h;
	return img;
}

void logo2txt(char readPath[]) {
	ofstream outfile;
	outfile.open("logo.txt");

	int w, h, i, j;
	unsigned char* img = read_1bmp(readPath, &w, &h);

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++) {
			outfile << (img[j * w + i] ? 1 : -1) << " ";
			//cout << (img[j * w + i] ? '0' : '1') << " ";
		}
		//cout << endl;
		outfile << endl;
	}
}

void readPixel2(char path[])
{
	readBmp(path);
	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	int PixelCount = 0;
	ofstream outfile("logo.txt", ios::in | ios::trunc);
	for (int i = 0; i < bmpHeight; i += 8) {
		for (int j = 0; j < bmpHeight; j += 8) {
			for (int k = 0; k < 8; k++) {
				for (int t = 0; t < 8; t++) {
					outfile << (int) * (pBmpBuf + lineByte * (i + k) + j + t) << " ";
				}
				outfile << endl;
			}
		}
	}
}

