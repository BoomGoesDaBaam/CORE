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
		//if(sur[1][0] == 0 && )
	}
};