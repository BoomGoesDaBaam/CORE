/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	go(gfx),
	gd(gfx)
{
	//mat.SetValueOfALL(true);
	//go.AddTileframe(Vec2(50.0f, 50.0f), mat, 0);
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	
}

void Game::ComposeFrame()
{
	gfx.DrawSurface(RectI(Vei2(50,50), 100, 100), RectI(Vei2(0, 0), 50, 50), s, SpriteEffect::Chroma(Colors::Magenta),t.GetPassedTime());


	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			gfx.DrawSurface(RectI(Vei2(200+100*x, 200 + 100 * y), 100, 100), RectI(Vei2(0, 0), 50, 50), s, SpriteEffect::Chroma(Colors::Magenta));
		}
	}
	go.Draw();
}

