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
	go(gfx)
	, mat2(mat.Get3x3Surrounded(1, 1, 0))
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
	go.Update(0.1f);
	
//	go.AddVolcano((Vec2)wnd.mouse.GetPos(), 50*rr.GetFaktor(), 50);
//	go.AddShot((Vec2)wnd.mouse.GetPos(),Vec2(500,50),50.0f);

	Vei2 mP = (Vei2)wnd.mouse.GetPos();

	for (int y = 0; y < mat2.GetRaws(); y++)
	{
		for (int x = 0; x < mat2.GetColums(); x++)
		{
			if (mat2[x][y] == 1)
			{
				go.AddVolcano(Vec2((float)mP.x + x * 50, (float)mP.y + y * 50), 20, 10);
			}
			else
			{
				go.AddVolcano(Vec2((float)mP.x + x * 50, (float)mP.y + y * 50), 1, 10);
			}
		}
	}
}

void Game::ComposeFrame()
{
	go.Draw();
	//gfx.DrawSurfaceQuick(RectI(Vei2(wnd.mouse.GetPosX()-20,wnd.mouse.GetPosY()-20),100,100),RectI(Vei2(1*9,2*13),2*9,2*13),RectI(Vei2(50,50),500,500), s, SpriteEffect::ChromaColor(Colors::Magenta),Colors::Blue);
//	f.DrawText("GUESS WHAT!", wnd.mouse.GetPosX(), wnd.mouse.GetPosY());
}
