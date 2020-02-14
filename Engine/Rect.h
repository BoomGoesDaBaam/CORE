#pragma once

#include "Vec2.h"

template<typename T>
class Rect_
{
public:
	Rect_(T left_in, T right_in, T top_in, T bottom_in)
		:
		left(left_in),
		right(right_in),
		top(top_in),
		bottom(bottom_in)
	{}
	Rect_(const Vec2_<T>& topLeft, const Vec2_<T>& bottomRight)
		:
		Rect_(topLeft.x, bottomRight.x, topLeft.y, bottomRight.y)
	{}
	Rect_(const Vec2_<T>& topLeft, T width, T height)
		:
		Rect_(topLeft, topLeft + Vec2_<T>(width, height))
	{}
	template<typename S>
	explicit Rect_(const Rect_<S>& src)
		:
		left((T)src.left),
		top((T)src.top),
		bottom((T)src.bottom),
		right((T)src.right)
	{}
	template<typename T>
	bool operator==(const Rect_<T>& rhs)
	{
		return left == rhs.left && right == rhs.right && top == rhs.top && bottom == rhs.bottom;
	}
	template<typename T>
	bool operator!=(const Rect_<T>& rhs)
	{ 
		return !(*this == rhs);
	}
	bool IsOverlappingWith(const Rect_& other) const
	{
		return right > other.left&& left < other.right
			&& bottom > other.top&& top < other.bottom;
	}
	bool IsContainedBy(const Rect_& other) const
	{
		return left >= other.left && right <= other.right &&
			top >= other.top && bottom <= other.bottom;
	}
	bool Contains(const Vec2_<T>& point) const
	{
		return point.x >= left && point.x < right && point.y >= top && point.y < bottom;
	}
	Rect_ FromCenter(const Vec2_<T>& center, T halfWidth, T halfHeight)
	{
		const Vec2_<T> half(halfWidth, halfHeight);
		return Rect_(center - half, center + half);
	}
	Rect_ GetExpanded(T offset) const
	{
		return Rect_(left - offset, right + offset, top - offset, bottom + offset);
	}
	Vec2_<T> GetCenter() const
	{
		return Vec2_<T>((left + right) / (T)2, (top + bottom) / (T)2);
	}
	T GetWidth() const
	{
		return right - left;
	}
	T GetHeight() const
	{
		return bottom - top;
	}

	Rect_<T> operator+(const Vec2_<T>& rhs) const
	{
		return Rect_<T>(left + rhs.x,right + rhs.x ,top + rhs.y,bottom + rhs.y);
	}
public:
	T left;
	T right;
	T top;
	T bottom;
};

typedef Rect_<int> RectI;
typedef Rect_<float> RectF;