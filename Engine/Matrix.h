#pragma once

template<typename T>
class Matrix
{
	T content[];
public:
	Matrix(T nRaws, T nColums)
	{
		content = { 3,3,3,3 };
	}
};