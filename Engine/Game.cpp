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
	, mat2(mat.Get3x3Surrounded(1, 2, 0))
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
	for (int i = 0; i < 100; i++)
	{
		go.Update(0.001);
	}
	
//	go.AddVolcano((Vec2)wnd.mouse.GetPos(), 50*rr.GetFaktor(), 50);
//	go.AddShot((Vec2)wnd.mouse.GetPos(),Vec2(500,50),50.0f);
	float dt = 0.05f;
	if (t.CheckIfTimePassed(dt))
	{
		Vec2 mP = (Vec2)wnd.mouse.GetPos();
		std::vector<Vec2> v = { {-2,-2},{2,-2},{2,2},{-2,2},{3,5},{1,-7},{3,-5} };
		PARTCONF p = PARTCONF();
		p.body = v;
		p.pos = (Vec2)wnd.mouse.GetPos();
		go.AddVoc(&POLYGON(), &p, 8, 10);
		go.AddVoc(&POLYGON(), &p, 2, 20);
		go.AddVoc(&POLYGON(), &p, 1, 40);
	
	}
}

void Game::ComposeFrame()
{
	go.Draw();
}

