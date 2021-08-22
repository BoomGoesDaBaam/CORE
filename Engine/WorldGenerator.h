#pragma once
#include "RandyRandom.h"
#include "ResourceCollection.h"
#include "Chunk.h"
#include "Settings.h"
class WorldGenerator
{
	Matrix<Chunk>& chunks;
	const Settings::WorldSettings& settings;
	std::map<std::string, Team>* teams;
	RandyRandom rng;
	sharedResC resC;

	Vei2 mChunk = { 0,0 };
	Vec2 c = Vec2(0,0);

	//surrBy just available after calling UpdateConMap and UpdateGroundedMap!!!
	void GenerateCircle(Vei2 pos, int radius, int type, int ontoType = -1, int surrBy = -1);
	void GenerateLine(Vec2 p0, Vec2 p1, int type, int ontoType = -1, int thickness = 1, int surrBy = -1);	//Points arent allowed to cross worldrecusionLine!!!
	void GenerateExplosion(Vei2 pos, int maxLineLength, int type, int ontoType = -1, int nRolls = 30, int surrBy = -1);
	bool GenerateCell(Vei2 pos, int type, int ontoType = -1, int surrBy = -1);
	void GenerateExplosionRect(RectI sprayPos, int maxLineLength, int type, int nRolls=30);
	std::vector<Vei2> GenerateRandomExplosions(RectI bounds, int maxLineLength, int type, float density, int ontoType = -1, int nRolls = 30, int surrBy = -1);
	void GenerateExplosions(std::vector<Vei2> spawnPoints, int maxLineLength, int type, int ontoType = -1, int nRolls = 30, int surrBy = -1);

	void GenerateObstaclesOnRandomPosition(RectI bounds, int type, float density, std::vector<int> ontoTypes, Team* team = nullptr);
	void GenerateObstaclegroupOnRandomPosition(RectI bounds, int type, float density, std::vector<int> ontoTypes, Team* team);
	CtPos SpawnPlayer(Vei2 tilePos);
	void InitEnemies();
	void SpawnEnemie(int spawnCode, Team* team, RectI bounds);

	std::vector<Obstacle*> GenerateObstacleExplosion(Vei2 pos, int nMax, int maxLineLength, int type, Team* team = nullptr, int ontoType = -1, int nRolls = 25, int surrBy = -1);
	void GenerateObstacleLine(Vec2 tile0, Vec2 tile1, int type, Team* team = nullptr, int ontoType = -1, int thickness = 1, int surrBy = -1);
	void GenerateObstaclesInCell(Vei2 cellPos, int type, int number, Team* team = nullptr, int ontoType = -1, int surrBy = -1);


	std::vector<Obstacle*> SpawnUnits(int n, int type, Team* inTeam, Vei2 tilePos);
	CtPos SpawnTeam(Team* team, Vei2 circaSpawnPoint);
	void SpawnUnitGroup(Vei2 circaTilePos, int type, Team* team, int n);
	bool IsSurroundedBy(Vei2 pos, int type);		//3x3 around pos

	void InitChunks();
public:
	WorldGenerator(Matrix<Chunk>& chunks, sharedResC resC, const Settings::WorldSettings& settings,std::map<std::string, Team>* teams);

	void Generate(const Settings::WorldSettings& settings);

	void UpdateConMap();							//
	void UpdateGroundedMap(Vei2 pos = Vei2(0, 0), Vei2 size = Vei2(-1, -1));						// VERY performance heavy - UpdateConMap must be called before UpdateGroundedMap

	Vei2 GetMChunk()
	{
		return mChunk;
	}
	Vec2 GetCamera()
	{
		return c;
	}
	static Vei2 PutCellInWorld(Vei2 v, Vei2 wSizeInCells)
	{
		return PutCellInWorld(v.x, v.y, wSizeInCells);
	}
	static Vei2 PutCellInWorld(int x, int y, Vei2 wSizeInCells)
	{
		if (x < 0)
		{
			x = -x;
			x %= wSizeInCells.x;
			x = wSizeInCells.x - x;
		}
		if (x >= wSizeInCells.x)
		{
			x = x % wSizeInCells.x;
		}

		if (y < 0)
		{
			y = 0;
		}
		if (y >= wSizeInCells.y)
		{
			y = wSizeInCells.y - 1;
		}
		return Vei2(x, y);
	}
	static Vec2_<Vei2> Cell2ChunkPos(Vei2 cellPos)
	{
		return Vec2_<Vei2>(cellPos / Settings::chunkHasNCells, cellPos % Settings::chunkHasNCells);
	}
};

