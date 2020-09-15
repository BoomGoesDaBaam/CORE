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
	class Transparent
	{
		float alpha=0.5f;
		Color chroma = Colors::OtherMagenta;
	public:
		Transparent(Color chroma,float alpha = 0.5f):chroma(chroma),alpha(alpha) {}
		Transparent(float alpha = 0.5f) :alpha(alpha) {}

		void operator()(int x, int y, Color c, Graphics& gfx)
		{
			if(c != chroma)
			{
				Color old = gfx.GetPixel(x, y) * (1 - alpha);
				c *= alpha;
				Color m = Color(old.GetR() + c.GetR(), old.GetG() + c.GetG(), old.GetB() + c.GetB());
				gfx.PutPixel(x, y, m);
			}
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