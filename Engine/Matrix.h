#pragma once
#include <vector>
template<typename T>
class Matrix
{
	class Cloum
	{
		std::vector<T> colum;
	public:
		Cloum(int size, T value)
		{
			for (int i = 0; i < size;i++)
			{
				colum.push_back(value);
			}
		}
		T& operator[](std::size_t i)
		{
			return colum[i];
		}
		void SetValueOfALL(T value)
		{
			for (int i = 0; i < raw.size(); i++)
			{
				raw[i] = value;
			}
		}
		void SetValue(int i, T value)
		{
			raw[i] = T;
		}
	};
	std::vector<Cloum> colums;
	int nRaws, nColums;
public:
	Matrix(int nRaws, int nColums, T value):nRaws(nRaws),nColums(nColums)
	{
		assert(nRaws >= 1 && nColums >= 1);
		for (int i = 0; i < nRaws; i++)
		{
			colums.push_back(Cloum(nRaws,value));
		}
	}
	int GetRaws()const
	{
		return nRaws;
	}
	int GetColums()const
	{
		return nColums;
	}
	Cloum& operator[](std::size_t i)
	{
		return colums[i];
	}
	void SetValueOfALL(T value)
	{
		for (int i = 0; i < nColums; i++)
		{
			colums[i].SetValueOfALL(value);
		}
	}
	void SetValueOfRaw(int raw, T Value)
	{
		assert(raw >= 0 && raw < nRaws)
		for (int i = 0; i < nColums; i++)
		{
			colums[i].SetValue(raw, value);
		}
	}
	void SetValueOfColum(int colum, T Value)
	{
		assert(colum >= 0 && colum < nColums)
		for (int i = 0; i < nRaws; i++)
		{
			colums[colum].SetValue(i, value);
		}
	}
	Matrix<T> Get3x3Surrounded(int x, int y, T notValue)
	{	
		assert(x >= 0 && x < nColums && y >= 0 && y < nRaws);
		Matrix<T> newM = Matrix<T>(3,3,notValue);
		newM[1][1] = colums[x][y];

		if (x > 0 && y > 0)
		{
			newM[0][0] = colums[x-1][y-1];
		}
		if (nColums - 1 > x && y > 0)
		{
			newM[2][0] = colums[x + 1][y - 1];
		}
		if (x > 0 && nRaws - 1 > y)
		{
			newM[0][2] = colums[x - 1][y + 1];
		}
		if (nColums - 1 > x && nRaws - 1 > y)
		{
			newM[2][2] = colums[x + 1][y + 1];
		}

		if (x > 0)
		{
			newM[0][1] = colums[x - 1][y];
		}
		if (y > 0)
		{
			newM[1][0] = colums[x][y - 1];
		}
		if (nColums - 1 > x)
		{
			newM[2][1] = colums[x + 1][y];
		}
		if (nRaws - 1 > y)
		{
			newM[1][2] = colums[x][y + 1];
		}
		return newM;
	}
};