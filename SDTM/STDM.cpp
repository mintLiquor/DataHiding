#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<Windows.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<string>

using namespace std;
#define ZIGNUM 16  //Ƕ��ʱ��ÿ��8*8����ȡ��Ƶϵ���ĸ���


//����ȫ�ֱ���
double bmp[512][512]; //����ԭ����ͼ�������
double bmp_inserted[512][512]; //Ƕ�����ݺ��ͼ�����ؾ���
int bmp_logo[64][8];  //����logo�е����أ����ֽ�Ϊ��λ�洢
int bmp_insert[4096]; //��bmp_logoת��Ϊ��λΪ��λ�洢�õ���Ϊ��ҪǶ������ݣ���ɫ��Ϊ-1����ɫ��Ϊ1
int bmp_logo_decode[4096];  //�����õ���logoͼ����ֵ
unsigned char p_new[512 * 512]; //д��bmpͼ��ʱ����������ָ��pBmpBuf
unsigned char p_logo_new[512];//д��logoͼ��ʱ����������ָ��pBmpBuf


//====================================================================
//class BMP:ʵ��bmpͼ��Ķ�ȡ��д��
//====================================================================
class BMP
{
public:
	//����ȫ�ֱ���
	unsigned char* pBmpBuf; //����ͼ�����ݵ�ָ��
	int bmpWidth; //ͼ��Ŀ�(������Ϊ��λ)
	int bmpHeight; //ͼ��ĸ�
	RGBQUAD* pColorTable;//��ɫ��ָ��
	int biBitCount; //ÿ�����ص�λ��

	bool readBmp(char* bmpName);
	bool saveBmp(char* bmpName, unsigned char* imgBuf, int width,
		int height, int biBItCount, RGBQUAD* pColorTable);
};

//��ȡͼ���ļ������ɹ�����true��ʧ�ܷ���false
//�ú�����������
bool BMP::readBmp(char* bmpName)
{
	FILE* fp = fopen(bmpName, "rb"); //�Զ����Ʒ�ʽ��ָ�����ļ�
	if (fp == NULL)
	{
		cout << "��ȡͼƬ�ļ�ʧ�ܣ�" << endl;
		return false;
	}

	fseek(fp, sizeof(BITMAPFILEHEADER), 0); //����λͼ�ļ�ͷ�ṹBITMAPFILEHEADER

	BITMAPINFOHEADER head; //����λͼ��Ϣͷ�ṹ����
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp); //��ȡλͼ��Ϣͷ

	//��ȡͼ����ߡ�ÿ��������ռλ����ֵ
	bmpWidth = head.biWidth;
	bmpHeight = head.biHeight;
	biBitCount = head.biBitCount;

	//����ͼ��ÿ��������ռ���ֽ�����4�ı�����
	int lineByte;
	lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	//���ͼ�����ɫ�����Ϊ256����ȡ��ɫ������ڴ�
	if (biBitCount == 8)
	{
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
	}
	else if (biBitCount == 1)  //ͼ�����ɫ�����Ϊ1����ͼΪ�ڰ�ͼ
	{
		pColorTable = new RGBQUAD[2];
		fread(pColorTable, sizeof(RGBQUAD), 2, fp);
	}

	//��ȡλͼ���ݽ����ڴ�
	//����Ǻڰ�ͼ����Ϊ�ǰ��ֽڶ�ȡ��һ���ֽ����а˸����أ�һ������ռһ��bit����������Ҫ��ȡ����ÿһ��bit
	pBmpBuf = new unsigned char[lineByte * bmpHeight];
	fread(pBmpBuf, 1, lineByte * bmpHeight, fp);

	fclose(fp); //�ر��ļ�

	return true;
}

/*д��BMP�ļ�
����������
   char *bmpName-�ļ�����·��
   unsigned char *imgBuf-�������λͼ����
   int width-������Ϊ��λ�Ĵ�����λͼ�Ŀ�
   int height-������Ϊ��λ�Ĵ�����λͼ�ĸ�
   int biBItCount-ÿ��������ռλ��
   RGBQUAD *pColorTable-��ɫ��ָ��
����ֵ��
	����trueΪ�ɹ�������falseΪʧ��
 */

//�ú�����������
bool BMP::saveBmp(char* bmpName, unsigned char* imgBuf, int width,
	int height, int biBItCount, RGBQUAD* pColorTable)
{
	if (!imgBuf)
	{
		cout << "û�д�д���λͼ���ݣ�" << endl;
		return false;
	}

	int colorTablesize = 0; //��ɫ���С
	if (biBitCount == 8)
		colorTablesize = 1024; //�Ҷ�ͼ�����ɫ���СΪ1024

	int lineByte;
	lineByte = (width * biBitCount / 8 + 3) / 4 * 4;

	FILE* fp = fopen(bmpName, "wb");
	if (fp == NULL)
	{
		cout << "д��ͼ��ʧ�ܣ�" << endl;
		return false;
	}

	BITMAPFILEHEADER filehead; //λͼ�ļ�ͷ�ṹ����
	filehead.bfType = 0x4D42; //����λͼ�ļ�Ϊbmp����
	filehead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte * height;
	filehead.bfReserved1 = 0;
	filehead.bfReserved2 = 0;
	filehead.bfOffBits = 54 + colorTablesize;
	fwrite(&filehead, sizeof(BITMAPFILEHEADER), 1, fp);

	BITMAPINFOHEADER head;
	head.biBitCount = biBitCount;
	head.biClrImportant = 0;
	head.biClrUsed = 0;
	head.biCompression = 0;
	head.biHeight = height;
	head.biPlanes = 1;
	head.biSize = 40;
	head.biSizeImage = lineByte * height;
	head.biWidth = width;
	head.biXPelsPerMeter = 0;
	head.biYPelsPerMeter = 0;
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp); //д��λͼ��Ϣͷ

	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp); //���ǻҶ�ͼ��д����ɫ��

	else if (biBitCount == 1)  //ͼ�����ɫ�����Ϊ1����ͼΪ�ڰ�ͼ
	{
		fwrite(pColorTable, sizeof(RGBQUAD), 2, fp);
	}

	fwrite(imgBuf, lineByte * height, 1, fp);  //д��λͼ�����ļ�

	fclose(fp);  //�ر��ļ�
	return true;
}

//====================================================================
//class DCT��ʵ��DCT���任����任�Լ�Ƕ�롢�������
//====================================================================
class DCT
{
private:
	double dct[8][8];  //dct����
	double dct_orth[8][8];  //dct�����
	int* w;
	int count_w;
	//double a; //��������
	double fault; //�������ô�����
	void array_mul(double left[8][8], double right[8][8], double result[8][8]);  //����8*8������ˣ�result[8][8]=left[8][8]*right[8][8]
	void dct_exch(double x[8][8], double y[8][8]); //�Ծ���x����DCT���任: Y=TXT'   ���������y������
	void dct_exch_oppo(double y[8][8], double x[8][8]); //�Ծ���y����DCT��任: X=T'YT  ���������x������
	void insert_block(double x[8][8], int k, int b);  //��һ�飨x[8][8]������Ƕ�룬kΪ��Ƶϵ���ĸ�����bΪ��Ƕ�������
	void insert_all(double bmp[512][512], double bmp_inserted[512][512], int logo[4096], int k, double a1);   //�����е�4096�������Ƕ�루����Ƕ�����ʱ���ô˺�����

public:
	double a; //��������
	DCT();  //���캯��������dct����������
	~DCT()  //��������
	{
		if (w != NULL)
		{
			delete w; w = NULL;
		}
	}
	void decode_logo(double bmp_inserted[512][512], int bmp_logo_decode[4096], int bmp_logo[4096], int k); //���룬����ȡ��Ƕ���logoͼ������ֵ����������bmp_logo_decode��
	void Spread_Spectrum();//��Ƶˮӡ����ڣ�
};

//�ú�����������
DCT::DCT() //���캯��������dct����������
{
	//����dct����
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (i == 0)
				dct[i][j] = 1.0 / sqrt(8.0);
			else
				dct[i][j] = 0.5 * cos((2 * j + 1) * i * 3.14 / 16);
		}
	}
	//����dct�����
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
			dct_orth[i][j] = dct[j][i];
	}
	w = NULL;
	//��������w
	int k = ZIGNUM;
	w = new int[k];
	for (int i = 0; i < k; i++)
	{
		if (i % 2 == 0)
			w[i] = -1;
		else
			w[i] = 1;
	}
	count_w = 0;
}

//����8*8������ˣ�result[8][8]=left[8][8]*right[8][8]
void DCT::array_mul(double left[8][8], double right[8][8], double result[8][8])
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			result[i][j] = 0;
			for (int k = 0; k < 8; k++)
				result[i][j] = result[i][j] + left[i][k] * right[k][j];
		}
}

void DCT::dct_exch(double x[8][8], double y[8][8])   //DCT���任: Y=TXT', x������Ϊԭʼ�Ĵ�Ƕ�������
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			x[i][j] = x[i][j] - 128;

	double result[8][8];
	array_mul(dct, x, y);
	array_mul(y, dct_orth, result);
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			y[i][j] = result[i][j];

}


void DCT::dct_exch_oppo(double y[8][8], double x[8][8]) //DCT��任: X=T'YT
{
	double result[8][8];
	array_mul(dct_orth, y, x);
	array_mul(x, dct, result);

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			x[i][j] = result[i][j];

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			x[i][j] = x[i][j] + 128;

	//�����任�õ��Ľ������Ϊ����
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (x[i][j] < 0)
				x[i][j] = 0;
			else
				if (x[i][j] > 255)
					x[i][j] = 255;
				else  //��������
				{
					double a = x[i][j];
					int b;
					b = (int)a;
					double c = a - b;
					if (c > 0.50)
						b = b + 1;
					x[i][j] = b;
				}
		}

}

void DCT::insert_block(double x[8][8], int k, int b)  //��һ�飨x[8][8]������Ƕ�룬kΪ��Ƶϵ���ĸ�����bΪ��Ƕ������ݣ�aΪ��������
{
	double y[8][8];
	dct_exch(x, y); //����DCT���任

	double zig[36];  //zig��������ʱ����y����zigzag�����ǰ36��ϵ������Ƶϵ���������еĵ�6��36��λ��
	int i, j;
	int count_zig = 0;
	for (i = 0; i < 8; i++)
		for (j = 0; j < (i + 1); j++)
		{
			zig[count_zig] = y[i - j][j];
			count_zig++;
		}

	//����ͶӰ
	double x_ave = 0;
	for (i = 0; i < k; i++)
		x_ave += zig[6 + i] * w[i];
	x_ave = x_ave / k;

	double q;
	if (b == -1)
	{
		double temp = (x_ave + a / 4) / a + 0.5;
		if (temp < 0)
			q = a * ((int)(temp)-1) - a / 4;
		else
			q = a * ((int)(temp)) - a / 4;
	}
	else
	{
		double temp = (x_ave - a / 4) / a + 0.5;
		if (temp < 0)
			q = a * ((int)(temp)-1) + a / 4;
		else
			q = a * ((int)(temp)) + a / 4;
	}

	int t;
	for (t = 0; t < k; t++)
		zig[6 + t] = zig[6 + t] + (q - x_ave) * w[t];//����Ƕ��

	count_zig = 0;
	for (i = 0; i < 8; i++)
		for (j = 0; j < (i + 1); j++)
		{
			y[i - j][j] = zig[count_zig];  //��Ƕ����ϵ���Ż�ԭ����λ��
			count_zig++;
		}

	//��y������з��任�����д��x������
	dct_exch_oppo(y, x);
}

//�����е�4096�������Ƕ�룬kΪ��Ƶϵ���ĸ�����a1Ϊ��������;���д����� bmp_inserted[512][512]
void DCT::insert_all(double bmp[512][512], double bmp_inserted[512][512], int logo[4096], int k, double a1)   
{
	int i, j, m, n;
	int count_logo = 0;  //logo������ÿ����ҪǶ���λ����һ��8*8�����Ƕ��
	int b; //��Ƕ�������
	count_w = 0;
	a = a1;

	double block[8][8];
	for (i = 0; i < 512; i = i + 8)
		for (j = 0; j < 512; j = j + 8)
		{
			//����Ƕ�����������8*8����block��
			for (m = 0; m < 8; m++)
				for (n = 0; n < 8; n++)
					block[m][n] = bmp[i + m][j + n];

			b = logo[count_logo];
			count_logo++;  //ÿ��Ƕ��λ��һ��8*8�����Ƕ�루����4k��ϵ��Ƕ��һλb��

			insert_block(block, k, b); //����Ƕ��

			//�˺����Ƕ����blockд��bmp_inserted
			for (m = 0; m < 8; m++)
				for (n = 0; n < 8; n++)
					bmp_inserted[i + m][j + n] = block[m][n];
		}
}

//��ˮӡ��Ʒ���룬ͳ�ƴ�����
void DCT::decode_logo(double bmp_insert[512][512], int bmp_logo_decode[4096], int bmp_logo[4096], int k) 
{
	int i, j, m, n;
	int count_logo = 0;  //logo������ÿ����ҪǶ���λ����һ��8*8�����Ƕ��
	int count_fault = 0; //������ͳ��
	int b; //�������õ�����
	double y_ave = 0;

	double block[8][8];
	for (i = 0; i < 512; i = i + 8)
		for (j = 0; j < 512; j = j + 8) //��ÿһ��8*8����н���
		{
			//�����������������8*8����block��
			for (m = 0; m < 8; m++)
				for (n = 0; n < 8; n++)
					block[m][n] = bmp_insert[i + m][j + n];  //block����Ϊ��ǰ8*8���ؿ�

			double y[8][8];
			dct_exch(block, y);  //DCT���任���������y����

			double zig[36];  //zig��������ʱ����y����zigzag�����ǰ36��ϵ������Ƶϵ���������еĵ�6��36��λ��
			int count_zig = 0;

			for (m = 0; m < 8; m++)
				for (n = 0; n < (m + 1); n++)
				{
					zig[count_zig] = y[m - n][n]; //ȡ����Ƶϵ������
					count_zig++;
				}

			y_ave = 0;
			for (m = 0; m < k; m++)
				y_ave += zig[6 + m] * w[m];
			y_ave = y_ave / k;

			double temp = (y_ave + a / 4) / a + 0.5;
			if (temp < 0)
				temp = a * ((int)(temp)-1) - a / 4 - y_ave;
			else
				temp = a * ((int)(temp)) - a / 4 - y_ave;

			if (temp<(a / 4) && temp>-(a / 4))
				b = -1;
			else
				b = 1;

			bmp_logo_decode[count_logo] = b;
			if (bmp_logo[count_logo] != b)
				count_fault++; //���������
			count_logo++;
		}
	fault = (double)count_fault / 4096;
	//������õ���bmp_logo_decode���鰴˳��д��p_logo_newָ����ָ�����Ա㽫�����logoͼ
	int count_new_logo = 0;
	int count_eight = 0;
	for (int i = 0; i < 4096;)
	{
		p_logo_new[count_new_logo] = 0;
		for (int j = 0; j < 8; j++, i++)
		{
			char c = 0;
			char c2;
			if (bmp_logo_decode[7 - j + count_new_logo * 8] == 1)
			{
				c2 = ((c >> j) | 0x01) << j;
				p_logo_new[count_new_logo] |= c2;
			}
		}
		count_new_logo++;
	}
	//���������
	cout << "����õ��Ĵ�����Ϊ��" << fault << endl;
}

//��Ƶˮӡ����ڣ�
void DCT::Spread_Spectrum()
{
	cout << endl << ">>>>>>>>>>>>>>������һ����ƵˮӡǶ��<<<<<<<<<<<<<<" << endl << endl;
	//=============��LENAͼ==============================
	BMP m;
	string path_in;
	cout << "�����뽫Ҫ��Ƕ�����ݵ�ͼ��LENAͼ��·������·������ʹ��˫б�ܣ���d:\\\\LENA.bmp��" << endl;
	cin >> path_in;
	char* path = _strdup(path_in.c_str());

	if (m.readBmp(path))
		cout << "��ȡ�ɹ���" << endl;
	else
	{
		cout << "ͼ���ȡʧ�ܣ�" << endl;
		exit(0);
	}
	cout << endl << "��ͼ��Ŀ�Ϊ��������Ϊ��λ��:" << m.bmpWidth << endl;
	cout << "��ͼ��ĸ�Ϊ��������Ϊ��λ��:" << m.bmpHeight << endl << endl;
	int lineByte;
	lineByte = (m.bmpWidth * m.biBitCount / 8 + 3) / 4 * 4;
	//����������ת�Ƶ�512*512����bmp��bmp_inserted��
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
		{
			bmp[i][j] = (*m.pBmpBuf);
			bmp_inserted[i][j] = bmp[i][j];
			m.pBmpBuf++;
		}

	//============��logoͼ===============================
	BMP m2;
	string path2_in;
	cout << "�����뽫ҪǶ���logoͼ·������·������ʹ��˫б�ܣ���d:\\\\tj-logo.bmp��" << endl;
	cin >> path2_in;
	char* path2 = _strdup(path2_in.c_str());

	if (m2.readBmp(path2))
		cout << "��ȡ�ɹ���" << endl;
	else
	{
		cout << "ͼ���ȡʧ�ܣ�" << endl;
		exit(0);
	}
	cout << endl << "��ͼ��Ŀ�Ϊ��������Ϊ��λ��:" << m2.bmpWidth << endl;
	cout << "��ͼ��ĸ�Ϊ��������Ϊ��λ��:" << m2.bmpHeight << endl << endl;

	int lineByte2;
	lineByte2 = (m2.bmpWidth * m2.biBitCount / 8 + 3) / 4 * 4;

	//����������ת�Ƶ�64*8����bmp_logo��
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 8; j++)
		{
			bmp_logo[i][j] = (int)(*m2.pBmpBuf);
			m2.pBmpBuf++;
		}

	//�����ذ�λд��bmp_insert������
	int count = 0;
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 8; j++)
		{
			int a;
			a = bmp_logo[i][j];
			for (int k = 0; k < 8; k++)
			{
				bmp_insert[7 - k + count * 8] = ((a >> k) & 0x01);
				if (bmp_insert[7 - k + count * 8] == 0)
					bmp_insert[7 - k + count * 8] = -1;   //����ֵ0��-1��ʾ
			}
			count++;
		}

	double strength;
	cout << "����������������";
	cin >> strength;

	//==============����Ƕ��===============================
	insert_all(bmp, bmp_inserted, bmp_insert, ZIGNUM, strength); //kΪ10��Ƕ��ǿ��Ϊ1
	cout << endl << "Ƕ��ɹ���" << endl;

	//==============���ͼ��===============================	
	string path3_in;
	cout << "������Ƕ����bmpͼ�������뱣��·������·������ʹ��˫б�ܣ���d:\\\\result.bmp��" << endl;
	cin >> path3_in;
	char* path3 = _strdup(path3_in.c_str());

	int count_new = 0;
	for (int i = 0; i < 512; i++)   //������ת����p_newָ��
		for (int j = 0; j < 512; j++)
		{
			p_new[count_new] = (unsigned char)bmp_inserted[i][j];
			count_new++;
		}
	if (m.saveBmp(path3, p_new, 512, 512, 8, m.pColorTable))
		cout << "ͼ�񱣴�ɹ���" << endl;
	else
	{
		cout << "ͼ�񱣴�ʧ��"; exit(0);
	}
	//==============���н���==============================
	cout << endl << "��ˮӡͼ����н���..." << endl;
	decode_logo(bmp_inserted, bmp_logo_decode, bmp_insert, ZIGNUM);

	//==============���������logoͼ========================
	cout << "�Ƿ���Ҫ���������logoͼ��y/n����:";
	char temp = 'n';
	cin >> temp;

	if (temp == 'y' || temp == 'Y')
	{
		string path4_in;
		cout << "�����������logoͼ�������뱣��·������·������ʹ��˫б�ܣ���d:\\\\logo.bmp��" << endl;
		cin >> path4_in;
		char* path4 = _strdup(path4_in.c_str());
		if (m2.saveBmp(path4, p_logo_new, 64, 64, 1, m2.pColorTable))
			cout << "����ɹ���" << endl;
		else
		{
			cout << "����ʧ�ܣ�"; exit(0);
		}
		free(path4);
	}
	free(path);
	free(path2);
	free(path3);
}


//====================================================================
//������
//====================================================================
int main()
{
	cout << "====================================================" << endl;
	cout << "          �������ʵ�����¹��ܣ�                                             " << endl;
	cout << "          1.ʹ�ò�ͬ��Ƕ��ǿ�ȣ���ͼ��Ƕ��ˮӡ������               " << endl;
	cout << "          2. ���ܵ�������ˮӡ��Ʒ���н���                                 " << endl;
	cout << "====================================================" << endl << endl;

	DCT t;
	t.Spread_Spectrum();

	char temp = 'n';
	cout << endl << "�Ƿ�����ѡ���ܣ�y/n����";
	cin >> temp;
	while (temp == 'y' || temp == 'Y')
	{
		cout << "====================================================" << endl;
		cout << "           �������ʵ�����¹��ܣ�                                             " << endl;
		cout << "          1.ʹ�ò�ͬ��Ƕ��ǿ�ȣ���ͼ��Ƕ��ˮӡ������               " << endl;
		cout << "          2.���ܵ�������ˮӡ��Ʒ���н���                                 " << endl;
		cout << "====================================================" << endl << endl;
		cout << "��ѡ������ţ�1��2����";
		int temp2;
		cin >> temp2;
		if (temp2 == 1)
			t.Spread_Spectrum();
		if (temp2 == 2)
		{
			cout << endl << ">>>>>>>>>>>>>>�����ܵ�������ˮӡ��Ʒ���н���<<<<<<<<<<<<<<" << endl << endl;
			BMP m2;
			string path_in;
			cout << "�����뽫Ҫ�������ͼ��·����" << endl;
			cin >> path_in;
			char* path = _strdup(path_in.c_str());

			if (m2.readBmp(path))
				cout << "��ȡ�ɹ���" << endl;
			else
			{
				cout << "ͼ���ȡʧ�ܣ�" << endl;
				exit(0);
			}

			//����������ת�Ƶ�512*512����bmp_inserted��
			for (int i = 0; i < 512; i++)
				for (int j = 0; j < 512; j++)
				{
					bmp_inserted[i][j] = (*m2.pBmpBuf);
					m2.pBmpBuf++;
				}
			DCT t2;
			cout << endl << "�������ܵ�������ˮӡ��Ʒ�ڽ���ˮӡǶ��ʱѡȡ������������";
			cin >> t2.a;
			t2.decode_logo(bmp_inserted, bmp_logo_decode, bmp_insert, ZIGNUM);
			free(path);
		}
		cout << endl << "�Ƿ�����ѡ���ܣ�y/n����";
		cin >> temp;
	}
}
