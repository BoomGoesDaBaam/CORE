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

//#include <sstream>
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
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	Font f = { "Spritesheet.bmp",9,11,9,13,'!','~',gfx };
	GraphicObjects go;
	RandyRandom rr;
	Tim t;
	float passedTime = 0;

	std::shared_ptr<ResourceCollection> resC = nullptr;

	std::unique_ptr<World> curW;
	GrabHandle gH;
	Vec2 c = { 0,0 };

	int fps_c, fps_d;
	/********************************/
};