#pragma once
#include "Font.h"
#include "SpriteEffect.h"
Font::Font(std::string filename, int nRaws, int nColums, int charWidth, int charHeight,char first, char last,Graphics& gfx):
	s(filename),
	nRaws(nRaws),nColums(nColums), charWidth(charWidth),charHeight(charHeight), first(first),last(last),
	gfx(gfx)
{
	for (int y = 0; y < nRaws; y++)
	{
		for (int x = 0; x < nColums; x++)
		{
			cRects.push_back(RectI(Vei2(x * charWidth, y * charHeight), charWidth, charHeight));
		}
	}
}

const RectI& Font::GetRectOfChar(char c)
{
	if (c >= first && c <= last)
	{
		return cRects[c - first];
	}
	return RectI({ -1,-1 }, -1, -1);
}
void Font::DrawText(std::string text, int x, int y)
{
	int xM = 0;
	int yM = 0;
	for (int i = 0; i < text.length(); i++)
	{
		if (text[i] >= first && text[i] <= last)
		{
			gfx.DrawSurface(Vei2(x+xM,y),cRects[text[i] - first],Graphics::GetScreenRect<int>(),s, Colors::Red,SpriteEffect::Chroma(Colors::Magenta));
			xM += charWidth;
		}
		if(text[i] == ' ')
		{
			xM += charWidth;
		}
	}
}
