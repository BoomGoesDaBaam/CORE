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
	void AddLayer(RectI pos, RectI sourceR, const Surface& s, int n90rot = 0)
	{
		n90rot %= 4;
		for (int y = 0; y < pos.GetHeight(); y++)
		{
			for (int x = 0; x < pos.GetWidth(); x++)
			{
				if (RectI(Vei2(0,0),width,height).Contains({ x + pos.left, y + pos.top }))
				{
					int sPixelX = 0;
					int sPixelY = 0;

					if (n90rot == 0)
					{
						sPixelX = (int)(sourceR.left + ((float)(x) / pos.GetWidth()) * sourceR.GetWidth());
						sPixelY = (int)(sourceR.top + ((float)(y) / pos.GetHeight()) * sourceR.GetHeight());
					}
					if (n90rot == 1)
					{
						sPixelX = (int)(sourceR.right - 1 - ((float)(y) / pos.GetWidth()) * sourceR.GetWidth());
						sPixelY = (int)(sourceR.top + ((float)(x) / pos.GetHeight()) * sourceR.GetHeight());
					}
					if (n90rot == 2)
					{
						sPixelX = (int)(sourceR.right - 1 - ((float)(x) / pos.GetWidth()) * sourceR.GetWidth());
						sPixelY = (int)(sourceR.bottom - 1 - ((float)(y) / pos.GetHeight()) * sourceR.GetHeight());
					}
					if (n90rot == 3)
					{
						sPixelX = (int)(sourceR.left + ((float)(x) / pos.GetWidth()) * sourceR.GetWidth());
						sPixelY = (int)(sourceR.bottom - 1 - ((float)(y) / pos.GetHeight()) * sourceR.GetHeight());
					}
					assert(sPixelX >= 0);
					assert(sPixelY >= 0);
					Color sourceP = s.GetPixel(sPixelX, sPixelY);
					if(sourceP != Colors::Magenta)
					pixels[x + pos.left + (y + pos.top) * width] = sourceP;
					//effect(x + pos.left, y + pos.top, sourceP, *this);
				}
			}
		}
	}
	void AddRect(RectI pos, Color& c)
	{
		for (int y = 0; y < pos.GetHeight(); y++)
		{
			for (int x = 0; x < pos.GetWidth(); x++)
			{
				if (RectI(Vei2(0, 0), width, height).Contains({ x + pos.left, y + pos.top }))
				{
					pixels[x + pos.left + (y + pos.top) * width] = c;
				}
			}
		}
	}
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