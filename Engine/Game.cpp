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
#include <sstream>
Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd),
	go(gfx),
	tC(std::make_shared<TexturesCollection>(gfx)),
	w(World(tC))
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
	//Set Up Things
	if (t.CheckIfTimePassed(1.0f))//FPS
	{
		fps_d = fps_c;
		fps_c = 0;
	}
	for (int i = 0; i < 100; i++)//Update Particles
	{
		go.Update(0.001);
	}



}

void Game::ComposeFrame()
{
	fps_c++;
	w.Draw(gfx);
	go.Draw();

	std::ostringstream oss;
	oss <<"FPS: "<< fps_d;
	tC->fonts.at(0).DrawText(oss.str().c_str(), 25, 25, 25, Colors::Red);

}

