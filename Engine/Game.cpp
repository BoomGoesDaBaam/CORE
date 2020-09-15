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
	resC(std::make_shared<ResourceCollection>(gfx)),
	go(gfx, resC),
	curW(std::make_unique<World>(World::WorldSettings(),resC,c)),
	igwH(resC)
{

	PARTCONF pc(resC);
	pc.pos = Vec2(60, 60);
	pc.pos = Vec2(60, 60);
	Matrix<int>m(3, 5, 1);
	pc.size = 50;

	igwH.AddFrame(RectF(Vec2(540, 110),140,280), 0, 2, resC);
	std::vector<int> a = {1, 0};
	igwH[0].AddText(Settings::lang_Feldinformationen[Settings::lang], RectF(Vec2(35, 3), 50, 50), 7, resC->tC.fonts[0], Colors::Black);
	igwH[0].AddText(Settings::lang_Flora[Settings::lang] + ":", RectF(Vec2(10, 30), 50, 50), 7, resC->tC.fonts[0], Colors::Black,a);
	igwH[0].AddText(Settings::lang_Flora[Settings::lang] + ":", RectF(Vec2(70, 30), 50, 50), 7, resC->tC.fonts[0], Colors::Black, a);

	
	//AddScrollWindow(RectF(Vec2(50, 50), 50, 50), RectF(Vec2(110, 50), 10, 50));
	//AddText(RectF(Vec2(50, 100), 500, 500), "Yahhhhhhooooouuuu", 10, configs.resC->tC.fonts[0]);
	//TILEFRAME p(pc, m);
	//p.AddText(RectF(Vec2(0, 0), 100, 100), "Ressourcen", 14, resC->tC.fonts[0],Colors::Black);
	//p.AddButton(RectF(Vec2(0, 0), 50, 100),Colors::Blue);
	//p.AddScrollWindow(RectF(Vec2(50, 50), 50, 50), RectF(Vec2(60, 60), 10, 50));
	//go.Add(&p);
	
	//go.AddVoc(&p, &pc, 20, 70);
	
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

	//go.objects[0]->SetPos((Vec2)wnd.mouse.GetPos());
	/*
	PARTCONF pc(resC);
	pc.pos = (Vec2) wnd.mouse.GetPos();
	
	PARTICLE p = PARTICLE(pc);
	*/

	//go.AddVoc(&p, &pc, 20, 70);


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
		std::ostringstream oss1, oss2,oss4;
		oss1 <<"World cords:(" << curW->GetmCell().x << " | " << curW->GetmCell().y << ")" << " Camera:(" << c.x << " | " << c.y << ")";
		oss2 <<"Ange:(" << curW->GetfCell().x << "|" << curW->GetfCell().y << ")" << "   CSize:" << curW->GetcSize().x << "   x-Felder:"<<curW->GetxStart();
		oss4 << "Type:"<<curW->GetfCellType()<<"  use count:"<<resC.use_count()<<" Something:"<< ignoreMouse;
		resC->tC.fonts.at(1).DrawText(oss1.str().c_str(), 200, 25, 15, Colors::Red);
		resC->tC.fonts.at(0).DrawText(oss2.str().c_str(), 25, 45, 20, Colors::Red);
		resC->tC.fonts.at(0).DrawText(oss4.str().c_str(), 25, 65, 20, Colors::Red);
		Vei2 mos = Graphics::GetMidOfScreen();
		gfx.DrawCircle(mos.x, mos.y, 2, Colors::Black);
	}
	igwH.Draw(gfx);
}
//Handle
void Game::HandleMouseInput(Mouse::Event& e)
{
	if (e.IsValid())
	{
		if (ignoreMouse && e.GetType() == Mouse::Event::Type::LRelease)
		{
			ignoreMouse = false;
			igwH.HandleMouseInput(e);
			return;
		}
		if (igwH.HandleMouseInput(e) && e.GetType() == Mouse::Event::Type::LPress)
		{
			ignoreMouse = true;
			gH.Unlock();
		}
		else
		{
			curW->HandleMouseEvents(e, gH);
			igwH.UpdateFieldinformation(*curW);
		}
	}
}
void Game::HandleKeyboardInput(Keyboard::Event& e)
{
	curW->HandleKeyboardEvents(e);
}

