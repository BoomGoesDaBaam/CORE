#pragma once
#include "ResourceCollection.h"
#include "Vec3.h"
#include "Rect.h"
#include "GigaMath.h"
#include "Team.h"
#include <memory>
class EducationTrait
{
protected:
	int turn2Educate = 0, turn2EducateBase, educatesType;
	bool educates = false;
public:
	EducationTrait(int turn2Educate, int educatesType)
		:
		turn2Educate(turn2Educate),
		turn2EducateBase(turn2Educate),
		educatesType(educatesType)
	{}
	void TurnPassed()
	{
		if(educates)
			turn2Educate--;
	}
	bool EducationFinished()
	{
		return turn2Educate <= 0;
	}
	void ResetTurns()
	{
		turn2Educate = turn2EducateBase;
	}
	int GetFinishedType()
	{
		return educatesType;
	}
	void SetEducates(bool educates)
	{
		this->educates = educates;
	}
	bool Educates()
	{
		return educates;
	}
};
class HealTrait
{
protected:
	bool heals = false;
public:
	HealTrait(int heals)
		:
		heals(heals)
	{}
	void Enable()
	{
		heals = true;
	}
	void Disable()
	{
		heals = false;
	}
	bool Isenabled()
	{
		return heals;
	}
};
class CraftTrait
{
protected:
	int itemID = -1;
	float turnsLeft = -1;
	bool isCrafting = false;
public:
	CraftTrait() = default;
	void SetItem2Craft(int itemID)
	{
		this->itemID = itemID;
		turnsLeft = Settings::itemStats[itemID].turns2Craft;
		isCrafting = true;
	}
	int TurnsLeft(float productivity)
	{
		return (int)std::ceil(turnsLeft / productivity);
	}
	void TurnPassed(float productivity)
	{
		if (turnsLeft > 0)
		{
			turnsLeft -= productivity;
			if (turnsLeft <= 0)
			{
				turnsLeft = 0;
			}
		}
	}
	int GetItemID()
	{
		return itemID;
	}
	bool IsCrafting()
	{
		return isCrafting;
	}
	void StoppedCrafting()
	{
		isCrafting = false;
	}
};
class AttackTrait
{
protected:
	int attacksLeft = 0;
	bool reloadNextTurn = true;
	CtPos automaticMode = CtPos(Vei2(-1,-1), Vei2(-1, -1));		//CtPos(Vei2(-1,-1), Vei2(-1, -1)) == not attacking automatic
																//CtPos(Vei2(-2,-2), Vei2(-2, -2)) == need new location to attack automatic
public:
	AttackTrait(int attacksLeft)
		:
		attacksLeft(attacksLeft)
	{}
	CtPos GetAutomaticMode()
	{
		return automaticMode;
	}
	void SetAutomaticMode(CtPos automaticMode)
	{
		this->automaticMode = automaticMode;
	}
	bool GetReloadNextTurn()
	{
		return reloadNextTurn;
	}
	void SetReloadNextTurn(bool reloadNextTurn)
	{
		this->reloadNextTurn = reloadNextTurn;
	}
	void SetAttacksleft(int attacksLeft)
	{
		this->attacksLeft = attacksLeft;
	}
	int GetAttacksLeft()
	{
		return attacksLeft;
	}
	void Attacked()
	{
		attacksLeft--;
	}
};
class Obstacle
{
protected:
	sharedResC resC;
	Vei2 tilePos = Vei2(-1, -1);						//pos in chunk		Vei2(-1,-1) == disabled
	Vei2 chunkPos = Vei2(-1, -1);
	int type;
	int state = 0;
	int n90rot = 0;
	int hp = 50;
	int stepsLeft = 0;
	float productivity = 1.0f;
	Team* team = nullptr;
	RectF rect = RectF(Vec2(0, 0), 0, 0);			//pos where Obstacle is drawn on screen
	std::vector<Animation> animations;				//index runs through states
	//Traits		new Trait need handle in constructor and in COPY ASSIGNMENT!
	std::unique_ptr<EducationTrait> education = nullptr;
	std::unique_ptr<HealTrait> heal = nullptr;
	std::unique_ptr<AttackTrait> attack = nullptr;
	std::unique_ptr<CraftTrait> craft = nullptr;
	//
	std::unique_ptr<Inventory> inv = nullptr;
public:
	//Obstacle() {}							//no args constructor not needed
	//Obstacle(Obstacle&& obst) {}			//move constructor not needed
	/*
	Obstacle& operator=(Obstacle&& other) {	//move assignment not needed

		return *this;
	}
	Obstacle::~Obstacle() {};				//destructor not needed
	*/

	Obstacle& operator=(const Obstacle& other);
	Obstacle(const Obstacle& obst);
	Obstacle(Vei2 tilePos, Vei2 chunkPos, int type, sharedResC resC, Team* team = nullptr);
	void Heal(int deltaHP);
	int GetHealRange();
	int GetHealAmount();
	void Draw(Graphics& gfx)const;
	void UpdateRect(RectF tileRect, Vei2 tilePos, RectF chunkRect);
	void Update(float dt);
	CtPos GetCtPos();
	Vei2 GetPosFlat();
	CctPos GetCctPos();
	int GetDmg(const Obstacle* victim);
	int GetAttackRange();
	float AttackObstacle(Obstacle* attacker, float dmg, int range);
	
	//Getter
	Vei2 GetChunkPos()const;
	Vei2 GetTilePos()const;
	int GetType()const;
	int GetState()const;
	int GetN90Rot()const;
	int GetHP()const;
	int GetStepsLeft()const;
	float GetProductivity()const;
	Team* GetTeam()const;
	RectF GetRect()const;
	Inventory* GetInventory()const;
	//Traits
	EducationTrait* GetEducation()const;
	HealTrait* GetHealTrait()const;
	AttackTrait* GetAttackTrait()const;
	CraftTrait* GetCraftTrait()const;


	//Setter
	void SetChunkPos(Vei2 pos);
	void SetTilePos(Vei2 pos);
	void SetN90Rot(int n90rot);
	void SetState(int state);
	void SetHp(int hp);
	void AddHP(int delta);
	void SetStepsLeft(int stepsLeft);
	void AddStepsLeft(int delta);
	void SetProductivity(float stepsLeft);
	void AddProductivity(float delta);
	void SetRect(RectF rect);

	virtual void Poop() {}
};

class ConstructionSite : public Obstacle
{
	int typeWhenFinished;
	float turnsLeft;
public:
	ConstructionSite(float turnsLeft, int typeWhenFinished, Vei2 tilePos, Vei2 chunkPos, int type, sharedResC resC, Team* team = nullptr)
		:
		Obstacle( tilePos, chunkPos, type, resC, team),
		turnsLeft(turnsLeft),
		typeWhenFinished(typeWhenFinished)
	{

	}
	virtual void Poop()override {}
	void TurnPassed()
	{
		turnsLeft-=productivity;
	}
	bool BuildingFinished()
	{
		return turnsLeft <= 0;
	}
	int GetTypeWhenFinished()
	{
		return typeWhenFinished;
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
	Matrix<RectF> cellsRect;
	Matrix<RectF> tilesRect;

	Matrix<Chunk>* chunks = nullptr;
	Vei2 chunkPos = Vei2(-1, -1);
	std::vector<Matrix<int>> conMap;		//Connectionmap	 (1 = needsConnections, 0 = does not		index for type)
	Matrix<int> groundedMap;				// '0' = spot is not grounded(water), '1' = is grounded, '-1' = not identified yet (will be 0 if not changed), '2' = hill
	Matrix<Matrix<int>> aMats;
	Matrix<int> obstacleMap;				// '-1' = empty   val > -1 = index of obstacle in obstacleVec

	std::vector<std::unique_ptr<Obstacle>> obstacles;
	std::vector<int> obstaclesIndexNotUsed;
	RandyRandom rr;
	//int graphicsWidth = 0;
	//int graphicsUpToDate = 0;				// 0 = nothing		1 = types drawn		2 = obstacles + types
	//new values have to be added in copy assignment!!!

	void SetTilesAT(CtPos CtPos, int value);
	void SetTilesAT(CtPos CtPos, Matrix<int> matrix);
	void PlaceConnectionsIntoCelltiles(Vei2 cellPos, int value, int valOfMixed, int valueOfZero, const int* types);
	void PlaceTilesForMaskedField(Vei2 cellPos, int value, int valOfMixed, int valueOfZero, int type);
	void ChangeGroundedVal(int from, int to);

	int CountNumberOf(CtPos ctPos, int radius, int type);
	CctPos CtPos2CctPos(CtPos pos, int chunkHasNCells);
	Vei2 GetWorldSizeInTiles()
	{
		return chunks->GetSize() * cells.GetSize() * Settings::CellSplitUpIn;
	}
	void CastHeal(CtPos pos, int deltaHP, int radius);
	void IncreaseProductivity(CtPos pos,const Settings::OPB& opb);
	void UnitKilled(CtPos killerPos, CtPos victimPos);
	void PlantExpand(CtPos ctPos, int type, int radius, int maxInRange, Team* team);
	void MakeRadomMove(Obstacle* obstacle);
	void MakeMoveTowardsHunting(Obstacle* obstacle);
	void AttractObstacles(Obstacle* attracer, const CtPos& attractTo, const int& radius, const std::vector<int>& allowedTypes, bool costsFood = false);

	CtPos FindNearestObstacle(CtPos pos,std::vector<int> allowedTypes, int radius);
	void ApplyAutoAttackPosWhenNeeded(Obstacle* obstacle);
	void ApplyObstacleEffectFirst(Obstacle* obstacle);
	void ApplyObstacleEffectSecond(Obstacle* obstacle);


	void MarkObstacleMap(Vei2 tilePos, Vei2 size, int index);
	void DeleteObstacle(Vei2 tilePos);

	Vec2 GetCellSize(RectF chunkRect) const;
	Vec2 GetTileSize(RectF chunkRect) const;
	RectF GetCellRect(RectF chunkRect, Vei2 cellPos) const;
	RectF GetTileRect(RectF chunkRect, Vei2 tilePos) const;

	bool CheckIfObstacleBaseTile(Vei2 tilePos);
	bool CheckIfObstacleBaseTileX(Vei2 tilePos);
	bool CheckIfObstacleBaseTileY(Vei2 tilePos);

	bool TileIsInChunk(Vei2& pos)const;
	bool TileIsInWorld(Vei2& pos)const;
	bool Chunk::TileIsInWorld(CtPos& ctPos)const;
	Vei2 PutTileInChunk(Vei2 pos)const;

	int GetGroundedOutOfBounds(Vei2 tilePos) const;
	int GetObstacleTypeOutOfBounds(Vei2 tilePos) const;
public:
	Chunk() = default;
	Chunk(int hasNCells, Cell defVal, Matrix<Chunk>* chunks, sharedResC& resC)
		:
		hasNCells(hasNCells),
		resC(resC),
		chunks(chunks),
		cells(Matrix<Cell>(hasNCells, hasNCells, defVal)),
		obstacleMap(Matrix<int>(hasNCells* Settings::CellSplitUpIn, hasNCells* Settings::CellSplitUpIn, -1))
	{
		conMap.clear();
		for (int i = 0; i < Settings::nDiffFieldTypes; i++)
		{
			conMap.emplace_back(Matrix<int>(hasNCells, hasNCells, 0));
		}
		int k = 0;
	}
	Chunk(const Chunk& other)	//needed because unique_ptr dont gets auto copied
	{
		*this = other;
	}
	Chunk& operator=(const Chunk& other)//needed because unique_ptr donts get auto copied
	{
		
		resC = other.resC;
		hasNCells = other.hasNCells;
		cells = other.cells;
		cellsRect = other.cellsRect;
		tilesRect = other.tilesRect;

		chunks = other.chunks;
		chunkPos = other.chunkPos;
		conMap = other.conMap;
		groundedMap = other.groundedMap;
		aMats = other.aMats;
		obstacleMap = other.obstacleMap;
		obstaclesIndexNotUsed = other.obstaclesIndexNotUsed;


		obstacles.clear();
		for (int i = 0; i < other.obstacles.size(); i++)
		{
			obstacles.push_back(std::make_unique<Obstacle>(*other.obstacles[i].get()));
		}
		return *this;
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
	bool MoveObstacle(int index, CtPos newPos);
	bool MoveObstacle(Obstacle* obstacle, Vec2 dir);	//dir must be normalized
	bool ObstaclePosAllowed(Vei2 tilePos, int type, int except = -1, Matrix<int> placeCondMat = Matrix<int>(1,1,-3)) const;
	bool ObstaclePosAllowed(Vei2 tilePos, Vei2 size, int except = -1, Matrix<int> placeCondMat = Matrix<int>(1, 1, -3)) const;
	int GetObstacleIndex(Vei2 tilePos)const;
	RectF GetTileRect(Vei2 tilePos) const;
	int GetObstacleCount() { return (int)obstacles.size(); }
	void SetTypeAt(Vei2 pos, int type);

	void DrawObstacleOutlines(Vei2 tilePos, int type, RectF chunkRect, Color c, Graphics& gfx) const;
	void DrawObstacle(Vei2 tilePos, int type, RectF chunkRect, int n90rot, Graphics& gfx) const;
	void DrawObstacles(Graphics& gfx) const;
	void DrawType(Graphics& gfx)const;
	void DrawGroundedMap(Graphics& gfx)const;
	void DrawGrit(Graphics& gfx)const;
	void DrawSurfaceAt(Vei2 drawPos, Vei2 cellPos, int cellSize, float scale, const Surface& s, Graphics& gfx)const;
	void DrawTile(RectF chunkRect, Vei2 tilePos, Color c, Graphics& gfx)const;

	void UpdateRects(RectF curRect);
	void DeleteRects();

	void UpdateAroundMatrix(Matrix<int> mat);
	void UpdateGroundedMap();
	void Update(float dt);
	void UpdateWhenMoved(RectF chunkRect);
	bool UnitIsAround(Vei2 tilePos, int range);

	bool PlaceObstacle(Vei2 tilePos, Obstacle* o);
	bool PlaceObstacleWithoutCheck(Vei2 tilePos, Obstacle* o);
	bool PlaceObstacle(Vei2 tilePos, int type, Team* team = nullptr, int ontoType = -1, int surrBy = -1);
	Vei2 PutTileInChunk(int x, int y)const;
	Vec2_<Vei2> GetTilePosOutOfBounds(Vei2 tilePos) const;
	
	int ObstacleMapAt(Vei2 tilePos)const;
	int ObstacleMapAt(Vec3_<Vei2> tilePos)const;
	int ObstacleMapAt(CtPos ctPos)const;
	bool CellIsInWorld(Vei2& pos)const;

	Obstacle* GetObstacleOutOfBounds(Vei2 tilePos) const;

	void NextTurnFirst(std::map<std::string, Team>* teams);
	void NextTurnSecond(std::map<std::string, Team>* teams);

	void NextTurnSecondObstacle(Obstacle* obstacle, std::map<std::string, Team>* teams);

	void AttackTile(CctPos pos, Obstacle* attacker);


	int GroundedMapAt(Vei2 tilePos)const;
	CtPos FindNearestPositionThatFits(Vei2 tilePos, int type);
	void SetConMapAt(Vei2 pos, int type, bool value);
	int GetCellTypeAt(Vei2 pos)const;
	int GetGrounedMapAt(Vei2 pos)const;
	int GetObstacleMapAt(Vei2 pos)const;
	Obstacle* GetObstacleAt(Vei2 pos);
	Matrix<int> GetAroundmatrix(Vei2 pos)const;
	void SetChunkPos(Vei2 pos);
	void PlaceLadderableTiles(int type);
	Cell& GetCellAt(Vei2 cellPos);
	static CtPos CctPos2CtPos(const CctPos& cctPos)
	{
		return CtPos(cctPos.x, cctPos.y * Settings::CellSplitUpIn + cctPos.z);
	}
	static CctPos CtPos2CctPos(const CtPos& ctPos)
	{
		return CctPos(ctPos.x, ctPos.y / Settings::CellSplitUpIn, ctPos.y % Settings::CellSplitUpIn);
	}
	static Vei2 PutChunkInWorld(Vei2 pos, Vei2 worldHasNChunks)
	{
		return PutChunkInWorld(pos.x, pos.y, worldHasNChunks);
		//return Vei2(pos.x % worldHasNChunks.y, pos.y % worldHasNChunks.y);
	}
	static Vei2 PutChunkInWorld(int x, int y, Vei2 worldHasNChunks)
	{
		return Vei2(GigaMath::NegMod(x, worldHasNChunks.x), GigaMath::NegMod(y, worldHasNChunks.y));
	}
	static Vec3_<Vei2> Flat2ChunkPos(Vei2 tilePos, Vei2 wSizeInTiles)
	{
		tilePos = PutTileInWorld(tilePos, wSizeInTiles);

		using namespace Settings;
		Vec3_<Vei2> v3 = Vec3_<Vei2>(Vei2(-1, -1), Vei2(-1, -1), Vei2(-1, -1));

		v3.x = (tilePos / (CellSplitUpIn * Settings::chunkHasNCells));		//chunkPos
		v3.y = (tilePos / CellSplitUpIn) % Settings::chunkHasNCells;		//cellPos in chunk
		v3.z = tilePos % CellSplitUpIn;								//tilePos in cellPos 

		return v3;
	}
	static CtPos Flat2ChunkPosCtPos(Vei2 tilePos, Vei2 wSizeInTiles)
	{
		CctPos pos = Flat2ChunkPos(tilePos, wSizeInTiles);
		return CtPos(pos.x, pos.y * Settings::CellSplitUpIn + pos.z);
	}
	static Vei2 PutTileInWorld(Vei2 pos, Vei2 wSizeInTiles)
	{
		return PutTileInWorld(pos.x, pos.y, wSizeInTiles);
	}
	static Vei2 PutTileInWorld(int x, int y, Vei2 wSizeInTiles)
	{
		if (y < 0)
		{
			y = 0;
		}
		if (y >= wSizeInTiles.y)
		{
			y = wSizeInTiles.y - 1;
		}
		while (x < 0)
		{
			x += wSizeInTiles.x;
		}
		while (x >= wSizeInTiles.x)
		{
			x -= wSizeInTiles.x;
		}
		return Vei2(x, y);
	}
	static CtPos Chunk::PutCtPosInWorld(CtPos ctPos, Vei2 worldHasNChunks)
	{
		//	tiles
		while (ctPos.y.x < 0)
		{
			ctPos.y.x += Settings::chunkHasNTiles;
			ctPos.x.x--;
		}
		while (ctPos.y.x >= Settings::chunkHasNTiles)
		{
			ctPos.y.x -= Settings::chunkHasNTiles;
			ctPos.x.x++;
		}
		while (ctPos.y.y < 0)
		{
			ctPos.y.y += Settings::chunkHasNTiles;
			ctPos.x.y--;
		}
		while (ctPos.y.y >= Settings::chunkHasNTiles)
		{
			ctPos.y.y -= Settings::chunkHasNTiles;
			ctPos.x.y++;
		}
		//	chunks
		while (ctPos.x.x < 0)
		{
			ctPos.x.x += worldHasNChunks.x;
		}
		while (ctPos.x.x >= worldHasNChunks.x)
		{
			ctPos.x.x -= worldHasNChunks.x;
		}
		while (ctPos.x.y < 0)
		{
			ctPos.x.y = 0;
		}
		while (ctPos.x.y >= worldHasNChunks.y)
		{
			ctPos.x.y = worldHasNChunks.y - 1;
		}
		return ctPos;
	}
	static CtPos Chunk::PutCursorInMidOfObstacle(CtPos ctPosBottomLeft, int type, Vei2 worldHasNChunks)
	{
		ctPosBottomLeft.y -= Settings::obstacleStats[type].size[0] / 2;
		return Chunk::PutCtPosInWorld(ctPosBottomLeft, worldHasNChunks);
	}
	static CtPos Chunk::GetMidPosOfObstacle(CtPos ctPosBottomLeft, int type, Vei2 worldHasNChunks)
	{
		ctPosBottomLeft.y += Settings::obstacleStats[type].size[0] / 2;
		return Chunk::PutCtPosInWorld(ctPosBottomLeft, worldHasNChunks);
	}
	static Vei2 chunkPos2Flat(Vec3_<Vei2> cctPos)
	{
		return cctPos.x * Settings::CellSplitUpIn * Settings::chunkHasNCells + cctPos.y * Settings::CellSplitUpIn + cctPos.z;
	}
	static Vei2 chunkPos2Flat(CtPos ctPos)
	{
		return ctPos.x * Settings::CellSplitUpIn * Settings::chunkHasNCells + ctPos.y;
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
	static double GetDistBetween2Obstacles(Obstacle* obstacle1, Obstacle* obstacle2, int worldWidth, Vei2 worldHasNChunks)
	{
		CtPos tileCtPos1 = GetMidPosOfObstacle(obstacle1->GetCtPos(), obstacle1->GetType(), worldHasNChunks);
		CtPos tileCtPos2 = GetMidPosOfObstacle(obstacle2->GetCtPos(), obstacle2->GetType(), worldHasNChunks);

		Vei2 tilePos1 = chunkPos2Flat(tileCtPos1);
		Vei2 tilePos2 = chunkPos2Flat(tileCtPos2);

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
};

