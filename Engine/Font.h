#pragma once
#include "Surface.h"
#include "Graphics.h"
class Font
{
	Surface s;
	int nRaws = 0, nColums = 0, charWidth = 0, charHeight = 0;
	char first=0, last=0;
	bool costumWidth = false;
	std::vector<RectI> cRects;
	Graphics& gfx;
public:
	Font(std::string filename, int nRaws, int nColoms, int charWidth, int charHeight, char first, char last,Graphics& gfx);
	Font(std::string filename, int charHeight, char first, char last, Color delimiter, Color newLine, Graphics& gfx);
	void DrawText(std::string text, int x, int y, int size, Color c);
	void Font::DrawTextCentered(std::string text, Vei2 pos, int size, Color c);

};