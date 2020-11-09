#pragma once
#include "Vec2.h"
template<typename T>
class Vec3_ : public Vec2_<T>
{
public:
	Vec3_() = default;
	Vec3_(T x_in, T y_in, T z_in)
		:
		Vec2_<T>(x_in, y_in),
		z(z_in)
	{}
	//Conversions
	template<typename S>
	explicit Vec3_(const Vec3_<S>& src)
		:
		x((T)src.x),
		y((T)src.y),
		z((T)src.z)
	{}

	T z;
};
