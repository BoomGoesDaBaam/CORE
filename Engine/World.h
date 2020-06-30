#include <vector>
#include <memory>
#include "Rect.h"
#include "Graphics.h"
#include "TexturesCollection.h"
#include "SpriteEffect.h"
#include "RandyRandom.h"
#pragma once
class World
{
public:
	class WorldSettings
	{
	public:
		Vei2 wSize = { 50,50 };
		Vei2 cSize = { 50,50 };
		int nIslands=150;
	};
private:
	class Cell
	{
	public:
		Cell(int type):type(type)
		{

		}
		int type = 0;
	};

	//Vars
	std::shared_ptr<TexturesCollection> tC;
	RandyRandom rng;
	//Gamevars
	Vei2 wSize = { 0,0 };
	Vei2 cSize = { 0,0 };
	Vei2 fCell = { 0,0 };
	std::vector<Cell> cells;


	//Private Functions
	RectF GetCellRect(Vei2 cellP);
	void LoadSettings(WorldSettings& s);
	bool IsInWorld(Vei2 pos);
	int Vec2Number(Vei2 pos);
	Vei2 Number2Vec(int i);
	//World Generator
	void Generate(WorldSettings& s);
	void GenerateCircle(Vei2 where, int radius);
public:
	World(WorldSettings wSettings, std::shared_ptr<TexturesCollection> tC);
	World(std::shared_ptr<TexturesCollection> tC) :World(WorldSettings(), tC) {}
	void Draw(Graphics& gfx,Vec2& Camera);


	Vei2 GetwSize() { return wSize; }
	Vei2 GetcSize() { return cSize; }
	Vei2 GetfCell() { return fCell; }
};

