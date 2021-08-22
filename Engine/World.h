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
#include "WorldGenerator.h"
#pragma once
class World
{
	friend class WorldDrawer;
private:
	
	//Resourcen
	std::shared_ptr<ResourceCollection> resC;
	const TexturesCollection* tC;
	const FramesizeCollection* fsC;
	RandyRandom rng;
	Settings::WorldSettings s;
	WorldGenerator worldGen;
	//Gamevars
	//Zelle in der Mitte des Bildschirms auf dem Debugzeiger ist
	Vei2 mChunk = { 0,0 };
	Vei2 fTile = { 0,0 };
	Matrix<Chunk> chunks;
	Vec3_<Vei2> fcctPos = Vec3_<Vei2>(Vei2(0, 0), Vei2(1, 1), Vei2(1, 1));
	Vec3_<Vei2> fcctPosHover = Vec3_<Vei2>(Vei2(10, 12), Vei2(1, 1), Vei2(1, 1));
	CtPos fctPos = Vec2_<Vei2>(Vei2(0, 0), Vei2(0, 0));


	Obstacle* focusedObst = nullptr;
	Obstacle* storageObst = nullptr;

	Vec2& c;								//Camera


	//std::vector<Team> enemys;
	//Team player;

	bool grit = false;						//show grit
	bool buildMode = false;					//place something
	int placeObstacle = 30;
	bool buildFieldMode = false;
	int placeField = 14;
	int placeObstaclen90Rot = 0;
	Matrix<int> placeCondMat = Matrix<int>(1, 1, -3);
	bool posAllowed = true;
	bool moveMode = false;
	bool attackMode = false;
	bool updateChunkGraphics = true;

	bool updateFrameInfo = false;
	bool showBuildMenu = false;
	bool loadBuildMenu = false;
	bool buildMenuShown = false;

	std::map<std::string, Team>* teams;
	Team* player;
	//Team animals = Team("Fuer die Natur");
	/*
	Team enemie1 = Team("In dem Sinne");
	Team enemie2 = Team("Nichts wie Zeg");
	Team enemie3 = Team("Was geht!");
	*/

	//Mouse calculations
	Vei2 GetChunkHit(Vec2 mP)const;
	Vec3_<Vei2> GetHitTile(Vec2 mP)const;
	CtPos World::GetHitTileCtPos(Vec2 mP)const;

	//Tile operations
	bool TileIsInRange(CctPos tPos1, CctPos tPos2, float range);

	//Graphic calculations
	RectF GetChunkRect(Vei2 pos)const;
	void UpdateChunkRects();
	RectF GetChunkRectDelta(Vei2 chunkPos)const;

	//Tests and corrections
	bool TileIsInWorld(Vei2& pos)const;
	bool CellIsInWorldY(int y)const;		
	bool TileIsInWorldY(int y)const;
	bool ChunkIsInWorld(Vei2& cellPos)const;


	Vei2 AbstractTilePos(CctPos chunkPos)const;
	

	CctPos PutCctPosInWorld(CctPos cctPos)const;
	

	Obstacle* World::GetObstacleAt(Vec2_<Vei2> ctPos);
	Obstacle* GetObstacleAt(Vec3_<Vei2> tilePos);

	Vei2 TileIsInCell(Vei2 tilePos);
	//Init
	void DestroyObstacleAt(Vei2 tilePos);
	//Private not const Funktions
	void Zoom(Vei2 delta);				//Delta == delta cSize
	void ApplyCameraChanges(Vec2 cDelta);


	void Init(Settings::WorldSettings& s);
	bool ObstaclePosAllowed(Vei2 tilePos, int type);

public:
	
	int updatedGraphics = 0;																						//Konstruktor + Operatoren
	World(Settings::WorldSettings wSettings, std::shared_ptr<ResourceCollection> resC, Vec2& camera, std::map<std::string, Team>* teams);
	
	//Handles
	void HandleMouseEvents(Mouse::Event& e, GrabHandle& gH);
	void HandleKeyboardEvents(Keyboard::Event& e);
	void UpdateGameLogic(float dt);
	
	//Grafiken + Einbindung dieser in groundedMap
	void Draw(Graphics& gfx)const;
	void DrawObstacle(Vei2 tilePos, int type, Graphics& gfx, Color color = Colors::Magenta, int frame = -1)const;
	void DrawCircle(CtPos pos, int radius, Color c, Graphics& gfx)const;

	std::vector<SubAnimation> GetConnectionsOfTypes(Vei2 pos, int* types);
	bool NeedsConnections(Vei2 curXY)const;
	void SetBuildMode(int obstacle);
	void SetCraftMode(int itemID);
	void SetMoveMode(bool moveMode)
	{
		if (focusedObst != nullptr)
		{
			this->moveMode = moveMode;
		}
	}
	void SetAttackMode(bool val)
	{
		if (val && focusedObst != nullptr && focusedObst->attack->GetAttacksLeft() > 0)
		{
			grit = true;
			attackMode = true;
		}
		else
		{
			grit = false;
			attackMode = false;
		}
	}
	bool GetAttackMode()
	{
		return attackMode;
	}
	void NextTurn();
	//Frames Update
	bool UpdateFrameInfo()const{ return updateFrameInfo; }
	void FramesUpdated() { updateFrameInfo = false; }

	void LoadBuildMenu()
	{
		loadBuildMenu = true;
	}
	bool NeedToLoadBuildMenu()
	{
		return loadBuildMenu;
	}
	void BuildMenuLoaded()
	{
		buildMenuShown = true;
		loadBuildMenu = false;
	}
	bool BuildMenuShown()
	{
		return buildMenuShown;
	}
	//
	Vei2 GetwSize()const { return s.GetWorldSizeInCells(); }
	Vei2 GetcSize()const { return s.GetChunkSize() / Settings::chunkHasNCells; }
	Vec2 GetTileSize()const { return (Vec2)GetcSize()/Settings::CellSplitUpIn; }
	Vei2 GetfCell()const { return fctPos.x*Settings::chunkHasNCells + fctPos.y / Settings::CellSplitUpIn; }
	Vei2 GetfTile()const { return Chunk::chunkPos2Flat(fcctPos); }
	int GetfCellType()const { return chunks(fctPos.x).GetCellTypeAt(fctPos.y / Settings::CellSplitUpIn); }
	Vei2 GetmChunk()const { return mChunk; }
	void SetPlayer(Team* player) { this->player = player; }
	bool GetBuildMode() { return buildMode; }
	void SetBuildMode(bool val) { buildMode = val; }
	int GetPlaceObstacle() { return placeObstacle; }
	int GetPlaceField() { return placeField; }
	int GetObstacleCount()
	{
		return chunks(fctPos.x).GetObstacleCount();
	}
	Team* GetPlayer()const { return player; }
	RectI GetRenderRect()const { 
		auto mos = Graphics::GetMidOfScreen();
		return RectI(-1 - (int)((mos.x / s.GetChunkSize().x) * 1.5f), 1 + (int)((mos.x / s.GetChunkSize().x) * 1.5f), -1 - (int)((mos.y / s.GetChunkSize().y) * 1.5f), 1 + (int)((mos.y / s.GetChunkSize().y) * 1.5f)); };
	Obstacle* GetFocusedObstacle()const
	{
		return focusedObst;
	}
	Obstacle* GetStorageObstacle()const
	{
		return storageObst;
	}
	const Settings::WorldSettings* GetWorldSettings()
	{
		return &s;
	}
	Matrix<Chunk>& GetChunks()
	{
		return chunks;
	}
};

