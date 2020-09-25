#pragma once
#include "Matrix.h"
#include "Rect.h"
class Obstaclee
{
	Vei2 tilePos;				//pos is bottomleft
	Matrix<int> size;
public:
	Obstaclee(Vei2 tilePos, Matrix<int> size)
		:
		tilePos(tilePos),
		size(size)
	{

	}
};
