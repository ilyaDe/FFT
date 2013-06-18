#include "FFT_test.h"
#define _USE_MATH_DEFINES 1
#include <math.h>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <complex>


using namespace std;

double inputBuff[1024];
double outputBuff[1024];
double *pointBuf1[1024];
double *pointBuf2[1024];

complex<double> inputBuffC[1024];
complex<double> outputBuffC[1024];
complex<double> *pointBuf1C[1024];
complex<double> *pointBuf2C[1024];

// применять только при n > 2
complex<double> Wnk(unsigned int n)
{
	double tmp;
	int pow = 1 << n;
	tmp = M_PI * 2 / pow;
	complex<double> result(cos(tmp),-sin(tmp));
	return  result;
}

complex<double> WFunc(double num, double size)
{
	double tmp;
	//int pow = 1 << n;
	tmp = (M_PI * 2 * num) / size;
	complex<double> result(cos(tmp),-sin(tmp));
	return  result;
}

//один проход перестановки с прореживанием по времени
void transposition1d2(double** inputBuf, double** outputBuf, int size)
{
	int der = size/2;

	for(int i=0,j=0; i<size; i+=2,j++)
	{
		outputBuf[j] = inputBuf[i];
		outputBuf[der + j] = inputBuf[i + 1];
	}
	return;
}

//двоично-инверсная перестановка массива указателей
void transpositionBinInversion(double** inputBuf, int sizeLine)
{
	int sizeDev;
	double **tmp;
	double **tmpPointBuff1 = inputBuf;
	double **tmpPointBuff2 = new double*[sizeLine];
	double **tmpPointBuff2Tmp = tmpPointBuff2;

	for(sizeDev = sizeLine; sizeDev >= 2; sizeDev >>= 1)
	{
		for(int begin = 0; begin + sizeDev <= sizeLine; begin += sizeDev)
		{
			transposition1d2(tmpPointBuff1 + begin, tmpPointBuff2Tmp + begin, sizeDev);
		}
		tmp = tmpPointBuff1;
		tmpPointBuff1 = tmpPointBuff2Tmp;
		tmpPointBuff2Tmp = tmp;
	}

	delete[] tmpPointBuff2;
	return;
}

//один проход перестановки с прореживанием по времени
void transposition1d2C(complex<double> **inputBuf, complex<double> **outputBuf, int size)
{
	int der = size/2;

	for(int i=0,j=0; i<size; i+=2,j++)
	{
		outputBuf[j] = inputBuf[i];
		outputBuf[der + j] = inputBuf[i + 1];
	}
	return;
}

//двоично-инверсная перестановка массива указателей
void transpositionBinInversionC(complex<double> **inputBuf, int sizeLine)
{
	int sizeDev;
	complex<double> **tmp;
	complex<double> **tmpPointBuff1 = inputBuf;
	complex<double> **tmpPointBuff2 = new complex<double>*[sizeLine];
	complex<double> **tmpPointBuff2Tmp = tmpPointBuff2;

	for(sizeDev = sizeLine; sizeDev >= 2; sizeDev >>= 1)
	{
		for(int begin = 0; begin + sizeDev <= sizeLine; begin += sizeDev)
		{
			transposition1d2C(tmpPointBuff1 + begin, tmpPointBuff2Tmp + begin, sizeDev);
		}
		tmp = tmpPointBuff1;
		tmpPointBuff1 = tmpPointBuff2Tmp;
		tmpPointBuff2Tmp = tmp;
	}

	delete[] tmpPointBuff2;
	return;
}

int FFT2point(double *inputBuf, int sizeLine)
{
	double temp;
	for(int i = 0; i < sizeLine; i++)
	{
		temp = inputBuf[i];
		inputBuf[i] += inputBuf[i + 1];
		inputBuf[i + 1] = inputBuf[i] - temp;
	}
	return 1;
}

//FFT не оптимизированное
void FFTC_plain(complex<double> **inputBufFFT, int sizeLine)
{
	complex<double> *ptrBuf = NULL;
	complex<double> *tempBuf = new complex<double>[sizeLine];
	complex<double> *tempBuf2 = new complex<double>[sizeLine];

	//копирование входных данных
	for(int i = 0; i < sizeLine; i++)
	{
		tempBuf[i] = *inputBufFFT[i];
	}

	int numSubLine;
	// главный цикл БПФ
	for(int level = 2; level <= sizeLine; level = level << 1)
	{
		//строка
		for(int count = 0; count < sizeLine/level; count++)
		{
			// номер подстроки
			numSubLine = count * level;
			// объединение
			for(int i = 0; i < level/2; i++)
			{
				tempBuf2[i + numSubLine] = tempBuf[i + numSubLine] + (WFunc(i + numSubLine, level) * tempBuf[level/2 + i + numSubLine]);
				tempBuf2[i + numSubLine + level/2] = tempBuf[i + numSubLine] + (WFunc(i + numSubLine + level/2, level) * tempBuf[level/2 + i + numSubLine]);
			}
		}
		ptrBuf = tempBuf2;
		tempBuf2 = tempBuf;
		tempBuf = ptrBuf;
	}
	// копирование в выходной массив
	for(int i = 0; i < sizeLine; i++)
	{
        *inputBufFFT[i] = tempBuf[i];
	}
	delete[] tempBuf;
	delete[] tempBuf2;
}

int compute(wstring filename)
{
	string wstr;
	stringstream strStream;
	ifstream fileStream;
	fileStream.open(filename);

	int sizeLine = 0;

	fileStream >> sizeLine;

/*
	for(int i = 0; i < 4; i++)
	{
		WFunc(i, 4);
	}
return 1;
*/
	// чтение из файла и копирование адресов входного массива даных
	double tmpDig;
	for(int i = 0; i < sizeLine; i++)
	{
		fileStream >> tmpDig;
		inputBuffC[i].real(tmpDig);
		inputBuffC[i].imag(0.0);
		pointBuf1C[i] = &(inputBuffC[i]);
	}

	transpositionBinInversionC(pointBuf1C, sizeLine);

	FFTC_plain(pointBuf1C, sizeLine);

	ofstream fileStreamOut;
	fileStreamOut.open("fft_out.txt", std::ofstream::out | std::ofstream::trunc);

    fileStreamOut << "real \t\t\t image" << endl;
	//fileStreamOut.flags(std::ios::fixed);
	fileStreamOut << std::setprecision(16);
	
	for(int i = 0; i < sizeLine; i++)
	{
        fileStreamOut << pointBuf1C[i]->real() << "\t\t" << pointBuf1C[i]->imag() << endl;
	}

	fileStream.close();
	fileStreamOut.close();
	return 1;
}
