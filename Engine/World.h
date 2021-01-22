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
#include "Vec3.h"
#include "Chunk.h"
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
			worldHasNChunks = Vei2(3, 3);
#endif
			wSize = Vei2(worldHasNChunks.x * Settings::chunkHasNCells, worldHasNChunks.y * Settings::chunkHasNCells);
		}
		int defBlueprint = 0;
		int defType = 0;
		Vei2 wSize;
		Vei2 worldHasNChunks = Vei2(25, 25);
		Vei2 chunkSize = { 250, 250 };
	};
private:
	
	//Resourcen
	std::shared_ptr<ResourceCollection> resC;
	TexturesCollection* tC;
	FramesizeCollection* fsC;
	RandyRandom rng;
	WorldSettings s;
	//Gamevars
	Vei2 mChunk = { 0,0 };
	Vei2 fTile = { 0,0 };

	Vec3_<Vei2> fcctPos = Vec3_<Vei2>(Vei2(0, 0), Vei2(0, 0), Vei2(0, 0));
	Vec3_<Vei2> fcctPosHover = Vec3_<Vei2>(Vei2(0, 0), Vei2(0, 0), Vei2(0, 0));

	Obstacle* focusedObst = nullptr;

	Vec2& c;								//Camera

	//Zelle in der Mitte des Bildschirms auf dem Debugzeiger ist
	Vei2 mCunk = { 0,0 };
	//Matrix<Cell> cells;
	Matrix<Chunk> chunks;
	
	Matrix<int> obstacleMap;				// '-1' = empty   < -1 index of obstacle in obstacleVec
	
	std::vector<std::unique_ptr<Obstacle>> obstacles;
	
	//std::vector<Team> enemys;
	//Team player;

	bool grit = false;						//show grit
	bool buildMode = false;					//place something
	int placeObstacle = 0;
	bool posAllowed = true;
	bool moveMode = false;
	int moveRange = 4;

	Team player = Team("Die reichlich raeudigen Raucher");
	Team enemie1 = Team("In dem Sinne");
	Team enemie2 = Team("Nichts wie Zeg");
	Team enemie3 = Team("Was geht!");

	//Mouse calculations
	Vei2 GetChunkHit(Vec2 mP)const;
	Vec3_<Vei2> GetHitTile(Vec2 mP)const;

	//Tile operations
	bool TileIsInRange(CctPos tPos1, CctPos tPos2, float range);

	//Graphic calculations
	RectF GetChunkRect(Vei2 pos)const;

	//Tests and corrections
	bool CellIsInWorld(Vei2& pos)const;
	bool TileIsInWorld(Vei2& pos)const;
	bool CellIsInWorldY(int y)const;		
	bool TileIsInWorldY(int y)const;
	bool ChunkIsInWorld(Vei2& cellPos)const;


	Vei2 PutCellInWorldX(Vei2 pos)const;					//Calculates coordinates when x negativ or > cSize.x  
	Vei2 PutCellInWorldX(int x, int y)const;
	Vei2 PutTileInWorld(Vei2 pos)const;
	Vei2 PutTileInWorld(int x, int y)const;
	Vec3_<Vei2> PutCctPosInWorld(CctPos cctPos)const;

	Vei2 AbstractTilePos(CctPos chunkPos)const;
	//Transformation between chunk and flat
	Vec2_<Vei2> Cell2ChunkPos(Vei2 CellPos)const;			//	'x' = chunkPos	'y' = cellPos in chunk
	Vec3_<Vei2> Tile2ChunkPos(Vei2 tilePos)const;			//	1x3 Matrix		'0' = chunkPos		'1' = cellPos		'2' = tilePos
	Vei2 chunkPos2Flat(Vec3_<Vei2> cctPos)const
	{
		return cctPos.x * Settings::CellSplitUpIn * Settings::chunkHasNCells + cctPos.y * Settings::CellSplitUpIn + cctPos.z;
	}
	int ObstacleMapAt(Vei2 tilePos)const;
	int ObstacleMapAt(Vec3_<Vei2> tilePos)const;
	int GroundedMapAt(Vei2 tilePos)const;

	Obstacle* GetObstacleAt(Vec3_<Vei2> tilePos);

	Vei2 TileIsInCell(Vei2 tilePos);
	//Init
	void DestroyObstacleAt(Vei2 tilePos);
	void UpdateConMap();							//
	void UpdateGroundedMap(Vei2 pos=Vei2(0,0), Vei2 size = Vei2(-1,-1));						// VERY performance heavy - UpdateConMap must be called before UpdateGroundedMap
	void SpawnUnits(int n, Team& inTeam, Vei2 tilePos);
	void SpawnPlayer();
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
	bool ObstaclePosAllowed(Vei2 tilePos, int type);
	
	bool GenerateObstacle(Vei2 tilePos, int type, int ontoType = -1, int surrBy = -1);
	void GenerateObstacleExplosion(Vei2 pos, int maxLineLength, int type, int ontoType = -1, int nRolls = 25, int surrBy = -1);
	void GenerateObstacleLine(Vec2 tile0, Vec2 tile1, int type, int ontoType = -1, int thickness = 1, int surrBy = -1);
	void GenerateObstaclesInCell(Vei2 cellPos, int type, int number, int ontoType = -1, int surrBy = -1);

public:
																							//Konstruktor + Operatoren
	World(WorldSettings wSettings, std::shared_ptr<ResourceCollection> resC, Vec2& camera);
	
	//Handles
	void HandleMouseEvents(Mouse::Event& e, GrabHandle& gH);
	void HandleKeyboardEvents(Keyboard::Event& e);
	void UpdateGameLogic(float dt);

	//Grafiken + Einbindung dieser in groundedMap
	void Draw(Graphics& gfx)const;
	void DrawObstacle(Vei2 tilePos, int type, Graphics& gfx, Color color = Colors::Magenta, int frame = -1)const;

	std::vector<SubAnimation> GetConnectionsOfTypes(Vei2 pos, int* types);
	bool NeedsConnections(Vei2 curXY)const;
	void SetBuildMode(int obstacle);
	void SetMoveMode(bool moveMode)
	{
		if (focusedObst != nullptr)
		{
			this->moveMode = moveMode;
		}
	}
	//
	Vei2 GetwSize()const { return s.wSize; }
	Vei2 GetcSize()const { return s.chunkSize / Settings::chunkHasNCells; }
	Vec2 GetTileSize()const { return (Vec2)GetcSize()/Settings::CellSplitUpIn; }
	Vei2 GetfCell()const { return Vei2(-1,-1); }
	Vei2 GetfTile()const { return fTile; }
	int GetChunksDrawnToLeft()const { return -(Graphics::GetMidOfScreen().x / s.chunkSize.x) * 2 - 1; }
	int GetfCellType()const { return chunks(fcctPos.x).GetCellTypeAt(fcctPos.y); }
	Vei2 GetmChunk()const { return mChunk; }
	Team& GetPlayer() { return player; }
	RectI GetRenderRect()const { 
		auto mos = Graphics::GetMidOfScreen();
		return RectI(-1 - (mos.x / s.chunkSize.x) * 1.5f, 1 + (mos.x / s.chunkSize.x) * 1.5f, -1 - (mos.y / s.chunkSize.y) * 1.5f, 1 + (mos.y / s.chunkSize.y) * 1.5f); };
	Obstacle* GetFocusedObstacle()
	{
		return focusedObst;
	}
};

