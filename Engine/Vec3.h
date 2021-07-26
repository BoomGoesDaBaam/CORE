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
	Vec3_ operator+(const Vec3_& rhs) const
	{
		return Vec3_(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	bool operator==(const Vec3_& rhs)const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	bool operator!=(const Vec3_& rhs)
	{
		return !(*this == rhs);
	}
	T z;
};

typedef Vec3_<float> Vec3;
typedef Vec3_<int> Vei3;

typedef Vec3_<Vei2> CctPos;
