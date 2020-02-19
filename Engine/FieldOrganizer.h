#pragma once
#include "Surface.h"
#include "Matrix.h"
#include <cassert>
class FieldOrganizer
{
	Surface& s;
	Vec2 size;		//		width/height of a picture from the Spritesheet
	FieldOrganizer(Surface& s,Vei2 size) :s(s),size(size) {}
	
	
	int check(Matrix<int> sur)
	{
		assert(sur.GetColums() == 3 && sur.GetRaws() == 3);
		if (sur[0][0] == 0 && sur[2][1] == 1 && sur[0][1] == 0 && sur[1][2] == 1 && sur[2][2] == 1)
		{
			return 1;
		}
	}
};