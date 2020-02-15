#pragma once
#include "Graphics.h"
namespace SpriteEffect
{
	class Chroma
	{
	public:
		Chroma(Color c)
			:
			chroma(c) 
		{}
		void operator()(int x, int y, Color c, Graphics& gfx)
		{
			if (c != chroma)
			{
				gfx.PutPixel(x, y, Colors::Red);
			}
		}
	private:
		Color chroma;
	};
	class Nothing
	{
	public:
		Nothing() {}
		void operator()(int x, int y, Color c, Graphics& gfx)
		{
			gfx.PutPixel(x, y, c);
		}
	};
	class ChromaColor
	{
	public:
		ChromaColor(Color chroma)
			:
			chroma(chroma)
		{}
		void operator()(int x, int y, Color testForChroma, Color c, Graphics& gfx)
		{
			if (testForChroma != chroma)
			{
				gfx.PutPixel(x, y, c);
			}
		}
	private:
		Color chroma;
	};
};