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
};