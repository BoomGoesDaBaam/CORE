#pragma once
#include <iostream>
namespace GigaMath
{
	template <typename T>
	std::pair<T,T> RotPointToOrigin(T x, T y, float radiant)
	{
		std::pair<T, T> p = { x,y };
		float s = sin(radiant);
		float c = cos(radiant);

		float xnew = p.first * c - p.second * s;
		float ynew = p.first * s + p.second * c;

		p.first = xnew;
		p.second = ynew;
		return p;
	}
}
