#include <vector>
#include <memory>
#include "Rect.h"
#include "Graphics.h"
#include "TexturesCollection.h"
#include "SpriteEffect.h"
#include "RandyRandom.h"
#include "MainWindow.h"
#pragma once
class World
{
public:
	class WorldSettings
	{
	public:
		Vei2 wSize = { 50, 50 };
		Vei2 cSize = { 100,100 };
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
	Vei2 fCell = { 0,0 };				//angeklickte Zelle
	Vei2 mCell = { 0,0 };				//Zelle in der Mitte des Bildschirms
	std::vector<Cell> cells;
	Vec2& c;							//Camera

	//Private Functions
	RectF GetCellRect(Vei2 cellP)const;
	Vei2 GetCellHit(Vec2 mouseP)const;
	void LoadSettings(WorldSettings& s);
	bool IsInWorld(Vei2 v)const;
	int Vec2Number(Vei2 v)const;
	Vei2 Number2Vec(int x)const;
	Vei2 PutInWorld(Vei2 v)const;		//Calculates coordinates when x negativ or > cSize.x
	//World Generator
	void Generate(WorldSettings& s);
	void GenerateCircle(Vei2 where, int radius, int type, float density = 1.0f); //density gibt Wert fuer Normalverteilung an
public:
	//Konstruktor + Operatoren
	World(WorldSettings wSettings, std::shared_ptr<TexturesCollection> tC, Vec2& camera);
	//change game values
	
	//Handles
	void ApplyCameraChanges(Vec2 cDelta);
	void HandleMouseEvents(Mouse::Event& e);
	//Grafiken
	void Draw(Graphics& gfx)const;

	Vei2 GetwSize() { return wSize; }
	Vei2 GetcSize() { return cSize; }
	Vei2 GetfCell() { return fCell; }
	Vei2 GetmCell() { return mCell; }
};

