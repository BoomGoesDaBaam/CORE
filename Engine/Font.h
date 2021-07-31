#pragma once
#include "Surface.h"
//#include "Graphics.h"

class Font
{
	Surface surface;
	int nRaws = 0, nColums = 0, charWidth = 0, charHeight = 0;
	char first=0, last=0;
	bool costumWidth = false;
	std::vector<RectI> cRects;
	Color chroma;
	//Graphics& gfx;
public:
	Font(std::string filename, Color chroma, int nRaws, int nColoms, int charWidth, int charHeight, char first, char last);
	Font(std::string filename, Color chroma, int charHeight, char first, char last, Color delimiter, Color newLine);
	void DrawText(std::string text, int x, int y, int size, Color c);
	void DrawTextCentered(std::string text, Vei2 pos, int size, Color c);

	//
	int GetFirst()const;
	int GetLast()const;
	const Color& GetChroma()const { return chroma; }
	const std::vector<RectI>& GetCharRects()const;
	const Surface& GetSurface()const;
};