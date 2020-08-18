#include <vector>
#include <memory>
#include "Rect.h"
#include "Graphics.h"
#include "SpriteEffect.h"
#include "RandyRandom.h"
#include "MainWindow.h"
#include "GrabHandle.h"
#include "Matrix.h"
#include "ResourceCollection.h"
#include "GigaMath.h"
#pragma once

class World
{
public:
	class WorldSettings
	{
	public:
		int defBlueprint = 0;
		int defType = 0;
		Vei2 wSize = { 200, 200 };
		Vei2 cSize = { 50,50 };
	};
private:
	class Objekt
	{

	};
	class Cell
	{
	public:
		Cell(int type):type(type){}
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
		const Cell& InWorldAT(int y)const
		{
			return cellsY.at((y + cellsY.size()) % cellsY.size());
		}

		const Cell& operator[](std::size_t i)const {
			assert(i >= 0 && i < cellsY.size());
			return cellsY[i]; }
		Cell& operator[](std::size_t i) { return cellsY[i]; }
	};

	class Cells
	{
		std::vector<CellCol> cellsX;
		int PutInWorld(int x)const
		{
			if (x < 0)
			{
				x = -x;
				x %= cellsX.size();
				x = cellsX.size() - x;
			}
			if (x >= cellsX.size())
			{
				x = x % cellsX.size();
			}
			return x;
		}
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
		const Cell& InWorldAT(Vei2 curXY)const { return cellsX.at(PutInWorld(curXY.x)).InWorldAT(curXY.y); }
		
		const Cell& operator()(Vei2 pos)const { return cellsX[pos.x][pos.y]; }
		Cell& operator()(Vei2 pos) { return cellsX[pos.x][pos.y]; }
		const CellCol& operator[](std::size_t idx) const { return cellsX[idx]; }
		CellCol& operator[](std::size_t idx) { return cellsX[idx]; }
	};
	//Resourcen
	std::shared_ptr<ResourceCollection> resC;
	TexturesCollection* tC;
	FramesizeCollection* fsC;
	RandyRandom rng;
	//Gamevars
	Vei2 wSize = { 0,0 };
	Vei2 cSize = { 0,0 };
	Vei2 fCell = { 0,0 };					//angeklickte Zelle
	Vei2 mCell = { 0,0 };					//Zelle in der Mitte des Bildschirms audem Debugzeiger ist
	Cells cells;
	std::vector<Matrix<int>> conMap;		//Connectionmap	 (1 = needsConnections, 0 = doesn't, vectorindex for type)
	Matrix<int> groundedMap;				// 0 = spot is not grounded, 1 = is grounded, -1 = not identified yet (will be 0 if not changed)
	Vec2& c;								//Camera
	int grit=0;

	//Private const Functions
	RectF GetCellRect(Vei2 cellP)const;
	Vei2 GetCellHit(Vec2 mouseP)const;
	void Init(WorldSettings& s);
	bool IsInWorld(Vei2& v)const;
	bool IsInWorldY(int y)const;		
	Vei2 PutInWorldX(Vei2 v)const;		//Calculates coordinates when x negativ or > cSize.x  
	Matrix<int> GetAroundMatrix(Vei2 cell)const;	//in bounds: type		outside bounds(y-wise): -1		
	void UpdateConMap();							//UpdateConMap must be called before UpdateGroundedMap
	void UpdateGroundedMap();
	bool IsSurroundedBy(Vei2 pos, int type);		//3x3 around pos
	//Private not const Funktions
	void Zoom(Vei2 delta);				//Delta == delta cSize
	void ApplyCameraChanges(Vec2 cDelta);


	void Generate(WorldSettings& s);
	void GenerateCircle(Vei2 pos, int radius, int type, int ontoType = -1, int surrBy = -1); 
	void GenerateLine(Vec2 p0, Vec2 p1, int type, int ontoType = -1, int thickness = 1, int surrBy = -1);
	void GenerateExplosion(Vei2 pos, int maxLineLength, int type, int ontoType = -1, int nRolls = 100, int surrBy = -1);
	bool FIDF(int first, int second)const;//First is drawn first
	void CutHills(int replaceTo);
	//void GenerateWhenSurrBy(Vei2 pos, int type, int surrBy);
public:
																							//Konstruktor + Operatoren
	World(WorldSettings wSettings, std::shared_ptr<ResourceCollection> resC, Vec2& camera);
	//change game values
	int World::GetxStart() {
		Vei2 mos = Graphics::GetMidOfScreen();
		return -(mos.x / cSize.x) * 2;
	};
	//Handles
	void HandleMouseEvents(Mouse::Event& e, GrabHandle& gH);
	void HandleKeyboardEvents(Keyboard::Event& e);
	//Grafiken
	void Draw(Graphics& gfx)const;
	void DrawConnections(int onCell, Vei2 topLeft, Matrix<int> aMat, Graphics& gfx)const;
	std::vector<SubAnimation> GetConnectionAnimationVec(int lookFor, Matrix<int> aMat)const;
	bool NeedsConnections(Vei2 curXY)const;
	//
	Vei2 GetwSize() { return wSize; }
	Vei2 GetcSize() { return cSize; }
	Vei2 GetfCell() { return fCell; }
	int GetfCellType() { return cells(fCell).type; }
	Vei2 GetmCell() { return mCell; }
};

