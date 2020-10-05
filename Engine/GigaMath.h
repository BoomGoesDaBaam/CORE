#pragma once
#include <iostream>
#include <cmath>
#include "RandyRandom.h"
namespace GigaMath
{
	template<typename T>
	T const pi = std::acos(-T(1));

	/*
	double GetRandomRadiantt()
	{
		RandyRandom rng;
		return rng.Calc(360) * 0.0174533f;
	}
	*/

	template <typename T>
	std::pair<T, T> RotPointToOrigin(T x, T y, double radiant)
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
	template <typename T>
	T GetDist(Vec2_<T> p0, Vec2_<T> p1)
	{
		if (p1.x - p0.x != 0)
		{
			return (p1.y - p0.y) / (p1.x - p0.x);
		}
		return p1.y - p0.y;
	}
	template <typename T>
	T GetDist(T x0, T y0, T x1, T y1)
	{
		if (x1 - x0 != 0)
		{
			return std::sqrt(std::pow((float)(x0 - x1), 2.0f) + std::pow((float)(y0 - y1), 2.0f));
		}
		return y1 - y0;
	}
	static float GetRandomNormDistribution()
	{
		std::random_device rd{};
		std::mt19937 gen{ rd() };

		std::normal_distribution<> d{ 0,0.6f };

		float dist = d(gen);
		while (!(dist >= 0 && dist <= 1))
		{
			dist = d(gen);
		}
		return dist;
	}
}
namespace GiMa = GigaMath;