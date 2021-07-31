#pragma once
#include "Font.h"
#include "SpriteEffect.h"
Font::Font(std::string filename,Color chroma, int nRaws, int nColums, int charWidth, int charHeight,char first, char last):
	surface(filename),
	nRaws(nRaws),nColums(nColums), charWidth(charWidth),charHeight(charHeight), first(first),last(last),
	chroma(chroma)
{
	for (int y = 0; y < nRaws; y++)
	{
		for (int x = 0; x < nColums; x++)
		{
			cRects.push_back(RectI(Vei2(x * charWidth, y * charHeight), charWidth, charHeight));
			if (last - first + 1 == cRects.size())
			{
				return;
			}
		}
	}
}//94
Font::Font(std::string filename, Color chroma, int charHeight, char first, char last, Color delimiter, Color newLine) :
	surface(filename),
	charHeight(charHeight), first(first), last(last),
	chroma(chroma)
{
	Vei2 curPos = Vei2(0, 0);
	int curWidth = 0;

	for (int i = 0; i < last - first + 1; i++)
	{
		curWidth = 0;

		for (int x = curPos.x; surface.GetPixel(x,curPos.y) != delimiter && surface.GetPixel(x, curPos.y) != newLine; x++)
		{
			curWidth++;
		}
		cRects.push_back(RectI(curPos, curWidth, charHeight));

		if (surface.GetPixel(curPos.x + curWidth, curPos.y) == delimiter)
		{
			curPos.x += curWidth + 1;
		}
		else if (surface.GetPixel(curPos.x + curWidth, curPos.y) == newLine)
		{
			curPos = Vei2(0, curPos.y + charHeight+1);
		}
	}
	costumWidth = true;
}
void Font::DrawText(std::string text, int x, int y, int size, Color c)
{
	int xM = 0;
	int yM = 0;
	if (costumWidth)
	{
		for (int i = 0; i < (int)text.length(); i++)
		{
			if (text[i] >= first && text[i] <= last)
			{
				float ratio = (float)cRects[(__int64)text[i] - first].GetWidth() / cRects[(__int64)text[i] - first].GetHeight();
				Vec2 charSize(ratio * size, (float)size);
				//gfx.DrawSurface(RectI(Vei2(x + xM, y), (int)charSize.x, (int)charSize.y), cRects[(__int64)text[i] - first], s, SpriteEffect::ChromaColor(Colors::Magenta, c));
				xM += (int)((float)size * 0.9f * ratio);
			}
			if (text[i] == ' ')
			{
				xM += (int)(size * 0.7f);
			}
		}
	}
	else
	{
		float radio = 9.f / 13.f;
		for (int i = 0; i < (int)text.length(); i++)
		{
			if (text[i] >= first && text[i] <= last)
			{
				//gfx.DrawSurface(RectI(Vei2(x + xM, y), (int)(size * radio), size), cRects[(__int64)text[i] - first], s, SpriteEffect::ChromaColor(Colors::Magenta, c));
				xM += (int)(size * radio);
			}
			if (text[i] == ' ')
			{
				xM += (int)(size * radio);
			}
		}
	}
}
void Font::DrawTextCentered(std::string text, Vei2 pos, int size, Color c)
{
	int x = pos.x;
	int y = pos.y;
	int xM = 0;
	int yM = 0;
	if (costumWidth)
	{
		int width = 0;
		for (int i = 0; i < (int)(text.length()); i++)
		{
			if (text[i] >= first && text[i] <= last)
			{
				float ratio = (float)cRects[(__int64)text[i] - first].GetWidth() / cRects[(__int64)text[i] - first].GetHeight();
				width += (__int64)(size * 0.9f * ratio);
			}
			if (text[i] == ' ')
			{
				width += (int)(size * 0.7f);
			}
		}
		x -= width / 2;
		y -= size / 2;
		for (int i = 0; i < (int)text.length(); i++)
		{
			if (text[i] >= first && text[i] <= last)
			{
				float ratio = (float)cRects[(__int64)text[i] - first].GetWidth() / cRects[(__int64)text[i] - first].GetHeight();
				Vec2 charSize(ratio * size, (float)size);
				//gfx.DrawSurface(RectI(Vei2(x + xM, y), (int)charSize.x, (int)charSize.y), cRects[(__int64)text[i] - first], surface, SpriteEffect::ChromaColor(Colors::Magenta, c));
				xM += (__int64)(size * 0.9f * ratio);
			}
			if (text[i] == ' ')
			{
				xM += (int)(size * 0.7f);
			}
		}
	}
	/*
	else
	{
		float radio = 9.f / 13.f;
		for (int i = 0; i < text.length(); i++)
		{
			if (text[i] >= first && text[i] <= last)
			{
				gfx.DrawSurface(RectI(Vei2(x + xM, y), size * radio, size), cRects[(__int64)text[i] - first], Graphics::GetScreenRect<int>(), s, SpriteEffect::ChromaColor(Colors::Magenta, c));
				xM += size * radio;
			}
			if (text[i] == ' ')
			{
				xM += size * radio;
			}
		}
	}
	*/
}
int Font::GetFirst()const { return first; }
int Font::GetLast()const { return last; }
const std::vector<RectI>& Font::GetCharRects()const
{
	return cRects;
}
const Surface& Font::GetSurface()const
{
	return surface;
}
