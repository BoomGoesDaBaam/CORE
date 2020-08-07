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
	resC(std::make_shared<ResourceCollection>(gfx)),
	curW(std::make_unique<World>(World::WorldSettings(),resC,c))
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
	//OutputDebugStringW(L"I was here.");

	passedTime += 0.01f;
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
	while (!wnd.mouse.IsEmpty())
	{
		HandleMouseInput(wnd.mouse.Read());
	}
	while (!wnd.kbd.KeyIsEmpty())
	{
		Keyboard::Event e = wnd.kbd.ReadKey();
		HandleKeyboardInput(e);
		if (e.IsRelease() && e.GetCode() == 'D')
		{
			debugInfoOn = !debugInfoOn;
		}
	}
}

void Game::ComposeFrame()
{
	fps_c++;
	curW->Draw(gfx);
	go.Draw();
	resC->tC.Update(0.015f);

	std::ostringstream oss3;
	oss3 << "FPS: " << fps_d;
	resC->tC.fonts.at(0).DrawText(oss3.str().c_str(), 5, 5, 13, Colors::Black);

	if (debugInfoOn)
	{
		std::ostringstream oss1, oss2;
		oss1 <<"World cords:(" << curW->GetmCell().x << " | " << curW->GetmCell().y << ")" << " Camera:(" << c.x << " | " << c.y << ")";
		oss2 <<"Ange:(" << curW->GetfCell().x << "|" << curW->GetfCell().y << ")" << "   CSize:" << curW->GetcSize().x;
		resC->tC.fonts.at(0).DrawText(oss1.str().c_str(), 200, 25, 15, Colors::Red);
		resC->tC.fonts.at(0).DrawText(oss2.str().c_str(), 25, 45, 15, Colors::Red);
		Vei2 mos = Graphics::GetMidOfScreen();

		gfx.DrawCircle(mos.x, mos.y, 2, Colors::Black);
	}
}
//Handle
void Game::HandleMouseInput(Mouse::Event& e)
{

	curW->HandleMouseEvents(e,gH);
}
void Game::HandleKeyboardInput(Keyboard::Event& e)
{
	curW->HandleKeyboardEvents(e);
}

