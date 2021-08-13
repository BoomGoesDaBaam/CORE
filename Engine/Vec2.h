#pragma once

#include <cmath>
#include <iostream>
template<typename T>
class Vec2_
{
public:
	Vec2_() = default;
	Vec2_(T x_in, T y_in)
		:
		x(x_in),
		y(y_in)
	{}
	//Conversions
	template<typename S>
	explicit Vec2_(const Vec2_<S>& src)
		:
		x((T)src.x),
		y((T)src.y)
	{}
	
	template<typename S>
	explicit Vec2_(const std::pair<S,S>& src)
		:
		x((T)src.first),
		y((T)src.second)
	{}
	
	//Operator
	Vec2_ operator+(const Vec2_& rhs) const
	{
		return Vec2_(x + rhs.x, y + rhs.y);
	}
	Vec2_& operator+=(const Vec2_& rhs)
	{
		return *this = *this + rhs;
	}

	Vec2_ operator*(const Vec2_ rhs) const
	{
		return Vec2_(x * rhs.x, y * rhs.y);
	}
	Vec2_& operator*=(const Vec2_ rhs)
	{
		return *this = *this * rhs;
	}

	Vec2_ operator*(T rhs) const
	{
		return Vec2_(x * rhs, y * rhs);
	}
	Vec2_& operator*=(T rhs)
	{
		return *this = *this * rhs;
	}
	Vec2_ operator-(const Vec2_& rhs) const
	{
		return Vec2_(x - rhs.x, y - rhs.y);
	}
	Vec2_& operator-=(const Vec2_& rhs)
	{
		return *this = *this - rhs;
	}

	Vec2_ operator/(int rhs) const
	{
		return Vec2_(x / rhs, y / rhs);
	}
	Vec2_ operator/(Vec2_ rhs) const
	{
		return Vec2_(x / rhs.x, y / rhs.y);
	}
	Vec2_ operator%(int rhs) const
	{
		return Vec2_(x % rhs, y % rhs);
	}
	Vec2_& operator/=(int rhs)
	{
		return *this = *this / rhs;
	}


	bool operator==(const Vec2_& rhs)const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Vec2_& rhs)
	{
		return !(*this==rhs);
	}

	
	T GetLength() const
	{
		return (T)std::sqrt(GetLengthSq());
	}
	T GetLengthSq() const
	{
		return x * x + y * y;
	}
	Vec2_ GetAbsVec()
	{
		return Vec2_<T>(std::abs(x), std::abs(y));
	}
	Vec2_& Normalize()
	{
		return *this = GetNormalized();
	}
	Vec2_ GetNormalized() const
	{
		const T len = GetLength();
		if (len != (T)0)
		{
			return *this * ((T)1 / len);
		}
		return *this;
	}
	template <typename T>
	static bool IsPositivFactor(Vec2_<T> v)
	{
		return v.x >= 0 && v.x <= 1.0f && v.y >= 0 && v.y <= 1;
	}
public:
	T x;
	T y;
};
template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec2_<T>& obj)
{
	os << "(" << obj.x << "/" << obj.y << ")";
	return os;
}

typedef Vec2_<float> Vec2;
typedef Vec2_<int> Vei2;
typedef Vec2_<Vei2> CtPos;
