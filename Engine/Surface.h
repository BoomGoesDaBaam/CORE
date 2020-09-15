#pragma once

#include "Colors.h"
#include <string>
#include <vector>
#include "Rect.h"
#include "Matrix.h"
class Surface
{
public:
	Surface(const std::string& filename);
	Surface(int width, int height);
	Surface(Surface&& donor);
	Surface(const Surface&) = default;
	Surface& operator=(Surface&& rhs);
	Surface& operator=(const Surface&) = default;
	Surface() = default;

	Surface GetSupSurface(RectI where);
	void PutPixel(int x, int y, Color c);
	Color GetPixel(Vei2 pos) const;
	Color GetPixel(int x, int y) const;
	int GetWidth() const;
	int GetHeight() const;
	RectI GetRect() const;
	Vei2 GetSize() const;
	void Fill(Color c);
	const Color* Data() const;
	bool IsInBounds(int x, int y)const;
	Matrix<int> GetChromaMatrix(Color chroma)const;		// '0' == chroma		'1' == nonchroma
	bool TestIfHitOnScreen(Vec2 mP)const;
private:
	std::vector<Color> pixels;
	int width = 0;
	int height = 0;
};