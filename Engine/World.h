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
		Vei2 cSize = { 200, 200 };
		int chunkHasNCells = 5;
		Vei2 worldHasNChunks = Vei2(40,40);
	};
private:
	class Obstacle
	{
	protected:
		sharedResC resC;
	public:
		Vei2 tilePos;				//pos is bottomleft
		int type, state = 0;
		int n90rot = 0, curSurf = 0;
		float timePassed = 0.0f;
		Obstacle(Vei2 tilePos, int type, sharedResC resC)
			:
			tilePos(tilePos),
			type(type),
			resC(std::move(resC))
		{
			switch (type)
			{
			case 1:
			case 4:
				state = 1;
				break;

			}
			n90rot = (tilePos.x + tilePos.y) % 4;
			if (this->resC->tC.obstacles.size() > 0)
			{
				timePassed = (float)((tilePos.x + tilePos.y) % 10) / 10 * this->resC->tC.obstacles[0].GetKeepTime();
			}
		}
		void Draw(Graphics& gfx, RectF tilePos)const
		{
			if (state == 0)
			{
				Vec2 tileSize = Vec2(tilePos.GetWidth(), tilePos.GetHeight());
				tilePos.right += tileSize.x * (Settings::obstacleSizes[type].x - 1);
				tilePos.top -= tileSize.y * (Settings::obstacleSizes[type].y - 1);
				gfx.DrawSurface((RectI)tilePos, resC->tC.obstacles[type].GetSurfaceAt(curSurf), SpriteEffect::Chroma(Colors::Magenta),n90rot);
			}
			else
			{
				Vec2 tileSize = Vec2(tilePos.GetWidth(), tilePos.GetHeight());
				int multiObstIndex = Settings::Obstacle2MultiObstacle(type);
				tilePos += Vec2(Settings::multiObstaclePos[multiObstIndex].x * tileSize.x, -Settings::multiObstaclePos[multiObstIndex].y * tileSize.y);
				Vei2 size = Settings::multiObstacleSize[multiObstIndex][(__int64)state - 1];
				tilePos.right += tileSize.x * (size.x - 1);
				tilePos.top -= tileSize.y * (size.y - 1);
				gfx.DrawSurface((RectI)tilePos, resC->tC.multiObstacles[multiObstIndex].GetSurfaceAt(curSurf), SpriteEffect::Chroma(Colors::Magenta), n90rot);
			}
		}
		void Update(float dt)
		{
			
			timePassed += dt;
			if (state == 0)
			{
				while (timePassed > resC->tC.obstacles[type].GetKeepTime())
				{
					curSurf++;
					timePassed = 0;
				}
				if (curSurf >= resC->tC.obstacles[type].GetNumberOfFrames())
				{
					curSurf = 0;
				}
			}
			if (state == 1)
			{
				while (timePassed > resC->tC.multiObstacles[Settings::Obstacle2MultiObstacle(type)].GetKeepTime())
				{
					curSurf++;
					timePassed = 0;
				}
				if (curSurf >= resC->tC.multiObstacles[Settings::Obstacle2MultiObstacle(type)].GetNumberOfFrames())
				{
					curSurf = 0;
				}
			}
			
		}
	};

	class Cell
	{
	public: 
		Cell() = default;
		Cell(int type):type(type){}
		int type = 0;

		explicit operator int() const 
		{
			return type; 
		}
	};

	class Chunk
	{
		sharedResC resC = nullptr;
		int hasNCells = 0;
		Matrix<Cell> cells;
		std::vector<Matrix<int>> conMap;

	public:
		Chunk() = default;
		Chunk(int hasNCells, Cell defVal, sharedResC resC) :hasNCells(hasNCells), resC(std::move(resC))
		{
			cells = Matrix<Cell>(hasNCells, hasNCells, defVal);
			conMap.clear();
			for (int i = 0; i < Settings::nDiffFieldTypes; i++)
			{
				conMap.push_back(Matrix<int>(hasNCells, hasNCells, 0));
			}
		}
		void DrawType(Vei2 pos, int cellSize, Graphics& gfx)const
		{
			for (int y = 0; y < hasNCells; y++)
			{
				for (int x = 0; x < hasNCells; x++)
				{
					Vei2 curXY = Vei2(x, y);
					const Cell& curCell = cells(curXY);
					int cellType = curCell.type;
					RectI curCellPos = RectI(Vei2(pos.x, pos.y), cellSize, cellSize) + Vei2(x, - y - 1) * cellSize;

					assert(cellType >= 0 && cellType < Settings::nDiffFieldTypes);
					gfx.DrawSurface(curCellPos, RectI(Vei2(0, 0), 50, 50), resC->tC.fields.at(cellType).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
					
					for (int i = 0; i < Settings::nDiffFieldTypes; i++)
					{
						int order = Settings::typeLayer[i];
						if (conMap[order][curXY.x][curXY.y] == 1)
						{
							gfx.DrawConnections(order, Vei2(curCellPos.left, curCellPos.top), cells.GetAroundMatrix(curXY), resC->fsC.FieldCon, resC->tC.fields[order].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						}
					}
				}
			}
		}

		void SetValueAt(Vei2 pos, int type)
		{
			cells(pos) = type;
		}
		int GetTypeAt(Vei2 pos)
		{
			return cells(pos).type;
		}
		void SetConMapAt(Vei2 pos, int type, bool value)
		{
			conMap[type](pos) = value;
		}
	};
	//Resourcen
	std::shared_ptr<ResourceCollection> resC;
	TexturesCollection* tC;
	FramesizeCollection* fsC;
	RandyRandom rng;
	WorldSettings s;
	//Gamevars
	Vei2 wSize = { 0,0 };
	Vei2 cSize = { 0,0 };
	Vei2 chunkSize = {250, 250};
	Vei2 worldHasNChunks;
	Vei2 fCell = { 0,0 };					//angeklickte Zelle
	Vei2 mChunk = { 0,0 };
	Vei2 fTile = { 0,0 };
	
	//Zelle in der Mitte des Bildschirms auf dem Debugzeiger ist
	Vei2 mCunk = { 0,0 };
	Matrix<Cell> cells;
	Matrix<Chunk> chunks;
	std::vector<Matrix<int>> conMap;		//Connectionmap	 (1 = needsConnections, 0 = does not		index for type)
	Matrix<int> groundedMap;				// '0' = spot is not grounded, '1' = is grounded, '-1' = not identified yet (will be 0 if not changed)
	Matrix<int> obstacleMap;				// '-1' = empty   < -1 index of obstacle in obstacleVec
	std::vector<std::unique_ptr<Obstacle>> obstacles;
	
	//std::vector<Team> enemys;
	//Team player;
	
	Vec2& c;								//Camera
	bool grit=false;						//show grit
	bool buildMode = false;					//place something
	int placeObstacle = 0;
	bool posAllowed = true;

	Team player = Team("Die reinlich raeudigen Raucher");
	//Private const Functions
	//RectF GetCellRect(Vei2 cellP)const;
	//RectF GetTileRect(Vei2 tileP)const;
	//Vei2 GetCellHit(Vec2 mP)const;
	//Vei2 GetTileHit(Vec2 mP)const;	

	bool CellIsInWorld(Vei2& pos)const;
	bool TileIsInWorld(Vei2& pos)const;
	bool CellIsInWorldY(int y)const;		
	bool TileIsInWorldY(int y)const;

	bool ChunkIsInWorld(Vei2& cellPos)const;

	Vec2_<Vei2> Cell2ChunkLoc(Vei2 CellPos)const;			//'x' = chunkPos	'y' = cellPos in chunk

	Vei2 PutCellInWorldX(Vei2 pos)const;		//Calculates coordinates when x negativ or > cSize.x  
	Vei2 PutCellInWorldX(int x, int y)const;

	Vei2 PutChunkInWorldX(Vei2 pos)const;	
	Vei2 PutChunkInWorldX(int x, int y)const;

	Vei2 PutTileInWorld(Vei2 pos)const;
	Vei2 PutTileInWorld(int x, int y)const;
	
	Vei2 TileIsInCell(Vei2 tilePos);

	void DestroyObstacleAt(Vei2 tilePos);

	Matrix<int> GetAroundMatrix(Vei2 cell)const;	//in bounds: type		outside bounds(y-wise): -1		
	Matrix<int> GetObstacleAroundMatrix(Vei2 cell)const;
	void UpdateConMap();							//
	void UpdateGroundedMap(Vei2 pos=Vei2(0,0), Vei2 size = Vei2(-1,-1));						// VERY performance heavy - UpdateConMap must be called before UpdateGroundedMap
	void SetTilesAT(Vei2 pos, int value);
	void SetTilesAT(Vei2 pos, Matrix<int> matrix);	//sets tile(x0,y0) in Cell(x1,y1) to type when matrix at(x0,y0) != 0
	bool IsSurroundedBy(Vei2 pos, int type);		//3x3 around pos
	//Private not const Funktions
	void Zoom(Vei2 delta);				//Delta == delta cSize
	void ApplyCameraChanges(Vec2 cDelta);


	void Init(WorldSettings& s);																									// 'ontoType' defines the required Celltype 
	void Generate(WorldSettings& s);																								// 'surrBy' defines the required nearbyCell
	void GenerateCircle(Vei2 pos, int radius, int type, int ontoType = -1, int surrBy = -1); 
	void GenerateLine(Vec2 p0, Vec2 p1, int type, int ontoType = -1, int thickness = 1, int surrBy = -1);
	void GenerateExplosion(Vei2 pos, int maxLineLength, int type, int ontoType = -1, int nRolls = 100, int surrBy = -1);
	bool GenerateCell(Vei2 pos, int type, int ontoType = -1, int surrBy = -1);
	bool FIDF(int first, int second)const;//First is drawn first
	void CutHills(int replaceTo);
	bool ObstaclePosAllowed(Vei2 tilePos, int type);
	
	bool GenerateObstacle(Vei2 tilePos, int type, int ontoType = -1, int surrBy = -1);
	void GenerateObstacleExplosion(Vei2 pos, int maxLineLength, int type, int ontoType = -1, int nRolls = 25, int surrBy = -1);
	void GenerateObstacleLine(Vec2 tile0, Vec2 tile1, int type, int ontoType = -1, int thickness = 1, int surrBy = -1);
	void GenerateObstaclesInCell(Vei2 cellPos, int type, int number, int ontoType = -1, int surrBy = -1);

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
	void UpdateGameLogic(float dt);

	//Grafiken + Einbindung dieser in groundedMap
	void Draw(Graphics& gfx)const;
	void DrawObstacle(Vei2 tilePos, int type, Graphics& gfx, Color color = Colors::Magenta, int frame = -1)const;

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
	Vei2 GetmChunk()const { return mChunk; }
	Team& GetPlayer() { return player; }
};

