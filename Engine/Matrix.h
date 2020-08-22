#pragma once
#include <vector>
#include <cassert>
template<typename T>
class Matrix
{
	class Cloumn
	{
		std::vector<T> column;
	public:
		Cloumn(int size, T value)
		{
			for (int i = 0; i < size;i++)
			{
				column.push_back(value);
			}
		}

		const T& operator[](std::size_t i)const {
			assert(i >= 0 && i < column.size());
			return column[i];
		}
		T& operator[](std::size_t i) { 
			assert(i >= 0 && i < column.size());
			return column[i]; }


		void SetValueOfALL(T value)
		{
			for (int i = 0; i < raw.size(); i++)
			{
				raw[i] = value;
			}
		}
		void SetValue(int i, T value)
		{
			column[i] = value;
		}
		bool HasValue(T value)
		{
			for (int x = 0; x < column.size(); x++)
			{
				if (column[x] == value)
				{
					return true;
				}
			}
			return false;
		}
		std::vector<T> GetPosOfValue(T value)
		{
			std::vector<T> pos;
			for (int x = 0; x < column.size(); x++)
			{
				if (column[x] == value)
				{
					pos.push_back(x);
				}
			}
			return pos;
		}
	};
	std::vector<Cloumn> columns;
	int nRaws = 1, nColumns = 1;
public:
	Matrix(int nColums, int nRaws, T value):nRaws(nRaws),nColumns(nColums)
	{
		assert(nRaws >= 1 && nColumns >= 1);
		for (int i = 0; i < nRaws; i++)
		{
			columns.push_back(Cloumn(nRaws,value));
		}
	}
	Matrix()
	{
		columns.push_back(Cloumn(1, 0));
	}

	const T& operator()(Vei2 pos)const { return columns[pos.x][pos.y]; }
	T& operator()(Vei2 pos) { return columns[pos.x][pos.y]; }
	const Cloumn& operator[](std::size_t idx) const { return columns[idx]; }
	Cloumn& operator[](std::size_t idx) { return columns[idx]; }


	Vei2 GetSize()
	{
		return Vei2(nRaws, nColumns);
	}
	int GetRaws()const
	{
		return nRaws;
	}
	int GetColums()const
	{
		return nColumns;
	}
	void SetValueOfALL(T value)
	{
		for (int i = 0; i < nColumns; i++)
		{
			colums[i].SetValueOfALL(value);
		}
	}
	void SetValueOfRaw(int raw, T value)
	{
		assert(raw >= 0 && raw < nRaws);
		for (int i = 0; i < nColumns; i++)
		{
			colums[i].SetValue(raw, value);
		}
	}
	void SetValueOfColum(int column, T value)
	{
		assert(column >= 0 && column < nColumns);
		for (int i = 0; i < nRaws; i++)
		{
			colums[column].SetValue(i, value);
		}
	}
	bool HasValue(T value)
	{
		for (int x = 0; x < nRaws; x++)
		{
			if (columns[x].HasValue(value))
			{
				return true;
			}
		}
		return false;
	}
	std::vector<Vec2_<T>> GetPosOfValue(T value)
	{
		std::vector<Vec2_<T>> v;
		for (int x = 0; x < nRaws; x++)
		{
			if (columns[x].HasValue(value))
			{
				std::vector<T> pos = columns[x].GetPosOfValue(value);
				for (int i = 0; i < pos.size(); i++)
				{
					v.push_back(Vec2_<T>(pos[i], x));
				}
			}
		}
		return v;
	}
	Matrix<T> Get3x3Surrounded(int x, int y, T notValue)
	{	
		assert(x >= 0 && x < nColumns && y >= 0 && y < nRaws);
		Matrix<T> newM = Matrix<T>(3,3,notValue);
		newM[1][1] = columns[x][y];

		if (x > 0 && y > 0)
		{
			newM[0][0] = columns[x-1][y-1];
		}
		if (nColums - 1 > x && y > 0)
		{
			newM[2][0] = columns[x + 1][y - 1];
		}
		if (x > 0 && nRaws - 1 > y)
		{
			newM[0][2] = columns[x - 1][y + 1];
		}
		if (nColums - 1 > x && nRaws - 1 > y)
		{
			newM[2][2] = columns[x + 1][y + 1];
		}

		if (x > 0)
		{
			newM[0][1] = columns[x - 1][y];
		}
		if (y > 0)
		{
			newM[1][0] = columns[x][y - 1];
		}
		if (nColums - 1 > x)
		{
			newM[2][1] = columns[x + 1][y];
		}
		if (nRaws - 1 > y)
		{
			newM[1][2] = columns[x][y + 1];
		}
		return newM;
	}
};