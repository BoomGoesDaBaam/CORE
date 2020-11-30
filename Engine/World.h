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
			wSize = Vei2(worldHasNChunks.x * chunkHasNCells, worldHasNChunks.y * chunkHasNCells);
		}
		int defBlueprint = 0;
		int defType = 0;
		Vei2 wSize;
		int chunkHasNCells = 5;
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
	Vec2_<Vei2> fccPos = { {0,0},{0,0} };

	Vec3_<Vei2> fcctPos = Vec3_<Vei2>(Vei2(10, 12), Vei2(1, 1), Vei2(1, 1));

	Vec2& c;								//Camera

	//Zelle in der Mitte des Bildschirms auf dem Debugzeiger ist
	Vei2 mCunk = { 0,0 };
	//Matrix<Cell> cells;
	Matrix<Chunk> chunks;
	
	std::vector<Matrix<int>> conMap;		//Connectionmap	 (1 = needsConnections, 0 = does not		index for type)
	Matrix<int> groundedMap;				// '0' = spot is not grounded, '1' = is grounded, '-1' = not identified yet (will be 0 if not changed), '2' = hill
	Matrix<int> obstacleMap;				// '-1' = empty   < -1 index of obstacle in obstacleVec
	
	std::vector<std::unique_ptr<Obstacle>> obstacles;
	
	//std::vector<Team> enemys;
	//Team player;

	bool grit = false;						//show grit
	bool buildMode = false;					//place something
	int placeObstacle = 0;
	bool posAllowed = true;

	Team player = Team("Die reinlich raeudigen Raucher");

	Vei2 GetChunkHit(Vec2 mP)const;
	RectF GetChunkRect(Vei2 pos)const;

	Vec3_<Vei2> GetHitTile(Vec2 mP)const;

	bool CellIsInWorld(Vei2& pos)const;
	bool TileIsInWorld(Vei2& pos)const;

	bool CellIsInWorldY(int y)const;		
	bool TileIsInWorldY(int y)const;

	bool ChunkIsInWorld(Vei2& cellPos)const;


	Vec2_<Vei2> Cell2ChunkPos(Vei2 CellPos)const;			//	'x' = chunkPos	'y' = cellPos in chunk
	Vec3_<Vei2> Tile2ChunkPos(Vei2 tilePos)const;	//	1x3 Matrix		'0' = chunkPos		'1' = cellPos		'2' = tilePos


	Vei2 PutCellInWorldX(Vei2 pos)const;		//Calculates coordinates when x negativ or > cSize.x  
	Vei2 PutCellInWorldX(int x, int y)const;

	Vei2 PutTileInWorld(Vei2 pos)const;
	Vei2 PutTileInWorld(int x, int y)const;
	
	Vei2 TileIsInCell(Vei2 tilePos);

	void DestroyObstacleAt(Vei2 tilePos);
	Matrix<int> GetObstacleAroundMatrix(Vei2 cell)const;
	void UpdateConMap();							//
	void UpdateGroundedMap(Vei2 pos=Vei2(0,0), Vei2 size = Vei2(-1,-1));						// VERY performance heavy - UpdateConMap must be called before UpdateGroundedMap
	//void SetTilesAT(Vei2 pos, int value);
	//void SetTilesAT(Vei2 pos, Matrix<int> matrix);	//sets tile(x0,y0) in Cell(x1,y1) to type when matrix at(x0,y0) != 0
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
		return -(mos.x / GetcSize().x) * 2;
	};
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
	//
	Vei2 GetwSize()const { return s.wSize; }
	Vei2 GetcSize()const { return s.chunkSize / s.chunkHasNCells; }
	Vec2 GetTileSize()const { return (Vec2)GetcSize()/Settings::CellSplitUpIn; }
	Vei2 GetfCell()const { return Vei2(-1,-1); }
	Vei2 GetfTile()const { return fTile; }
	int GetChunksDrawnToLeft()const { return -(Graphics::GetMidOfScreen().x / s.chunkSize.x) * 2 - 1; }
	int GetfCellType()const { return chunks(fccPos.x).GetCellTypeAt(fccPos.y); }
	Vei2 GetmChunk()const { return mChunk; }
	Team& GetPlayer() { return player; }
	RectI GetRenderRect()const { 
		auto mos = Graphics::GetMidOfScreen();
		return RectI(-1 - (mos.x / s.chunkSize.x) * 1.5f, 1 + (mos.x / s.chunkSize.x) * 1.5f, -1 - (mos.y / s.chunkSize.y) * 1.5f, 1 + (mos.y / s.chunkSize.y) * 1.5f); };
};

