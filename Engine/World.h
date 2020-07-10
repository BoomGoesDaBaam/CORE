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
		int defType = 0;
		Vei2 wSize = { 500, 500 };
		Vei2 cSize = { 50,50 };
		int nIslands=200;
	};
private:
	class Cell
	{
	public:
		Cell(int type) :type(type){}
		int type = 0;
	};

	class CellCol
	{
		std::vector<Cell> cellsY;
	public:
		CellCol(int ySize,int type)
		{
			for (int y = 0; y < ySize; y++)
			{
				cellsY.push_back(Cell(type));
			}
		}
		
		const Cell& operator[](std::size_t i)const {
			assert(i >= 0 && i < cellsY.size());
			return cellsY[i]; }
		Cell& operator[](std::size_t i) { return cellsY[i]; }
	};

	class Cells
	{
		std::vector<CellCol> cellsX;
	public:
		Cells(Vei2 wSize, int defType)
		{
			Init(wSize, defType);
		}
		Cells() = delete;
		void Init(Vei2 wSize, int defType)
		{
			cellsX.clear();
			for (int x = 0; x < wSize.x; x++)
			{
				cellsX.push_back(CellCol(wSize.y, defType));
			}
		}
		const Cell& operator()(Vei2 pos)const { return cellsX[pos.x][pos.y]; }
		Cell& operator()(Vei2 pos) { return cellsX[pos.x][pos.y]; }
		const CellCol& operator[](std::size_t idx) const { return cellsX[idx]; }
		CellCol& operator[](std::size_t idx) { return cellsX[idx]; }
	};
	//Vars
	std::shared_ptr<TexturesCollection> tC;
	RandyRandom rng;
	//Gamevars
	Vei2 wSize = { 0,0 };
	Vei2 cSize = { 0,0 };
	Vei2 fCell = { 0,0 };				//angeklickte Zelle
	Vei2 mCell = { 0,0 };				//Zelle in der Mitte des Bildschirms
	Cells cells;
	Vec2& c;							//Camera

	//Private const Functions
	RectF GetCellRect(Vei2 cellP)const;
	Vei2 GetCellHit(Vec2 mouseP)const;
	void LoadSettings(WorldSettings& s);
	bool IsInWorld(Vei2& v)const;
	Vei2 PutInWorldX(Vei2& v)const;		//Calculates coordinates when x negativ or > cSize.x   !!! DANGER 1 !!! Reference!!!
	//Private not const Funktions
	void Zoom(Vei2 delta);				//Delta == delta cSize

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

