#pragma once
#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "RandyRandom.h"
#include "Rect.h"
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
		float s = (float)sin(radiant);
		float c = (float)cos(radiant);

		float xnew = (float)p.first * c - (float)p.second * s;
		float ynew = (float)p.first * s + (float)p.second * c;

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
	static float GetRandomNormDistribution()		//returns a value between 0 and 1. More likely to be 0
	{
		std::random_device rd{};
		std::mt19937 gen{ rd() };

		std::normal_distribution<> d{ 0,0.6f };

		float dist = (float)d(gen);
		while (!(dist >= 0 && dist <= 1))
		{
			dist = (float)d(gen);
		}
		return dist;
	}
	static int NegMod(int x, int divisor)			//divisor has to be positiv
	{
		assert(divisor > 0);
		return ((x % divisor) + divisor) % divisor;
	}
	template <typename T>
	static Vec2_<T> NegMod(Vec2_<T> x, int divisor)			//divisor has to be positiv
	{
		return Vec2_<T>(NegMod(x.x, divisor), NegMod(x.y, divisor));
	}
	static std::string float2StringWithPrecision(int precision, float number)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(precision) << number;
		return stream.str();
	}
	
	template <typename T>
	Vec2_<T> GetRandomPointOnUnitCircle()
	{
		Vec2_<double> point = Vec2_<double>(1, 0);
		RandyRandom rr;
		float rad = (float)((double)rr.Calc(360) * 0.017453292519943);
		auto a = RotPointToOrigin(point.x, point.y, rad);
		return Vec2_<T>((T)a.first, (T)a.second);
	}
	
}
namespace GiMa = GigaMath;