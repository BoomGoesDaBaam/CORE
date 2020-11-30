#pragma once
#include <vector>
#include <cassert>
#include "Rect.h"
template<typename T>
class Matrix
{
	class Column
	{
		std::vector<T> column;
	public:
		Column(int size, T value)
		{
			for (int i = 0; i < size;i++)
			{
				column.push_back(value);
			}
		}

		const T& operator[](std::size_t i)const {
			if (column.size() == 0)
			{
				return column[0];
			}
			return column[i % column.size()];
		}
		T& operator[](std::size_t i) { 
			if (column.size() == 0)
			{
				return column[0];
			}
			return column[i % column.size()]; 
		}
		template <typename T>
		Vec2_<T>& operator=(const Vec2_<T>& other)
		{
			assert(column.size() > 0);
			column[0].x = other.x;
			column[0].y = other.y;
			return column[0];
		}
		operator T& ()
		{
			assert(column.size() > 0);
			return column[0];
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
			column[i] = value;
		}
		bool HasValue(T value) const
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
		void Sort(int nonZero, int zeros)
		{
			for (int i = 0; i < column.size(); i++)
			{
				if (column[i] != 0)
				{
					column[i] = nonZero;
				}
				else
				{
					column[i] = zeros;
				}
			}
		}
		std::vector<T> GetPosOfValue(T value) const
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
	std::vector<Column> columns;
	int nRows = 1, nColumns = 1;
	bool leftOutRightIn = false;
public:
	Matrix(int nColums, int nRaws, T value):nRows(nRaws),nColumns(nColums)
	{
		assert(nRaws >= 1 && nColumns >= 1);
		for (int i = 0; i < nColums; i++)
		{
			columns.push_back(Column(nRaws,value));
		}
	}
	Matrix()
	{
		columns.push_back(Column(1, T()));
	}
	
	const T& operator()(Vei2 pos)const { return columns[pos.x][pos.y]; }
	T& operator()(Vei2 pos) { return columns[pos.x][pos.y]; }
	const Column& operator[](std::size_t idx) const { return columns[idx % columns.size()]; }
	Column& operator[](std::size_t idx) { return columns[idx % columns.size()]; }

	template <typename T>
	Matrix<T>& operator=(const Matrix<T>& other)
	{
		for (int x = 0; x < columns.size(); x++)
		{
			column[0] = other.column[0];
		}
		return *this;
	}

	void ReInit(int nColumns, int nRows, int value)
	{
		columns.clear();
		for (int y = 0; y < nColumns; y++)
		{
			columns.push_back(Column(nRows, value));
		}
	}
	void ReInit(Matrix<int> newM)
	{
		columns.clear();
		nColumns = newM.GetColums();
		nRows = newM.GetRows();

		for (int x = 0; x < newM.GetColums(); x++)
		{
			columns.push_back(Column(newM.GetRows(), 0));
			for (int y = 0; y < newM.GetRows(); y++)
			{
				columns[x].SetValue(y, newM[x][y]);
			}
		}
	}
	Vei2 GetSize()
	{
		return Vei2(nRows, nColumns);
	}
	bool IndexInBounds(Vei2 index)const
	{
		assert(index.x >= 0 && index.x < nColumns && index.y >= 0 && index.y < nRows);
		return index.x >= 0 && index.x < nColumns && index.y >= 0 && index.y < nRows;
	}
	int GetRows()const
	{
		return nRows;
	}
	int GetColums()const
	{
		return nColumns;
	}
	Vei2 GetSize()const
	{
		return Vei2(nColumns, nRows);
	}
	void SetValueOfALL(T value)
	{
		for (int i = 0; i < nColumns; i++)
		{
			columns[i].SetValueOfALL(value);
		}
	}
	void Sort(int nonZero,int zeros)			//puts in every nonZero element value of parameter
	{
		for (int i = 0; i < nColumns; i++)
		{
			columns[i].Sort(nonZero, zeros);
		}
	}
	void SetValueOfRaw(int raw, T value)
	{
		assert(raw >= 0 && raw < nRows);
		for (int i = 0; i < nColumns; i++)
		{
			columns[i].SetValue(raw, value);
		}
	}
	void SetValueOfColum(int column, T value)
	{
		assert(column >= 0 && column < nColumns);
		for (int i = 0; i < nRows; i++)
		{
			columns[column].SetValue(i, value);
		}
	}
	bool HasValue(T value) const
	{
		for (int x = 0; x < nRows; x++)
		{
			if (columns[x].HasValue(value))
			{
				return true;
			}
		}
		return false;
	}
	std::vector<Vec2_<T>> GetPosOfValue(T value) const
	{
		std::vector<Vec2_<T>> v;
		for (int x = 0; x < nRows; x++)
		{
			if (columns[x].HasValue(value))
			{
				std::vector<T> pos = this->columns[x].GetPosOfValue(value);
				for (int i = 0; i < pos.size(); i++)
				{
					v.push_back(Vec2_<T>(pos[i], x));
				}
			}
		}
		return v;
	}
	
	void HalfSize(Matrix<int> oldM, int valOfMixed)	
	{
		Matrix<int> newM = Matrix<int>(oldM.GetColums() / 2, oldM.GetRows() / 2, 0);
		for (int y = 0; y < newM.GetRows(); y++)
		{
			for (int x = 0; x < newM.GetColums(); x++)
			{
				bool hasMixed = false; 
				int last = oldM[(__int64)x * 2][(__int64)y * 2];

				for (int yInner = 0; yInner < 2; yInner++)
				{
					for (int xInner = 0; xInner < 2; xInner++)
					{
						if (oldM[(__int64)x * 2 + xInner][(__int64)y * 2 + yInner] != last)
						{
							hasMixed = true;
						}
					}
				}
				if (hasMixed)
				{
					newM[x][y] = valOfMixed;
				}
				else
				{
					newM[x][y] = oldM[(__int64)x * 2][(__int64)y * 2];
				}
			}
		}
		ReInit(newM);
	}
	void MirrowVertical()
	{
		Matrix<int> oldM = Matrix<int>(*this);
		for (int x = 0; x < nColumns; x++)
		{
			for (int y = 0; y < nRows; y++)
			{
				columns[x].SetValue(y, oldM[x][(__int64)nRows - y - 1]);
			}
		}
	}
	Matrix<T> GetMatPlusZeroOutline() const
	{
		Matrix<T> newM(nColumns + 2, nRows + 2, 0);
		for (int x = 0; x < nColumns; x++)
		{
			for (int y = 0; y < nRows; y++)
			{
				newM.columns[(__int64)x+1].SetValue(y+1, columns[x][y]);
			}
		}
		return newM;
	}
	bool InBoundsY(int y) const
	{
		return y >= 0 && y < nRows;
	}
	bool InBoundsX(int x) const
	{
		return x >= 0 && x < nColumns;
	}
	//Matrix<int> GetAroundMatrix(Vei2 cell)const;	//in bounds: type		outside bounds(y-wise): -1		
	Matrix<int> GetAroundMatrix(Vei2 pos)const	
	{
		Matrix<int> m = Matrix<int>(3, 3, 0);

		for (int y = 0; y < 3; y++)
		{
			for (int x = 0; x < 3; x++)
			{
				Vei2 curP(x + pos.x - 1, y + pos.y - 1);

				if (InBoundsY(curP.y) && InBoundsX(curP.x))
				{
					m[x][y] = (int)T(columns[curP.x][curP.y]);
				}
				else
				{
					m[x][y] = -1;
				}
			}
		}
		return m;
	}
	void CombineWith(Matrix<int> rhs)
	{
		assert(rhs.GetColums() == GetColums() && rhs.GetRows() == GetRows());
		for (int y = 0; y < GetRows(); y++)
		{
			for (int x = 0; x < GetColums(); x++)
			{
				columns[x][y] = columns[x][y] || rhs[x][y];
			}
		}
	}
};
typedef Matrix<Vei2> VecN;
//for exemple a vector with 7 elements:			VecN vec = VecN(7, 1, defValue);
//Can be accessed via:							Type t = vec[0];
//changed via:									m[0] = 4;