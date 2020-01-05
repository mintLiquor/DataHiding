#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<Windows.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<string>

using namespace std;
#define ZIGNUM 16  //嵌入时，每个8*8块中取中频系数的个数


//定义全局变量
double bmp[512][512]; //储存原宿主图像的像素
double bmp_inserted[512][512]; //嵌入数据后的图像像素矩阵
int bmp_logo[64][8];  //保存logo中的像素，以字节为单位存储
int bmp_insert[4096]; //由bmp_logo转化为以位为单位存储得到，为将要嵌入的数据，黑色记为-1，白色记为1
int bmp_logo_decode[4096];  //解码后得到的logo图像素值
unsigned char p_new[512 * 512]; //写入bmp图像时的像素数据指针pBmpBuf
unsigned char p_logo_new[512];//写入logo图像时的像素数据指针pBmpBuf


//====================================================================
//class BMP:实现bmp图像的读取和写入
//====================================================================
class BMP
{
public:
	//定义全局变量
	unsigned char* pBmpBuf; //读入图像数据的指针
	int bmpWidth; //图像的宽(以像素为单位)
	int bmpHeight; //图像的高
	RGBQUAD* pColorTable;//颜色表指针
	int biBitCount; //每个像素的位数

	bool readBmp(char* bmpName);
	bool saveBmp(char* bmpName, unsigned char* imgBuf, int width,
		int height, int biBItCount, RGBQUAD* pColorTable);
};

//读取图像文件，若成功返回true，失败返回false
//该函数来自网络
bool BMP::readBmp(char* bmpName)
{
	FILE* fp = fopen(bmpName, "rb"); //以二进制方式打开指定的文件
	if (fp == NULL)
	{
		cout << "读取图片文件失败！" << endl;
		return false;
	}

	fseek(fp, sizeof(BITMAPFILEHEADER), 0); //跳过位图文件头结构BITMAPFILEHEADER

	BITMAPINFOHEADER head; //定义位图信息头结构变量
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp); //读取位图信息头

	//获取图像宽、高、每个像素所占位数的值
	bmpWidth = head.biWidth;
	bmpHeight = head.biHeight;
	biBitCount = head.biBitCount;

	//计算图像每行像素所占的字节数（4的倍数）
	int lineByte;
	lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	//如果图像的颜色表表项为256，读取颜色表进入内存
	if (biBitCount == 8)
	{
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
	}
	else if (biBitCount == 1)  //图像的颜色表表项为1，改图为黑白图
	{
		pColorTable = new RGBQUAD[2];
		fread(pColorTable, sizeof(RGBQUAD), 2, fp);
	}

	//读取位图数据进入内存
	//如果是黑白图像，因为是按字节读取，一个字节中有八个像素（一个像素占一个bit），所以需要提取其中每一个bit
	pBmpBuf = new unsigned char[lineByte * bmpHeight];
	fread(pBmpBuf, 1, lineByte * bmpHeight, fp);

	fclose(fp); //关闭文件

	return true;
}

/*写入BMP文件
函数参数：
   char *bmpName-文件名和路径
   unsigned char *imgBuf-待保存的位图数据
   int width-以像素为单位的待保存位图的宽
   int height-以像素为单位的待保存位图的高
   int biBItCount-每个像素所占位数
   RGBQUAD *pColorTable-颜色表指针
返回值：
	返回true为成功，返回false为失败
 */

//该函数来自网络
bool BMP::saveBmp(char* bmpName, unsigned char* imgBuf, int width,
	int height, int biBItCount, RGBQUAD* pColorTable)
{
	if (!imgBuf)
	{
		cout << "没有待写入的位图数据！" << endl;
		return false;
	}

	int colorTablesize = 0; //颜色表大小
	if (biBitCount == 8)
		colorTablesize = 1024; //灰度图像的颜色表大小为1024

	int lineByte;
	lineByte = (width * biBitCount / 8 + 3) / 4 * 4;

	FILE* fp = fopen(bmpName, "wb");
	if (fp == NULL)
	{
		cout << "写入图像失败！" << endl;
		return false;
	}

	BITMAPFILEHEADER filehead; //位图文件头结构变量
	filehead.bfType = 0x4D42; //定义位图文件为bmp类型
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
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp); //写入位图信息头

	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp); //若是灰度图像，写入颜色表

	else if (biBitCount == 1)  //图像的颜色表表项为1，改图为黑白图
	{
		fwrite(pColorTable, sizeof(RGBQUAD), 2, fp);
	}

	fwrite(imgBuf, lineByte * height, 1, fp);  //写入位图数据文件

	fclose(fp);  //关闭文件
	return true;
}

//====================================================================
//class DCT：实现DCT正变换、逆变换以及嵌入、解码操作
//====================================================================
class DCT
{
private:
	double dct[8][8];  //dct矩阵
	double dct_orth[8][8];  //dct逆矩阵
	int* w;
	int count_w;
	//double a; //量化步长
	double fault; //解码所得错误率
	void array_mul(double left[8][8], double right[8][8], double result[8][8]);  //两个8*8矩阵相乘：result[8][8]=left[8][8]*right[8][8]
	void dct_exch(double x[8][8], double y[8][8]); //对矩阵x进行DCT正变换: Y=TXT'   结果储存在y矩阵中
	void dct_exch_oppo(double y[8][8], double x[8][8]); //对矩阵y进行DCT逆变换: X=T'YT  结果储存在x矩阵中
	void insert_block(double x[8][8], int k, int b);  //对一块（x[8][8]）进行嵌入，k为中频系数的个数，b为待嵌入的数据
	void insert_all(double bmp[512][512], double bmp_inserted[512][512], int logo[4096], int k, double a1);   //对所有的4096个块进行嵌入（进行嵌入操作时调用此函数）

public:
	double a; //量化步长
	DCT();  //构造函数：计算dct矩阵和逆矩阵
	~DCT()  //析构函数
	{
		if (w != NULL)
		{
			delete w; w = NULL;
		}
	}
	void decode_logo(double bmp_inserted[512][512], int bmp_logo_decode[4096], int bmp_logo[4096], int k); //解码，解码取出嵌入的logo图的像素值，存入数组bmp_logo_decode中
	void Spread_Spectrum();//扩频水印（入口）
};

//该函数来自网络
DCT::DCT() //构造函数：计算dct矩阵和逆矩阵
{
	//计算dct矩阵
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
	//计算dct逆矩阵
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
			dct_orth[i][j] = dct[j][i];
	}
	w = NULL;
	//产生数组w
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

//两个8*8矩阵相乘：result[8][8]=left[8][8]*right[8][8]
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

void DCT::dct_exch(double x[8][8], double y[8][8])   //DCT正变换: Y=TXT', x数组中为原始的待嵌入的数据
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


void DCT::dct_exch_oppo(double y[8][8], double x[8][8]) //DCT逆变换: X=T'YT
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

	//将反变换得到的结果调整为整数
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (x[i][j] < 0)
				x[i][j] = 0;
			else
				if (x[i][j] > 255)
					x[i][j] = 255;
				else  //四舍五入
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

void DCT::insert_block(double x[8][8], int k, int b)  //对一块（x[8][8]）进行嵌入，k为中频系数的个数，b为待嵌入的数据，a为量化步长
{
	double y[8][8];
	dct_exch(x, y); //进行DCT正变换

	double zig[36];  //zig数组中暂时保存y矩阵按zigzag排序的前36个系数，中频系数在数组中的第6到36个位置
	int i, j;
	int count_zig = 0;
	for (i = 0; i < 8; i++)
		for (j = 0; j < (i + 1); j++)
		{
			zig[count_zig] = y[i - j][j];
			count_zig++;
		}

	//计算投影
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
		zig[6 + t] = zig[6 + t] + (q - x_ave) * w[t];//进行嵌入

	count_zig = 0;
	for (i = 0; i < 8; i++)
		for (j = 0; j < (i + 1); j++)
		{
			y[i - j][j] = zig[count_zig];  //将嵌入后的系数放回原来的位置
			count_zig++;
		}

	//对y矩阵进行反变换，结果写回x矩阵中
	dct_exch_oppo(y, x);
}

//对所有的4096个块进行嵌入，k为中频系数的个数，a1为量化步长;结果写入矩阵 bmp_inserted[512][512]
void DCT::insert_all(double bmp[512][512], double bmp_inserted[512][512], int logo[4096], int k, double a1)   
{
	int i, j, m, n;
	int count_logo = 0;  //logo数组中每个需要嵌入的位都对一个8*8块进行嵌入
	int b; //待嵌入的数据
	count_w = 0;
	a = a1;

	double block[8][8];
	for (i = 0; i < 512; i = i + 8)
		for (j = 0; j < 512; j = j + 8)
		{
			//将待嵌入的数据移入8*8数组block中
			for (m = 0; m < 8; m++)
				for (n = 0; n < 8; n++)
					block[m][n] = bmp[i + m][j + n];

			b = logo[count_logo];
			count_logo++;  //每个嵌入位对一个8*8块进行嵌入（即用4k个系数嵌入一位b）

			insert_block(block, k, b); //进行嵌入

			//此后需把嵌入后的block写回bmp_inserted
			for (m = 0; m < 8; m++)
				for (n = 0; n < 8; n++)
					bmp_inserted[i + m][j + n] = block[m][n];
		}
}

//对水印作品解码，统计错误率
void DCT::decode_logo(double bmp_insert[512][512], int bmp_logo_decode[4096], int bmp_logo[4096], int k) 
{
	int i, j, m, n;
	int count_logo = 0;  //logo数组中每个需要嵌入的位都对一个8*8块进行嵌入
	int count_fault = 0; //错误率统计
	int b; //解码所得的数据
	double y_ave = 0;

	double block[8][8];
	for (i = 0; i < 512; i = i + 8)
		for (j = 0; j < 512; j = j + 8) //对每一个8*8块进行解码
		{
			//将待解码的数据移入8*8数组block中
			for (m = 0; m < 8; m++)
				for (n = 0; n < 8; n++)
					block[m][n] = bmp_insert[i + m][j + n];  //block数组为当前8*8像素块

			double y[8][8];
			dct_exch(block, y);  //DCT正变换，结果存入y矩阵

			double zig[36];  //zig数组中暂时保存y矩阵按zigzag排序的前36个系数，中频系数在数组中的第6到36个位置
			int count_zig = 0;

			for (m = 0; m < 8; m++)
				for (n = 0; n < (m + 1); n++)
				{
					zig[count_zig] = y[m - n][n]; //取出中频系数序列
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
				count_fault++; //解码错误率
			count_logo++;
		}
	fault = (double)count_fault / 4096;
	//将解码得到的bmp_logo_decode数组按顺序写入p_logo_new指针所指区域，以便将来输出logo图
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
	//输出错误率
	cout << "解码得到的错误率为：" << fault << endl;
}

//扩频水印（入口）
void DCT::Spread_Spectrum()
{
	cout << endl << ">>>>>>>>>>>>>>将进行一次扩频水印嵌入<<<<<<<<<<<<<<" << endl << endl;
	//=============读LENA图==============================
	BMP m;
	string path_in;
	cout << "请输入将要被嵌入数据的图像（LENA图）路径：（路径中请使用双斜杠，如d:\\\\LENA.bmp）" << endl;
	cin >> path_in;
	char* path = _strdup(path_in.c_str());

	if (m.readBmp(path))
		cout << "读取成功！" << endl;
	else
	{
		cout << "图像读取失败！" << endl;
		exit(0);
	}
	cout << endl << "该图像的宽为（以像素为单位）:" << m.bmpWidth << endl;
	cout << "该图像的高为（以像素为单位）:" << m.bmpHeight << endl << endl;
	int lineByte;
	lineByte = (m.bmpWidth * m.biBitCount / 8 + 3) / 4 * 4;
	//将像素数据转移到512*512矩阵bmp和bmp_inserted中
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
		{
			bmp[i][j] = (*m.pBmpBuf);
			bmp_inserted[i][j] = bmp[i][j];
			m.pBmpBuf++;
		}

	//============读logo图===============================
	BMP m2;
	string path2_in;
	cout << "请输入将要嵌入的logo图路径：（路径中请使用双斜杠，如d:\\\\tj-logo.bmp）" << endl;
	cin >> path2_in;
	char* path2 = _strdup(path2_in.c_str());

	if (m2.readBmp(path2))
		cout << "读取成功！" << endl;
	else
	{
		cout << "图像读取失败！" << endl;
		exit(0);
	}
	cout << endl << "该图像的宽为（以像素为单位）:" << m2.bmpWidth << endl;
	cout << "该图像的高为（以像素为单位）:" << m2.bmpHeight << endl << endl;

	int lineByte2;
	lineByte2 = (m2.bmpWidth * m2.biBitCount / 8 + 3) / 4 * 4;

	//将像素数据转移到64*8矩阵bmp_logo中
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 8; j++)
		{
			bmp_logo[i][j] = (int)(*m2.pBmpBuf);
			m2.pBmpBuf++;
		}

	//将像素按位写入bmp_insert数组中
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
					bmp_insert[7 - k + count * 8] = -1;   //像素值0用-1表示
			}
			count++;
		}

	double strength;
	cout << "请输入量化步长：";
	cin >> strength;

	//==============进行嵌入===============================
	insert_all(bmp, bmp_inserted, bmp_insert, ZIGNUM, strength); //k为10，嵌入强度为1
	cout << endl << "嵌入成功！" << endl;

	//==============输出图像===============================	
	string path3_in;
	cout << "将保存嵌入后的bmp图像，请输入保存路径：（路径中请使用双斜杠，如d:\\\\result.bmp）" << endl;
	cin >> path3_in;
	char* path3 = _strdup(path3_in.c_str());

	int count_new = 0;
	for (int i = 0; i < 512; i++)   //把像素转移至p_new指针
		for (int j = 0; j < 512; j++)
		{
			p_new[count_new] = (unsigned char)bmp_inserted[i][j];
			count_new++;
		}
	if (m.saveBmp(path3, p_new, 512, 512, 8, m.pColorTable))
		cout << "图像保存成功！" << endl;
	else
	{
		cout << "图像保存失败"; exit(0);
	}
	//==============进行解码==============================
	cout << endl << "对水印图像进行解码..." << endl;
	decode_logo(bmp_inserted, bmp_logo_decode, bmp_insert, ZIGNUM);

	//==============输出解码后的logo图========================
	cout << "是否需要保存解码后的logo图（y/n）？:";
	char temp = 'n';
	cin >> temp;

	if (temp == 'y' || temp == 'Y')
	{
		string path4_in;
		cout << "将保存解码后的logo图像，请输入保存路径：（路径中请使用双斜杠，如d:\\\\logo.bmp）" << endl;
		cin >> path4_in;
		char* path4 = _strdup(path4_in.c_str());
		if (m2.saveBmp(path4, p_logo_new, 64, 64, 1, m2.pColorTable))
			cout << "保存成功！" << endl;
		else
		{
			cout << "保存失败！"; exit(0);
		}
		free(path4);
	}
	free(path);
	free(path2);
	free(path3);
}


//====================================================================
//主函数
//====================================================================
int main()
{
	cout << "====================================================" << endl;
	cout << "          本程序可实现以下功能：                                             " << endl;
	cout << "          1.使用不同的嵌入强度，对图像嵌入水印并解码               " << endl;
	cout << "          2. 对受到攻击的水印作品进行解码                                 " << endl;
	cout << "====================================================" << endl << endl;

	DCT t;
	t.Spread_Spectrum();

	char temp = 'n';
	cout << endl << "是否重新选择功能（y/n）？";
	cin >> temp;
	while (temp == 'y' || temp == 'Y')
	{
		cout << "====================================================" << endl;
		cout << "           本程序可实现以下功能：                                             " << endl;
		cout << "          1.使用不同的嵌入强度，对图像嵌入水印并解码               " << endl;
		cout << "          2.对受到攻击的水印作品进行解码                                 " << endl;
		cout << "====================================================" << endl << endl;
		cout << "请选择功能序号（1或2）：";
		int temp2;
		cin >> temp2;
		if (temp2 == 1)
			t.Spread_Spectrum();
		if (temp2 == 2)
		{
			cout << endl << ">>>>>>>>>>>>>>将对受到攻击的水印作品进行解码<<<<<<<<<<<<<<" << endl << endl;
			BMP m2;
			string path_in;
			cout << "请输入将要被解码的图像路径：" << endl;
			cin >> path_in;
			char* path = _strdup(path_in.c_str());

			if (m2.readBmp(path))
				cout << "读取成功！" << endl;
			else
			{
				cout << "图像读取失败！" << endl;
				exit(0);
			}

			//将像素数据转移到512*512矩阵bmp_inserted中
			for (int i = 0; i < 512; i++)
				for (int j = 0; j < 512; j++)
				{
					bmp_inserted[i][j] = (*m2.pBmpBuf);
					m2.pBmpBuf++;
				}
			DCT t2;
			cout << endl << "请输入受到攻击的水印作品在进行水印嵌入时选取的量化步长：";
			cin >> t2.a;
			t2.decode_logo(bmp_inserted, bmp_logo_decode, bmp_insert, ZIGNUM);
			free(path);
		}
		cout << endl << "是否重新选择功能（y/n）？";
		cin >> temp;
	}
}
