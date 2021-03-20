#pragma once
#include "ResourceCollection.h"
#include "Vec3.h"
#include "Rect.h"
#include "GigaMath.h"
#include "Team.h"
class Obstacle
{
protected:
	sharedResC resC;
public:
	Vei2 tilePos;						//pos in chunk		Vei2(-1,-1) == disabled
	Vei2 chunkPos;
	int type, state = 0;
	int n90rot = 0;
	int hp=50;
	int stepsLeft = 0;
	Team* team = nullptr;
	std::vector<Animation> animations;	//index runs through states
	Obstacle(Vei2 tilePos, Vei2 chunkPos, int type, sharedResC resC, Team* team = nullptr)
		:
		tilePos(tilePos),
		chunkPos(chunkPos),
		type(type),
		resC(std::move(resC)),
		team(team),
		stepsLeft(Settings::obstacleMovesPerTurn[type])
	{
		hp = Settings::obstacleBaseHP[type];
		animations.push_back(Animation(this->resC->tC.obstacles[type]));
		switch (type)
		{
		case 1:
		case 4:
			state = 1;
			animations.push_back(Animation(this->resC->tC.multiObstacles[Settings::Obstacle2MultiObstacle(type)]));
			break;

		}
		n90rot = (tilePos.x + tilePos.y) % 4;
	}
	void Draw(RectF tileRect, Vei2 tilePos, RectF chunkRect, Graphics& gfx)const			//	'tileRect' = Rect of tile where (Vei2(0, -1) && Vei2(-1, 0) != index) == true
	{
		if (state == 0)
		{
			Vec2 tileSize = Vec2(tileRect.GetWidth(), tileRect.GetHeight());
			tileRect.right += tileSize.x * (Settings::obstacleSizes[type].x - 1);
			tileRect.top -= tileSize.y * (Settings::obstacleSizes[type].y - 1);
			gfx.DrawSurface((RectI)tileRect, animations[0].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
		}
		else
		{
			Vec2 tileSize = Vec2(tileRect.GetWidth(), tileRect.GetHeight());
			int multiObstIndex = Settings::Obstacle2MultiObstacle(type);
			tileRect += Vec2(Settings::multiObstaclePos[multiObstIndex].x * tileSize.x, -Settings::multiObstaclePos[multiObstIndex].y * tileSize.y);
			Vei2 size = Settings::multiObstacleSize[multiObstIndex][(__int64)state - 1];
			tileRect.right += tileSize.x * (size.x - 1);
			tileRect.top -= tileSize.y * (size.y - 1);
			gfx.DrawSurface((RectI)tileRect, animations[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta), 0);
		}
	}
	void Update(float dt)
	{
		for (auto animation : animations)
		{
			animation.Update(dt);
		}
	}
	void GetRect(Vei2 chunkSize, Vei2 midChunk, Vei2 c)
	{
		Vei2 delta = midChunk - chunkPos;
		Vei2 tileSize = chunkSize / Settings::chunkHasNCells * Settings::CellSplitUpIn;

	}
};
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
	std::vector<int> obstaclesIndexNotUsed;
	Surface surf_typesAndObstacles;
	int graphicsWidth = 0;
	int graphicsUpToDate = 0;				// 0 = nothing		1 = types drawn		2 = obstacles + types


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
	bool MoveObstacle(int index, Vec3_<Vei2> newPos);
	bool ObstaclePosAllowed(Vei2 tilePos, int type, int except = -1) const;
	bool ObstaclePosAllowed(Vei2 tilePos, Vei2 size, int except = -1) const;
	void DeleteObstacle(Vei2 tilePos);
	bool CheckIfObstacleBaseTile(Vei2 tilePos);
	int GetObstacleIndex(Vei2 tilePos)const;

	Vec2 GetCellSize(RectF chunkRect) const;
	Vec2 GetTileSize(RectF chunkRect) const;
	RectF GetCellRect(RectF chunkRect, Vei2 cellPos) const;
	RectF GetTileRect(RectF chunkRect, Vei2 tilePos) const;
	static Vei2 chunkPos2Flat(Vec3_<Vei2> cctPos)
	{
		return cctPos.x * Settings::CellSplitUpIn * Settings::chunkHasNCells + cctPos.y * Settings::CellSplitUpIn + cctPos.z;
	}
	static double GetDistBetween2tiles(Vei2 tilePos1, Vei2 tilePos2, int worldWidth)
	{
		Vec2 delta = (Vec2)tilePos1 - (Vec2)tilePos2;
		if (delta.x > worldWidth / 2)
		{
			delta.x -= worldWidth;
		}
		if (delta.x < -worldWidth / 2)
		{
			delta.x += worldWidth;
		}
		return (double)std::abs(sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
	}
	void DrawObstacleOutlines(Vei2 tilePos, int type, RectF chunkRect, Color c, Graphics& gfx) const;
	void DrawObstacle(Vei2 tilePos, int type, RectF chunkRect, Graphics& gfx) const;
	void DrawObstacles(RectF chunkRect, Graphics& gfx) const;
	void DrawTypeAndObst(RectF chunkRect, Graphics& gfx)const;
	void DrawGroundedMap(Vei2 pos, int cellSize, Graphics& gfx)const;
	void DrawGrit(Vei2 pos, int cellSize, Graphics& gfx)const;
	void DrawSurfaceAt(Vei2 drawPos, Vei2 cellPos, int cellSize, float scale, const Surface& s, Graphics& gfx)const;
	void DrawTile(RectF chunkRect, Vei2 tilePos, Color c, Graphics& gfx)const;
	
	void UpdateTypeSurface(RectF chunkRect);
	void UpdateObstacleSurface(RectF chunkRect);
	void DrawObstacleProtrud(RectF curRect, Vec2 chunkSize, const Surface& s);
	void DrawObstacleOnBuffer(RectF curRect,const Surface& s);

	void UpdateAroundMatrix(Matrix<int> mat);
	void UpdateGroundedMap();
	void Update(float dt);
	int NeedGraphicsUpdate(Vei2 chunkSize);
	void UpdateGraphics();
	void UpdateWhenMoved();
	bool UnitIsAround(Vei2 tilePos, int range);
	CtPos PutCtPosInWorld(CtPos ctPos, Vei2 worldHasNChunks);

	bool PlaceObstacle(Vei2 tilePos, int type, Team* team = nullptr);
	bool Chunk::PlaceObstacle(Vei2 tilePos, Obstacle o);
	bool TileIsInChunk(Vei2& pos)const;
	Vei2 PutTileInChunk(Vei2 pos)const;
	Vei2 PutTileInChunk(int x, int y)const;

	Vec2_<Vei2> GetTilePosOutOfBounds(Vei2 tilePos) const;

	int GetGroundedOutOfBounds(Vei2 tilePos) const;
	int GetObstacleOutOfBounds(Vei2 tilePos) const;
	void SetTypeAt(Vei2 pos, int type);
	void NextTurn();

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

