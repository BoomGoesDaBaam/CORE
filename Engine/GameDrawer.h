#pragma once
#include "Graphics.h"
#include "World.h"
#include "SpriteEffect.h"
class GameDrawer
{
	Surface s = { "Textures/green.bmp" };
	Graphics& gfx;
public:
	GameDrawer(Graphics& gfx) :gfx(gfx) {}
	void Draw(World* cWorld)
	{
		
		Vei2 wSize = cWorld->GetwSize();
		gfx.DrawSurface(RectI(Vei2(50,50),500,500), RectI(Vei2(0, 0), 50, 50), RectI(Vei2(0, 0), 50, 50), s, SpriteEffect::Chroma(Colors::Magenta));

		for (int y = 0; y < wSize.y; y++)
		{
			for (int x = 0; x < wSize.y; x++)
			{
				gfx.DrawSurface((RectI)(cWorld->GetCellRect(Vei2( x,y ))), RectI(Vei2( 0,0 ),50,50), RectI(Vei2( 0,0 ),50,50), s, SpriteEffect::Nothing());
			}
		}
	}
};

