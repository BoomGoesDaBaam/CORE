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

	std::vector<std::string> SplitTextToLines(std::string text, int size, int width)
	{
		std::vector<std::string> lines;
		int wordWidthInChar = 0;
		float wordWidthInPixel = 0;
		int charStart = 0;
		lines.push_back("");
		for (int i = 0; i < (int)text.length(); i++)
		{
			if (text[i] >= first && text[i] <= last)
			{
				wordWidthInChar++;
				float ratio = (float)cRects[(__int64)text[i] - first].GetWidth() / cRects[(__int64)text[i] - first].GetHeight();
				Vec2 charSize(ratio * size, (float)size);
				if (wordWidthInPixel + ((float)size * 0.9f * ratio) >= width)
				{
					lines.push_back("");
					wordWidthInPixel = 0;
				}
				wordWidthInPixel += ((float)size * 0.9f * ratio);
			}
			if (text[i] == ' ')
			{
				wordWidthInChar++;
				lines[(int)lines.size() - 1] += text.substr(charStart, wordWidthInChar);
				charStart += wordWidthInChar;
				wordWidthInChar = 0;
				if (wordWidthInPixel + (size * 0.7f) >= width)
				{
					lines.push_back("");
					wordWidthInPixel = 0;
				}
				wordWidthInPixel += ((float)(size * 0.7f));
			}
		}
		if (wordWidthInChar > 0)
		{
			lines[(int)lines.size() - 1] += text.substr(charStart, wordWidthInChar);
		}
		return lines;
	}
};