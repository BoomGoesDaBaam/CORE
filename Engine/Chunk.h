#pragma once
#include "ResourceCollection.h"
#include "Vec3.h"
#include "Rect.h"
#include "GigaMath.h"
#include "Obstacle.h"
class Cell
{
public:
	Cell() = default;
	Cell(int type) :type(type) {}
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
	Matrix<Chunk>* chunks = nullptr;
	Vei2 chunkPos = Vei2(-1, -1);
	std::vector<Matrix<int>> conMap;		//Connectionmap	 (1 = needsConnections, 0 = does not		index for type)
	Matrix<int> groundedMap;				// '0' = spot is not grounded, '1' = is grounded, '-1' = not identified yet (will be 0 if not changed), '2' = hill
	Matrix<Matrix<int>> aMats;
	Matrix<int> obstacleMap;				// '-1' = empty   val > -1 = index of obstacle in obstacleVec
	std::vector<Obstacle> obstacles;
	Surface surf_types;
	Surface surf_obstacles;

	void SetTilesAT(Vei2 pos, int value);
	void SetTilesAT(Vei2 pos, Matrix<int> matrix);
	void PlaceConnectionsIntoCelltiles(Vei2 pos, int value, int valOfMixed, int valueOfZero, const int* types);
	void PlaceTilesForMaskedField(Vei2 pos, int value, int valOfMixed, int valueOfZero, int type);
	void ChangeGroundedVal(int from, int to);
public:
	Chunk() = default;
	Chunk(int hasNCells, Cell defVal, Matrix<Chunk>* chunks, sharedResC resC)
		:
		hasNCells(hasNCells),
		resC(std::move(resC)),
		chunks(chunks),
		cells(Matrix<Cell>(hasNCells, hasNCells, defVal)),
		obstacleMap(Matrix<int>(hasNCells* Settings::CellSplitUpIn, hasNCells* Settings::CellSplitUpIn, -1))
	{
		conMap.clear();
		for (int i = 0; i < Settings::nDiffFieldTypes; i++)
		{
			conMap.push_back(Matrix<int>(hasNCells, hasNCells, 0));
		}
	}
	/*
	bool GenerateObstacleAt(Vei2 tilePos, int type, int ontoCellType = -1, int ontoGroundedTile = -1, int ontoObstacle = -1, int surrByCellType = -1, int surrByGroundedTile = -1, int surrByObstacle = -1)
	{
		if (Settings::obstaclesOn)
		{
			//auto a = AdjustTilePos(tilePos);
			VecN rPos = ChunkSwitchIsNeeded(tilePos);
			if ((Vei2)rPos[0] != Vei2(-1, -1))
			{
				return chunks->operator()(rPos[0]).GenerateObstacleAt((Vei2)rPos[1] * Settings::CellSplitUpIn + (Vei2)rPos[2], type, ontoCellType, ontoGroundedTile, ontoObstacle, surrByCellType, surrByGroundedTile, surrByObstacle);
			}
			rPos = AdjustTilePos(tilePos);

			//assert(cells.GetColums() < (Vei2)(rPos[1]).x && cells.GetRows() < (Vei2)(rPos[1]).y);
			if ((ontoCellType == -1 || ontoCellType == cells(rPos[1]).type) &&
				//(ontoGroundedTile == -1 || ontoGroundedTile == groundedMap(tilePos)) &&
				(ontoObstacle == -1 || obstacleMap(rPos[0]) != -1 && ontoObstacle == obstacles[obstacleMap(rPos[0])].type)
				//(surrByCellType == -1 || surrBy == groundedMap(tilePos))

				)
			{
				int index = obstacles.size();
				obstacles.push_back(Obstacle(tilePos,chunkPos, type, resC));
				assert(tilePos.x >= 0 && tilePos.x < 125);
				MarkObstacleMap(tilePos, Settings::obstacleSizes[type], index);
			}
		}
		else
		{
			return true;
		}
		return false;
	}
	*/
	void MarkObstacleMap(Vei2 tilePos, Vei2 size, int index);
	void MoveObstacle(int index, Vei2 newPos);
	VecN AdjustTilePos(Vei2 tilePos) const;
	VecN ChunkSwitchIsNeeded(Vei2 tilePos) const;
	bool ObstaclePosAllowed(Vei2 tilePos, int type, int except = -1) const;
	bool ObstaclePosAllowed(Vei2 tilePos, Vei2 size, int except = -1) const;

	Vec2 GetCellSize(RectF chunkRect) const;
	Vec2 GetTileSize(RectF chunkRect) const;
	RectF GetCellRect(RectF chunkRect, Vei2 cellPos) const;
	RectF GetTileRect(RectF chunkRect, Vei2 tilePos) const;

	void DrawObstacleOutlines(Vei2 tilePos, int type, RectF chunkRect, Color c, Graphics& gfx) const;
	void DrawObstacle(Vei2 tilePos, int type, RectF chunkRect, Graphics& gfx) const;
	void DrawObstacles(RectF chunkRect, Graphics& gfx) const;
	void DrawType(RectF chunkRect, Graphics& gfx)const;
	void DrawGroundedMap(Vei2 pos, int cellSize, Graphics& gfx)const;
	void DrawGrit(Vei2 pos, int cellSize, Graphics& gfx)const;
	void DrawSurfaceAt(Vei2 drawPos, Vei2 cellPos, int cellSize, float scale, const Surface& s, Graphics& gfx)const;
	void DrawTile(RectF chunkRect, Vei2 tilePos, Color c, Graphics& gfx)const;
	
	void UpdateTypeSurface(RectF chunkRect);

	void UpdateAroundMatrix(Matrix<int> mat);
	void UpdateGroundedMap();
	void Update(float dt);

	bool PlaceObstacle(Vei2 tilePos, int type);
	bool TileIsInChunk(Vei2& pos)const;
	Vei2 PutTileInChunk(Vei2 pos)const;
	Vei2 PutTileInChunk(int x, int y)const;

	Vec2_<Vei2> GetTilePosOutOfBounds(Vei2 tilePos) const;

	int GetGroundedOutOfBounds(Vei2 tilePos) const;
	int GetObstacleOutOfBounds(Vei2 tilePos) const;
	void SetTypeAt(Vei2 pos, int type);

	void SetConMapAt(Vei2 pos, int type, bool value);
	int GetCellTypeAt(Vei2 pos)const;
	int GetGrounedMapAt(Vei2 pos)const;
	int GetObstacleMapAt(Vei2 pos)const;
	Obstacle* GetObstacleAt(Vei2 pos);
	Matrix<int> GetAroundmatrix(Vei2 pos)const;
	void SetChunkPos(Vei2 pos);
	void PlaceLadderableTiles(int type);
	Cell& GetCellAt(Vei2 cellPos);

	static Vei2 PutChunkInWorld(Vei2 pos, Vei2 worldHasNChunks)
	{
		return PutChunkInWorld(pos.x, pos.y, worldHasNChunks);
		//return Vei2(pos.x % worldHasNChunks.y, pos.y % worldHasNChunks.y);
	}
	static Vei2 PutChunkInWorld(int x, int y, Vei2 worldHasNChunks)
	{
		return Vei2(GigaMath::NegMod(x, worldHasNChunks.x), GigaMath::NegMod(y, worldHasNChunks.y));
	}
};

