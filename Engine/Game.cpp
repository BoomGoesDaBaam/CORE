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
	//resC->tC.IdkCallOnce();



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
		resC->UpdateSurfs(0.015f);
		std::ostringstream oss3;
		oss3 << "FPS: " << fps_d;
		//gfx.DrawText()
		//resC->tC.fonts.at(0).DrawText(oss3.str().c_str(), 5, 5, 25, Colors::Black);
		gfx.DrawText(oss3.str().c_str(), 5, 5, 25, &resC->GetSurf().fonts[0], SpriteEffect::ChromaColor(Colors::Magenta, Colors::Black));

		if (debugInfoOn)
		{
			std::ostringstream oss1, oss2, oss4, oss5;
			oss1 << "World cords" << curW->GetmChunk().x << " Camera:(" << c.x << " | " << c.y << ")" << "mP: " << mP << "mChunk: " << curW->GetmChunk() << "n90rot: ";
			if (curW->GetFocusedObstacle() != nullptr)
			{
				oss1 << curW->GetFocusedObstacle()->n90rot;
			}
			oss1 << " obstaclesOnCHunk:" << curW->GetObstacleCount() << " PlaceFieldType: "<<Settings::GetTypeString(curW->GetPlaceField());
			oss2 << "fCell: " << curW->GetfCell() << "    fTile: " << curW->GetfTile() << "   CellSize:" << curW->GetcSize().x << "   x-Felder:" << curW->GetRenderRect().left << " Markerpos:" << debugMarkerPos << " Markpos:" << markPos << "Dist: " << Vei2(std::abs(debugMarkerPos.x - markPos.x)+1, std::abs(debugMarkerPos.y - markPos.y)+1);
			oss4 << "Type:" << curW->GetfCellType() << "  use count tC:" << resC.use_count() << " ignoreMouse:" << ignoreMouse << " opt1:" << Settings::obstaclesOn << " updatedGraphics:" << curW->updatedGraphics;
			//resC->tC.fonts.at(0).DrawText(oss1.str().c_str(), 25, 25, 14, Colors::Red);
			//resC->tC.fonts.at(0).DrawText(oss2.str().c_str(), 25, 45, 14, Colors::Red);
			//resC->tC.fonts.at(0).DrawText(oss4.str().c_str(), 25, 65, 14, Colors::Red);
			gfx.DrawText(oss1.str().c_str(), 25, 25, 14, &resC->GetSurf().fonts[0], SpriteEffect::ChromaColor(Colors::Magenta, Colors::Red));
			gfx.DrawText(oss2.str().c_str(), 25, 45, 14, &resC->GetSurf().fonts[0], SpriteEffect::ChromaColor(Colors::Magenta, Colors::Red));
			gfx.DrawText(oss4.str().c_str(), 25, 65, 14, &resC->GetSurf().fonts[0], SpriteEffect::ChromaColor(Colors::Magenta, Colors::Red));
			Vei2 mos = Graphics::GetMidOfScreen();
			//gfx.DrawCircle(mos.x, mos.y, 2, Colors::Black);
			for (int y = 1; y <= 7; y++)
			{
				gfx.DrawLine(Vec2((float)(y * 100), 0.f), Vec2((float)(y * 100), 600.f), Colors::Red);
			}
			for (int x = 1; x <= 5; x++)
			{
				gfx.DrawLine(Vec2(0.f, (float)(x * 100)), Vec2(800.f, (float)(x * 100)), Colors::Red);
			}
			//gfx.DrawLine((Vec2)wnd.mouse.GetPos(), (Vec2)wnd.mouse.GetPos() + Vec2(curW->GetcSize().x, curW->GetcSize().y), Colors::Red);
			//gfx.PutPixel((int)mP.x, (int)mP.y, Colors::Black);
			gfx.PutPixel((int)debugMarkerPos.x, (int)debugMarkerPos.y, Colors::Black);
			gfx.PutPixel((int)markPos.x, (int)markPos.y, Colors::Black);
		}

		//gfx.DrawSurface(RectI(Vei2(100, 100), 400, 400), RectI(Vei2(0, 0), 200, 200), resC->tC.newFields[1].GetCurSurface(), SpriteEffect::Chroma(), 0);
	}
	else
	{
	}
	igwH.Draw(gfx);
}
//Handle
void Game::HandleMouseInput(Mouse::Event& e)
{
	if (debugInfoOn && e.GetType() == Mouse::Event::Type::LRelease)
	{
		debugMarkerPos = (Vei2)e.GetPos();
	}
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
	if (debugInfoOn && e.IsRelease())
	{
		if (e.GetCode() == 'W')
		{
			debugMarkerPos.y--;
		}
		if (e.GetCode() == 'A')
		{
			debugMarkerPos.x--;
		}
		if (e.GetCode() == 'S')
		{
			debugMarkerPos.y++;
		}
		if (e.GetCode() == 'F')
		{
			debugMarkerPos.x++;
		}
		if (e.GetCode() == 'M')
		{
			markPos = debugMarkerPos;
		}
	}
	curW->HandleKeyboardEvents(e);
}
void Game::HandleFrameChanges()
{
	if (igwH.GetCurScene() == 0)
	{
		Obstacle* obstacle = curW->GetFocusedObstacle();
		if (curW->UpdateFrameInfo())
		{
			igwH.UpdateFrames(curW->GetFocusedObstacle(),curW->GetStorageObstacle());
			curW->FramesUpdated();
		}
	}
}
void Game::HandleFrameLogic(FrameEvent& e)
{
	if (e.GetType() == FrameEvent::Type::ButtonPressed)
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
		if (e.GetAction() == "set crafting queue")
		{
			if (curW->GetPlayer()->GetMaterials().Has(Settings::itemStats[e.GetExtra()].neededResToCraft))
			{
				curW->SetCraftMode(e.GetExtra());
				igwH.LoadScene(0, curW.get());
				igwH.UpdateFrames(curW->GetFocusedObstacle(),curW->GetStorageObstacle());
			}
		}
		if (e.GetAction() == "next turn")
		{
			curW->NextTurn();
			if (curW->GetFocusedObstacle() != nullptr)
			{
				igwH.UpdateFrames(curW->GetFocusedObstacle(),curW->GetStorageObstacle());
			}
		}
		if (e.GetAction() == "load scene")
		{
			igwH.LoadScene(e.GetExtra(), curW.get());
		}
		if (e.GetAction() == "set attackMode")
		{
			curW->SetAttackMode(!curW->GetAttackMode());
		}
		if (e.GetAction().find("set obstacle state") != std::string::npos)
		{
			Obstacle* obstacle = curW->GetFocusedObstacle();
			if (e.GetAction().find("townhall") != std::string::npos)
			{
				if (obstacle->education != nullptr)
				{
					//Disable all boxes
					if (e.GetExtra() == 1)
					{
						obstacle->education->SetEducates(false);
					}
					if (e.GetExtra() == 3)
					{
						obstacle->heal->Disable();
					}
					if (e.GetExtra() == 5)
					{
						obstacle->attack->SetReloadNextTurn(false);
					}
					//Check for ticked box
					if (e.GetExtra() == 0)
					{
						obstacle->education->SetEducates(true);
					}
					else
					{
						obstacle->education->SetEducates(false);
					}
					if (e.GetExtra() == 2)
					{
						obstacle->heal->Enable();
					}
					else
					{
						obstacle->heal->Disable();
					}
					if (e.GetExtra() == 4)
					{
						obstacle->attack->SetReloadNextTurn(true);
					}
					else
					{
						obstacle->attack->SetReloadNextTurn(false);
					}
				}
			}
			if (e.GetAction().find("lumberjackHut") != std::string::npos)
			{
				if (e.GetExtra() == 0)
				{
					obstacle->attack->SetAutomaticMode(CtPos(Vei2(-2,-2), Vei2(-2, -2)));
				}
				else
				{
					obstacle->attack->SetAutomaticMode(CtPos(Vei2(-1, -1), Vei2(-1, -1)));
				}
			}
		}
	}
	else if (e.GetType() == FrameEvent::Type::ItemDragReleased)
	{
		if (curW->GetFocusedObstacle() != nullptr || curW->GetStorageObstacle() != nullptr)
		{
			Vec2 mP = (Vec2)wnd.mouse.GetPos();
			int hitSlot = 0; 
			Obstacle* giver = nullptr;
			Obstacle* reciever = nullptr;
			//
			if (e.GetAction().find("unit") != std::string::npos)
			{
				hitSlot = igwH.GetHitInventorySlot("unit",mP);
				giver = curW->GetFocusedObstacle();
			}
			std::vector<std::string> keyStorage = { "box","storage","wrought" };
			for (int i = 0; i < 3; i++)
			{
				std::string curKey = keyStorage[i];
				if (e.GetAction().find(curKey) != std::string::npos)
				{
					hitSlot = igwH.GetHitInventorySlot(curKey, mP);
					giver = curW->GetStorageObstacle();
				}
			}
			
			int releasedHitSlot;
			if (igwH.GetHitInventorySlot("unit",mP) != -1)
			{
				reciever = curW->GetFocusedObstacle();
				releasedHitSlot = igwH.GetHitInventorySlot("unit",mP);
			}
			for (int i = 0; i < 3; i++)
			{
				std::string curKey = keyStorage[i];
				if (igwH.GetHitInventorySlot(curKey, mP) != -1)
				{
					reciever = curW->GetStorageObstacle();
					releasedHitSlot = igwH.GetHitInventorySlot(curKey, mP);
				}
			}
			//
			if (reciever != nullptr)
			{
				std::unique_ptr<Slot>* move = giver->inv->GetItem(e.GetExtra());
				if (hitSlot != e.GetExtra() && reciever->inv->ItemFitsForSlotFlat(move, releasedHitSlot))
				{
					reciever->inv->SetItem(std::move(*move), releasedHitSlot);
					igwH.UpdateFrames(curW->GetFocusedObstacle(), curW->GetStorageObstacle());
				}
				else if (hitSlot != e.GetExtra() && reciever->inv->WouldFitWhenEmptyFlat(move, releasedHitSlot) && giver->inv->WouldFitWhenEmptyFlat(reciever->inv->GetItem(releasedHitSlot), e.GetExtra()))
				{
					std::unique_ptr<Slot> swap = std::move(*reciever->inv->GetItem(releasedHitSlot));

					reciever->inv->SetItem(std::move(*move), releasedHitSlot);
					giver->inv->SetItem(std::move(swap), e.GetExtra());
					igwH.UpdateFrames(curW->GetFocusedObstacle(), curW->GetStorageObstacle());
				}
			}
		}
	}
}

