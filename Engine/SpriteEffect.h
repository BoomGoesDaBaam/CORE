#pragma once
#include "Graphics.h"
namespace SpriteEffect
{
	class Nothing
	{
	public:
		Nothing() {}
		void operator()(int x, int y, Color sourceP, Graphics& gfx)
		{
			gfx.PutPixel(x, y, sourceP);
		}
	};

	class OneColor
	{
		Color c;
	public:
		OneColor(Color c):c(c) {}
		void operator()(int x, int y, Color whatEVER ,Graphics& gfx)
		{
			gfx.PutPixel(x, y, c);
		}
	};
	class Chroma
	{
	public:
		Chroma(Color chroma)
			:
			chroma(chroma)
		{}
		void operator()(int x, int y, Color sourceP, Graphics& gfx)
		{
			if (sourceP != chroma)
			{
				gfx.PutPixel(x, y, sourceP);
			}
		}
	private:
		Color chroma;
	};

	class ChromaColor // Chroma + Color
	{
	public:
		ChromaColor(Color chroma,Color c)
			:
			chroma(chroma),
			c(c)
		{}
		void operator()(int x, int y, Color sourceP, Graphics& gfx)
		{
			if (sourceP != chroma)
			{
				gfx.PutPixel(x, y, c);
			}
		}
	private:
		Color chroma;
		Color c;
	};
	class Rainbow
	{
	public:
		Rainbow()=default;
		void operator()(int x, int y, Color c, Graphics& gfx)
		{
			gfx.RainbowPutPixel(x, y);
		}
	};
};