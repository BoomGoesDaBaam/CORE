/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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
#pragma once
#define _USE_MATH_DEFINES
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Font.h"
#include "SpriteEffect.h"
#include "GraphicObjects.h"
#include "Matrix.h"
#include "Tim.h"
#include "World.h"
#include "ResourceCollection.h"
#include "GrabHandle.h"
#include "FrameHandle.h"
#include "Team.h"
#include "WorldGenerator.h"
#include "Settings.h"
class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
	void Game::HandleMouseInput(Mouse::Event& e);
	void Game::HandleKeyboardInput(Keyboard::Event& e);
	void Game::HandleFrameLogic(FrameEvent& e);
	void Game::HandleFrameChanges();
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */

	RandyRandom rr;
	Tim t;
	Vec2 c = { 0,0 };
	float passedTime = 0;
	bool debugInfoOn = false,ignoreMouse=false;			//Ignores the Mouseiput for curW unless eventtype is LRelease
	Vei2 debugMarkerPos = Vei2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight / 2);
	Vei2 markPos = Vei2(Graphics::ScreenWidth / 2, Graphics::ScreenHeight / 2);

	std::map<std::string, Team> teams;

	std::shared_ptr<ResourceCollection> resC = nullptr;
	GraphicObjects go;
	std::unique_ptr<World> curW;
	FrameHandle igwH;
	GrabHandle gH;
	int fps_c = 0, fps_d = 0;
	/********************************/
};