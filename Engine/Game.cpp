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
	curW(std::make_unique<World>(World::WorldSettings(),resC,c, &player)),
	igwH(resC)

{
	igwH.LoadScene(0,curW.get());
	//Settings::ReloadFile();

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
	float dt = 0.01f;
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
	while (!igwH.BufferIsEmpty())
	{
		HandleFrameLogic(igwH.Read());
	}

	curW->UpdateGameLogic(dt);
	curW->SetMoveMode(wnd.mouse.ShiftIsPressed());
	/*
	if (curW->GetBuildMode() &&!curW->GetPlayer()->GetMaterials().Has(Settings::obstacleStats[curW->GetPlaceObstacle()].neededResToBuild))
	{
		curW->SetBuildMode(false);
	}
	*/
}

void Game::ComposeFrame()
{
	Vec2 mP = (Vec2)wnd.mouse.GetPos();
	fps_c++;

	if (!curW->BuildMenuShown())
	{
		curW->Draw(gfx);
		go.Draw();
		resC->tC.Update(0.015f);
		std::ostringstream oss3;
		oss3 << "FPS: " << fps_d;
		resC->tC.fonts.at(0).DrawText(oss3.str().c_str(), 5, 5, 13, Colors::Black);

		if (debugInfoOn)
		{
			std::ostringstream oss1, oss2, oss4, oss5;
			oss1 << "World cords" << curW->GetmChunk().x << " Camera:(" << c.x << " | " << c.y << ")" << "mP: " << mP << "mChunk: " << curW->GetmChunk() << "n90rot: ";
			if (curW->GetFocusedObstacle() != nullptr)
			{
				oss1 << curW->GetFocusedObstacle()->n90rot;
			}
			oss2 << "fCell: " << curW->GetfCell() << "    fTile: " << curW->GetfTile() << "   CSize:" << curW->GetcSize().x << "   x-Felder:" << curW->GetRenderRect().left;
			oss4 << "Type:" << curW->GetfCellType() << "  use count tC:" << resC.use_count() << " ignoreMouse:" << ignoreMouse << " opt1:" << Settings::obstaclesOn << " updatedGraphics:" << curW->updatedGraphics;
			resC->tC.fonts.at(0).DrawText(oss1.str().c_str(), 25, 25, 14, Colors::Red);
			resC->tC.fonts.at(0).DrawText(oss2.str().c_str(), 25, 45, 14, Colors::Red);
			resC->tC.fonts.at(0).DrawText(oss4.str().c_str(), 25, 65, 14, Colors::Red);
			Vei2 mos = Graphics::GetMidOfScreen();
			gfx.DrawCircle(mos.x, mos.y, 2, Colors::Black);
			for (int y = 1; y <= 7; y++)
			{
				gfx.DrawLine(Vec2((float)(y * 100), 0.f), Vec2((float)(y * 100), 600.f), Colors::Red);
			}
			for (int x = 1; x <= 5; x++)
			{
				gfx.DrawLine(Vec2(0.f, (float)(x * 100)), Vec2(800.f, (float)(x * 100)), Colors::Red);
			}
			//gfx.DrawLine((Vec2)wnd.mouse.GetPos(), (Vec2)wnd.mouse.GetPos() + Vec2(curW->GetcSize().x, curW->GetcSize().y), Colors::Red);
			gfx.PutPixel((int)mP.x, (int)mP.y, Colors::Black);
		}
	}
	else
	{

	}
	igwH.Draw(gfx);
}
//Handle
void Game::HandleMouseInput(Mouse::Event& e)
{
	
		if (ignoreMouse && e.GetType() == Mouse::Event::Type::LRelease)
		{
			ignoreMouse = false;
			igwH.HandleMouseInput(e);
			return;
		}
		if (igwH.HandleMouseInput(e))
		{
			gH.Release();
			if (e.GetType() == Mouse::Event::Type::LPress)
			{
				ignoreMouse = true;
			}
		}
		else
		{
			curW->HandleMouseEvents(e, gH);
			World& w = *curW.get();
			igwH.UpdateFieldinformation(w, &player);
		}
		HandleFrameChanges();
}
void Game::HandleKeyboardInput(Keyboard::Event& e)
{
	curW->HandleKeyboardEvents(e);
}
void Game::HandleFrameChanges()
{
	if (igwH.GetCurScene() == 0)
	{
		Obstacle* obstacle = curW->GetFocusedObstacle();
		if (curW->UpdateUnitInfo() && obstacle != nullptr)
		{
			igwH.UpdateUnitinformation(curW->GetFocusedObstacle());
			curW->UnitUpdated();
		}
		if (obstacle == nullptr || !Settings::anyOfUnit(curW->GetFocusedObstacle()->type))
		{
			igwH.HideUnitInfo();
		}
		if (curW->NeedToLoadBuildMenu())
		{
			//curW->BuildMenuLoaded();
		}
	}
}
void Game::HandleFrameLogic(FrameEvent& e)
{
	if (e.GetType() == FrameEvent::ButtonPressed)
	{
		if (e.GetAction() == "priv page")
		{
			PageFrame* p = static_cast<PageFrame*>(e.GetCaller()->parentC);
			p->PriviousPage();
		}
		if (e.GetAction() == "next page")
		{
			Component* caller = e.GetCaller();
			PageFrame* p = static_cast<PageFrame*>(e.GetCaller()->parentC);
			p->NextPage();
		}
		if (e.GetAction() == "enable buildmode")
		{
			if (curW->GetPlayer()->GetMaterials().Has(Settings::obstacleStats[e.GetExtra()].neededResToBuild))
			{
				curW->SetBuildMode(e.GetExtra());
				igwH.LoadScene(0,curW.get());
			}
		}
		if (e.GetAction() == "next turn")
		{
			curW->NextTurn();
			if (curW->GetFocusedObstacle() != nullptr && Settings::anyOfUnit(curW->GetFocusedObstacle()->type))
			{
				igwH.UpdateUnitinformation(curW->GetFocusedObstacle());
			}
		}
		if (e.GetAction() == "load scene")
		{
			igwH.LoadScene(e.GetExtra(), curW.get());
		}
		if (e.GetAction() == "set attackMode")
		{
			curW->SetAttackMode(true);
		}
	}
}

