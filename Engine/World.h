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
#include "Team.h"
#pragma once

class World
{
public:
	class WorldSettings
	{
	public:
		WorldSettings()
		{
#ifdef _DEBUG 
			wSize = Vei2(50, 50);
#endif
		}
		int defBlueprint = 0;
		int defType = 0;
		Vei2 wSize = { 200, 200 };
		Vei2 cSize = { 50,50 };
	};
private:
	class Obstacle
	{
	protected:
		Vei2 tilePos;				//pos is bottomleft
		int type;
		int animSets = 1;
		sharedResC resC;
	public:
		Obstacle(Vei2 tilePos, int type, sharedResC resC)
			:
			tilePos(tilePos),
			type(type),
			resC(std::move(resC))
		{

		}
		int GetType()const
		{
			return type;
		}
		virtual void Draw(Graphics& gfx, RectF tilePos)const
		{
			Vec2 tileSize = Vec2(tilePos.GetWidth(), tilePos.GetHeight());

			tilePos.right += tileSize.x * (Settings::obstacleSizes[type].x - 1);
			tilePos.top -= tileSize.y * (Settings::obstacleSizes[type].y - 1);
			gfx.DrawSurface((RectI)tilePos, resC->tC.obstacles[type].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
		}
	};

	class MultiAnimObstacle: public Obstacle
	{
		int curA = 0;
	public:
		MultiAnimObstacle(Vei2 tilePos, int type, sharedResC resC)
			:
			Obstacle(tilePos, type, resC)
		{
			curA++;
		}
		virtual void Draw(Graphics& gfx, RectF tilePos)const override
		{
			if (curA == 0)
			{
				Vec2 tileSize = Vec2(tilePos.GetWidth(), tilePos.GetHeight());
				tilePos.right += tileSize.x * (Settings::obstacleSizes[type].x - 1);
				tilePos.top -= tileSize.y * (Settings::obstacleSizes[type].y - 1);
				gfx.DrawSurface((RectI)tilePos, resC->tC.obstacles[type].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else
			{
				Vec2 tileSize = Vec2(tilePos.GetWidth(), tilePos.GetHeight());
				int multiObstIndex = Settings::Obstacle2MultiObstacle(type);
				tilePos += Vec2(Settings::multiObstaclePos[multiObstIndex].x * tileSize.x, - Settings::multiObstaclePos[multiObstIndex].y * tileSize.y) ;
				Vei2 size = Settings::multiObstacleSize[multiObstIndex][(__int64)curA - 1];
				tilePos.right += tileSize.x * (size.x - 1);
				tilePos.top -= tileSize.y * (size.y - 1);
				gfx.DrawSurface((RectI)tilePos, resC->tC.multiObstacles[multiObstIndex].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
		}
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
				x = (int)cellsX.size() - x;
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

		const Cell& operator()(Vei2 pos)const { return cellsX[pos.x][pos.y]; }
		Cell& operator()(Vei2 pos) { return cellsX[pos.x][pos.y]; }
		const CellCol& operator[](std::size_t idx) const { 
			assert (idx >= 0 && idx < cellsX.size()); return cellsX[idx];
		}
		CellCol& operator[](std::size_t idx) {
			assert(idx >= 0 && idx < cellsX.size()); return cellsX[idx]; 
		}
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
	Vei2 fTile = { 0,0 };
	Vei2 mCell = { 0,0 };					//Zelle in der Mitte des Bildschirms auf dem Debugzeiger ist
	Cells cells;
	std::vector<Matrix<int>> conMap;		//Connectionmap	 (1 = needsConnections, 0 = doesn't, vectorindex for type)
	Matrix<int> groundedMap;				// '0' = spot is not grounded, '1' = is grounded, '-1' = not identified yet (will be 0 if not changed)
	Matrix<int> obstacleMap;				// '-1' = empty   < -1 index of obstacle in obstacleVec
	std::vector<std::unique_ptr<Obstacle>> obstacles;

	Vec2& c;								//Camera
	bool grit=false;						//show grit
	bool buildMode = false;					//place something
	int placeObstacle = 0;
	bool posAllowed = true;

	Team player = Team("Die reinlich raeudigen Raucher");
	//Private const Functions
	RectF GetCellRect(Vei2 cellP)const;
	RectF GetTileRect(Vei2 tileP)const;
	Vei2 GetCellHit(Vec2 mP)const;
	Vei2 GetTileHit(Vec2 mP)const;	
	bool CellIsInWorld(Vei2& pos)const;
	bool CellIsInWorldY(int y)const;		
	Vei2 PutCellInWorldX(Vei2 pos)const;		//Calculates coordinates when x negativ or > cSize.x  
	Vei2 PutTileInWorld(Vei2 pos)const;
	Vei2 PutTileInWorld(int x, int y)const;
	bool TileIsInWorld(Vei2& pos)const;


	Matrix<int> GetAroundMatrix(Vei2 cell)const;	//in bounds: type		outside bounds(y-wise): -1		
	void UpdateConMap();							//
	void UpdateGroundedMap(Vei2 pos=Vei2(0,0), Vei2 size = Vei2(-1,-1));						// VERY performance heavy - UpdateConMap must be called before UpdateGroundedMap
	void SetTilesAT(Vei2 pos, int value);
	void SetTilesAT(Vei2 pos, Matrix<int> matrix);	//sets tile(x0,y0) in Cell(x1,y1) to type when matrix at(x0,y0) != 0
	bool IsSurroundedBy(Vei2 pos, int type);		//3x3 around pos
	//Private not const Funktions
	void Zoom(Vei2 delta);				//Delta == delta cSize
	void ApplyCameraChanges(Vec2 cDelta);


	void Init(WorldSettings& s);
	void Generate(WorldSettings& s);
	void GenerateCircle(Vei2 pos, int radius, int type, int ontoType = -1, int surrBy = -1); 
	void GenerateLine(Vec2 p0, Vec2 p1, int type, int ontoType = -1, int thickness = 1, int surrBy = -1);
	void GenerateExplosion(Vei2 pos, int maxLineLength, int type, int ontoType = -1, int nRolls = 100, int surrBy = -1);
	bool FIDF(int first, int second)const;//First is drawn first
	void CutHills(int replaceTo);
	void GenerateObstacle(Vei2 tilePos, int type);
	bool ObstaclePosAllowed(Vei2 tilePos, int type);
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
	//Grafiken + Einbindung dieser in groundedMap
	void Draw(Graphics& gfx)const;
	void DrawObstacle(Vei2 tilePos, int type, Graphics& gfx, int frame = -1)const;

	std::vector<SubAnimation> GetConnectionsOfTypes(Vei2 pos, int* types);
	void PlaceConnectionsIntoCelltiles(Vei2 pos, int value, int mixed, int valueOfZero, const int* types);
	void PlaceTilesForMaskedField(Vei2 pos, int value, int valOfMixed, int valueOfZero, int type);
	void ChangeGroundedVal(int from, int to);
	void PlaceLadderableTiles(int type);
	bool NeedsConnections(Vei2 curXY)const;
	void SetBuildMode(int obstacle);
	//
	Vei2 GetwSize()const { return wSize; }
	Vei2 GetcSize()const { return cSize; }
	Vec2 GetTileSize()const { return (Vec2)cSize/Settings::CellSplitUpIn; }
	Vei2 GetfCell()const { return fCell; }
	Vei2 GetfTile()const { return fTile; }
	int GetfCellType()const { return cells(fCell).type; }
	Vei2 GetmCell()const { return mCell; }
	Team& GetPlayer() { return player; }
};

