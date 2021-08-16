#include "World.h"

World::World(Settings::WorldSettings wSettings, std::shared_ptr<ResourceCollection> resC, Vec2& camera, std::map<std::string, Team>* teams)
	:
	resC(std::move(resC)),
	c(camera),
	tC(&this->resC->GetSurf()),
	fsC(&this->resC->GetFrameSize()),
	teams(teams),
	worldGen(chunks,this->resC,wSettings, teams)
{
	Init(wSettings);
}
//conMap(Matrix<int>(s.wSize.x, s.wSize.y, 0))
bool World::ChunkIsInWorld(Vei2& pos)const
{
	return pos.x >= 0 && pos.x < s.GetChunkAmount().x && pos.y >= 0 && pos.y < s.GetChunkAmount().y;
}
bool World::CellIsInWorldY(int y)const
{
	return y >= 0 && y < s.GetWorldSizeInCells().y;
}
bool World::TileIsInWorldY(int y)const
{
	return y >= 0 && y < s.GetWorldSizeInCells().y * Settings::CellSplitUpIn;
}
bool World::TileIsInWorld(Vei2& pos)const
{
	return pos.x >= 0 && pos.x < s.GetWorldSizeInCells().x * Settings::CellSplitUpIn && pos.y >= 0 && pos.y < s.GetWorldSizeInCells().y * Settings::CellSplitUpIn;
}

CctPos World::PutCctPosInWorld(CctPos cctPos)const
{
	Vei2 pos = Chunk::chunkPos2Flat(cctPos);
	return Chunk::Flat2ChunkPos(pos, s.GetWorldSizeInTiles());
}
Obstacle* World::GetObstacleAt(CtPos ctPos)
{
	return chunks(ctPos.x).GetObstacleAt(ctPos.y);
}
Obstacle* World::GetObstacleAt(Vec3_<Vei2> cctPos)
{
	return chunks(cctPos.x).GetObstacleAt(cctPos.y * Settings::CellSplitUpIn + cctPos.z);
}
void World::SetBuildMode(int obstacle)
{
	buildMode = true;
	placeObstacle = obstacle;
	placeObstaclen90Rot = 0;
	if (obstacle == 29)
	{
		placeCondMat = Matrix<int>(4, 4, 1);
		placeCondMat.SetValueOfRow(1, -1);
		placeCondMat.SetValueOfRow(2, -1);
		placeCondMat.SetValueOfRow(3, 0);
	}
	else if (obstacle == 36 || obstacle == 38)
	{
		placeCondMat = Matrix<int>(5, 5, -1);
		placeCondMat.SetValueOfRow(0, 1);
		placeCondMat.SetValueOfRow(4, 0);
	}
	else
	{
		//Vei2 size = Settings::obstacleStats[obstacle]
		placeCondMat = Matrix<int>(1, 1, -3);
	}
}
void World::SetCraftMode(int itemID)
{
	Obstacle* builder = nullptr;
	if (storageObst->type == 30)
	{
		builder = storageObst;
	}
	if (builder->craft.get() != nullptr)
	{
		builder->craft->SetItem2Craft(itemID);
	}
}
void World::NextTurn()
{
	CtPos lastFocused = CctPos(Vei2(-1,-1), Vei2(-1, -1), Vei2(-1, -1));
	if (focusedObst != nullptr)
	{
		lastFocused = CtPos(focusedObst->chunkPos, focusedObst->tilePos);
	}
	for (int y = 0; y < chunks.GetColums(); y++)
	{
		for (int x = 0; x < chunks.GetColums(); x++)
		{
			chunks(Vei2(x, y)).NextTurnFirst(teams);
		}
	}
	player->GetMaterials().Set({ {"maxUnits",5} });
	player->GetMaterials().Set({ {"units",0} });
	for (int y = 0; y < chunks.GetColums(); y++)
	{
		for (int x = 0; x < chunks.GetColums(); x++)
		{
			chunks(Vei2(x, y)).NextTurnSecond(teams);
		}
	}
	if (lastFocused != CctPos(Vei2(-1, -1), Vei2(-1, -1), Vei2(-1, -1)))
	{
		focusedObst = GetObstacleAt(lastFocused);
	}
	updateFrameInfo = true;
}
Vei2 World::AbstractTilePos(CctPos chunkPos)const
{
	return chunkPos.y * Settings::CellSplitUpIn + chunkPos.z;
}
Vei2 World::TileIsInCell(Vei2 tilePos)
{
	return Vei2(tilePos / Settings::CellSplitUpIn);
}
void World::DestroyObstacleAt(Vei2 tilePos)
{

	/*
	if (ObstacleMapAt(tilePos) != -1)
	{
		int index = ObstacleMapAt(tilePos);
		Obstacle* curObst = obstacles[ObstacleMapAt(tilePos)].get();
		Vei2 size = Settings::obstacleSizes[curObst->type];
		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				obstacleMap(PutTileInWorld(tilePos + Vei2(x, y))) = -1;
			}
		}
		obstacles.erase(obstacles.begin() + index);
	}
	*/
}
std::vector<SubAnimation> World::GetConnectionsOfTypes(Vei2 pos, int* types)
{
	std::vector<SubAnimation> cons;
	for (int i = 0; i < Settings::ArrSize(types); i++)
	{
		auto ccPos = WorldGenerator::Cell2ChunkPos(pos);
		std::vector<SubAnimation> newC = resC->GetFrameSize().GetConnectionAnimationVec(types[i], false, chunks(ccPos.x).GetAroundmatrix(ccPos.y));
		for (int n = 0; n < (int)newC.size(); n++)
		{
			cons.push_back(newC[n]);
		}
	}
	return cons;
}
/*
RectF World::GetCellRect(Vei2 cellP)const
{
	Vei2 d = cellP - mCell;

	if (d.x > s.wSize.x / 2)
	{
		d.x -= s.wSize.x;
	}
	if (d.x < -wSize.x / 2)
	{
		d.x += s.wSize.x;
	}

	if (d.y > s.wSize.y / 2)
	{
		d.y -= s.wSize.y;
	}
	if (d.y < -wSize.y / 2)
	{
		d.y += s.wSize.y;
	}

	Vei2 mos = Graphics::GetMidOfScreen();

	mos.x += d.x * cSize.x;
	mos.y -= d.y * cSize.y;
	return RectF(Vec2(mos), (float)cSize.x, (float)cSize.y) + Vec2(-(int)c.x, +(int)c.y);
}
RectF World::GetTileRect(Vei2 tileP)const
{
	using namespace Settings;

	Vei2 cellHit = Vei2(tileP.x / CellSplitUpIn, tileP.y / CellSplitUpIn);
	RectF cellPos = GetCellRect(cellHit);
	Vec2 tileSize = GetTileSize();

	cellPos.left += (tileP.x % CellSplitUpIn) * tileSize.x;
	cellPos.right = cellPos.left + tileSize.x;
	cellPos.bottom -= (tileP.y % CellSplitUpIn) * tileSize.y;
	cellPos.top = cellPos.bottom - tileSize.y;

	cellPos -= Vec2(0, cSize.y);

	return cellPos;
}
Vei2 World::GetCellHit(Vec2 mP)const
{
	Vec2 mos = (Vec2) Graphics::GetMidOfScreen();
	mos.x -= c.x;
	mos.y += c.y;

	Vei2 deltaPixel = (Vei2) mos - (Vei2) mP;
	Vei2 deltaCells = { 0,0 };

	deltaCells.y = (deltaPixel.y / cSize.y);
	if (deltaPixel.y < 0)
		deltaCells.y--;
	deltaCells.x = -(deltaPixel.x / cSize.x) - 1;
	if (deltaPixel.x < 0)
		deltaCells.x++;

	return PutCellInWorldX(deltaCells + mCell);
}
*/
Vei2 World::GetChunkHit(Vec2 mP)const
{
	Vec2 mos = (Vec2)Graphics::GetMidOfScreen();
	mos.x -= c.x;
	mos.y += c.y;

	Vei2 deltaPixel = (Vei2) mos - (Vei2) mP;
	Vei2 deltaChunks = { 0,0 };

	deltaChunks.y = (deltaPixel.y / s.GetChunkSize().y);
	if (deltaPixel.y < 0)
		deltaChunks.y--;
	deltaChunks.x = -(deltaPixel.x / s.GetChunkSize().x) - 1;
	if (deltaPixel.x < 0)
		deltaChunks.x++;

	return Chunk::PutChunkInWorld(deltaChunks + mChunk, s.GetChunkAmount());
}
RectF World::GetChunkRectDelta(Vei2 chunkPos)const
{
	Vei2 curChunkPos = Vei2(chunkPos.x * s.GetChunkSize().x, -chunkPos.y * s.GetChunkSize().y) + Graphics::GetMidOfScreen() - Vei2((int)c.x, (int)-c.y);
	return RectF((Vec2)curChunkPos, (float)s.GetChunkSize().x, (float)s.GetChunkSize().y);
}
RectF World::GetChunkRect(Vei2 chunkPos)const
{
	/*
	Vei2 curChunkPos = Vei2(chunkPos.x * s.GetChunkSize().x, -chunkPos.y * s.GetChunkSize().y) + Graphics::GetMidOfScreen() - Vei2((int)c.x, (int)-c.y);
	return RectF((Vec2)curChunkPos, (float)s.GetChunkSize().x, (float)s.GetChunkSize().y);
	*/
	Vei2 d = chunkPos - mChunk;
	if (d.x > s.GetChunkAmount().x / 2)
	{
		d.x -= s.GetChunkAmount().x;
	}
	if (d.x < -s.GetChunkAmount().x / 2)
	{
		d.x += s.GetChunkAmount().x;
	}
	if (d.y > s.GetChunkAmount().y / 2)
	{
		d.y -= s.GetChunkAmount().y;
	}
	if (d.y < -s.GetChunkAmount().y / 2)
	{
		d.y += s.GetChunkAmount().y;
	}

	Vec2 mos = (Vec2)Graphics::GetMidOfScreen();

	mos.x += (float)d.x * s.GetChunkSize().x;
	mos.y -= (float)d.y * s.GetChunkSize().y;
	return RectF(Vec2(mos) - Vec2(0.f, (float)s.GetChunkSize().y), (float)s.GetChunkSize().x, (float)s.GetChunkSize().y) + Vec2(-(float)c.x, +(float)c.y);

}
void World::UpdateChunkRects()
{
	for (int y = 0; y < chunks.GetRows(); y++)
	{
		for (int x = 0; x < chunks.GetColums(); x++)
		{
			chunks[x][y].DeleteRects();
		}
	}
	Vei2 mos = Graphics::GetMidOfScreen();
	auto renderRect = GetRenderRect();

	int xStart = renderRect.left;
	int xStop = renderRect.right;
	int yStart = renderRect.top;
	int yStop = renderRect.bottom;
	/*
	int xStart = 0;
	int xStop = 0;
	int yStart = 0;
	int yStop = 0;
	*/
#ifdef _DEBUG 
	xStart = 0;
	xStop = 0;
	yStart = 0;
	yStop = 0;
#endif


	for (int y = yStart; y <= yStop; y++)
	{
		for (int x = xStart; x <= xStop; x++)
		{
			Vei2 curChunk = Chunk::PutChunkInWorld(mChunk + Vei2(x, y), s.GetChunkAmount());
			
			chunks(curChunk).UpdateRects(GetChunkRectDelta(Vei2(x, y)));
			
		}
	}
}
CtPos World::GetHitTileCtPos(Vec2 mP)const
{
	CctPos pos = GetHitTile(mP);
	return CtPos(pos.x, pos.y * Settings::CellSplitUpIn + pos.z);
}
Vec3_<Vei2> World::GetHitTile(Vec2 mP)const
{
	Vec3_<Vei2> v = Vec3_<Vei2>(Vei2(0, 0), Vei2(0, 0), Vei2(0, 0));

	Vec2 mos = (Vec2)Graphics::GetMidOfScreen();
	mos.x -= c.x;
	mos.y += c.y;
	//Chunk
	Vei2 deltaPixel = (Vei2)mos - (Vei2)mP;
	Vei2 deltaChunks = { 0,0 };

	deltaChunks.y = (deltaPixel.y / s.GetChunkSize().y);
	if (deltaPixel.y < 0)
	{
		deltaChunks.y--;
	}
	deltaChunks.x = -(deltaPixel.x / s.GetChunkSize().x) - 1;
	if (deltaPixel.x < 0)
	{
		deltaChunks.x++;
	}

	v.x = Chunk::PutChunkInWorld(deltaChunks + mChunk, s.GetChunkAmount());

	//Cell
	deltaPixel += Vei2(deltaChunks.x, -deltaChunks.y) * s.GetChunkSize();
	Vei2 deltaCells = { 0,0 };

	deltaCells.y = (deltaPixel.y / GetcSize().y);
	if (deltaPixel.y < 0)
		deltaCells.y--;
	deltaCells.x = -(deltaPixel.x / GetcSize().x) - 1;
	if (deltaPixel.x < 0)
		deltaCells.x++;

	v.y = deltaCells;
	
	
	//Tile
	deltaPixel += Vei2(deltaCells.x, -deltaCells.y) * GetcSize();
	Vei2 deltaTiles = { 0,0 };

	deltaTiles.y = (int)(deltaPixel.y / GetTileSize().y);
	if (deltaPixel.y < 0)
		deltaTiles.y--;
	deltaTiles.x = (int)-(deltaPixel.x / GetTileSize().x);
	if (deltaPixel.x < 0)
		deltaTiles.x++;

	v.z = deltaTiles + Vei2(-1,0);
	

	return v;
}

bool World::TileIsInRange(CctPos tPos1, CctPos tPos2, float range)
{
	Vei2 delta = Chunk::chunkPos2Flat(tPos1) - Chunk::chunkPos2Flat(tPos2);
	if (delta.x > s.GetChunkAmount().x* Settings::chunkHasNTiles / 2)
	{
		delta.x -= s.GetChunkAmount().x * Settings::chunkHasNTiles;
	}
	if (delta.x < -s.GetChunkAmount().x* Settings::chunkHasNTiles / 2)
	{
		delta.x += s.GetChunkAmount().x * Settings::chunkHasNTiles;
	}
	if (sqrt(pow(delta.x,2) + pow(delta.y,2)) <= range)
	{
		return true;
	}
	return false;
}
/*
Vei2 World::GetTileHit(Vec2 mP)const
{
	Vec2 mos = (Vec2)Graphics::GetMidOfScreen();

	Vei2 cell = GetCellHit(mP) ;
	RectF cellRect = GetCellRect(cell);
	Vei2 delta = Vei2(mP.x - cellRect.left,cellRect.bottom - mP.y);
	//assert(delta.x > 0 && delta.y > 0);
	Vec2 tileSize = GetTileSize();
	Vei2 hitTile = Vei2(cell.x * Settings::CellSplitUpIn + delta.x / tileSize.x, (cell.y - 1) * Settings::CellSplitUpIn + delta.y / tileSize.y);

	return hitTile;
}
*/
void World::Zoom(Vei2 delta)
{
	//updateChunkGraphics = true;
	float deltaX = c.x / s.GetChunkSize().x;
	float deltaY = c.y / s.GetChunkSize().y;
	if (delta.x + s.GetChunkSize().x >= 50 && delta.x + s.GetChunkSize().x <= 3000)
	{
		s.AddChunkSize(Vei2(delta.x, 0));
		c.x = s.GetChunkSize().x * deltaX;
	}
	if (delta.y + s.GetChunkSize().y >= 50 && delta.y + s.GetChunkSize().y <= 3000)
	{
		s.AddChunkSize(Vei2(0, delta.y));
		c.y += delta.y / 2;
		c.y = s.GetChunkSize().y * deltaY;
	}
	resC->UpdateFieldCon(s.GetChunkSize() / Settings::chunkHasNCells);
}

void World::ApplyCameraChanges(Vec2 cDelta)
{
	c.x += -cDelta.x;
	c.y += cDelta.y;
	bool moved = false;

	if (c.x < 0)						//move Camera
	{
		mChunk.x--;
		c.x += s.GetChunkSize().x;
		moved = true;
	}
	if (c.x > s.GetChunkSize().x)
	{
		mChunk.x++;
		c.x -= s.GetChunkSize().x;
		moved = true;
	}
	if (c.y < 0)
	{
		mChunk.y--;
		c.y += s.GetChunkSize().y;
		moved = true;
	}
	if (c.y > s.GetChunkSize().y)
	{
		mChunk.y++;
		c.y -= s.GetChunkSize().y;
		moved = true;
	}
	
	if (mChunk.x < 0)
	{
		mChunk.x += s.GetChunkAmount().x;
	}
	if (mChunk.x > s.GetChunkAmount().x - 1)
	{
		mChunk.x -= s.GetChunkAmount().x;
	}
	Vec2 mos = (Vec2) Graphics::GetMidOfScreen();		//Move mCell
	/*
	if (mChunk.y < mos.y / s.GetChunkSize().y)
	{
		mChunk.y =(int) mos.y / s.GetChunkSize().y;
		c.y = 0;
	}
	if (mChunk.y > s.wSize.y - (mos.y / s.GetChunkSize().y) - 1)
	{
		mChunk.y = (int)(s.wSize.y - (mos.y / s.GetChunkSize().y) - 1);
		c.y = s.GetChunkSize().y-0.001f;
	}
	*/
	if (mos.y > (s.GetChunkAmount().y - 1 - mChunk.y) * s.GetChunkSize().y + (s.GetChunkSize().y - c.y))
	{
		mChunk.y = s.GetChunkAmount().y - 1 - ((int)mos.y / s.GetChunkSize().y);
		c.y = (float)(s.GetChunkSize().y -((int)mos.y % (int)s.GetChunkSize().y));
	}
	if (mos.y > mChunk.y * s.GetChunkSize().y + c.y)
	{
		mChunk.y =(int)( mos.y / s.GetChunkSize().y);
		c.y = (float)((int)mos.y % (int)s.GetChunkSize().y);
	}
	if (moved)
	{
		//updateChunkGraphics = true;
	}
}
void World::HandleMouseEvents(Mouse::Event& e, GrabHandle& gH)
{
	CctPos oldCctPos = fcctPos;
	CtPos oldCtPos = fctPos;
	Vec2 mP = (Vec2)e.GetPos();
	fcctPosHover = GetHitTile(mP);
	if (e.GetType() == Mouse::Event::Type::LRelease && !gH.IsLocked())
	{
		fcctPos = GetHitTile(mP);
		fctPos = GetHitTileCtPos(mP);
		//chunks(fctPos.x).SetTypeAt(fcctPos.y, 3);
		//UpdateConMap();
		if (attackMode)
		{
			Vei2 dist2lastclick = Chunk::chunkPos2Flat(Chunk::CtPos2CctPos(Chunk::GetMidPosOfObstacle(focusedObst->GetCtPos(),focusedObst->type,chunks.GetSize()))) - Chunk::chunkPos2Flat(fctPos);
			if (dist2lastclick.x > s.GetWorldSizeInTiles().x / 2)
			{
				dist2lastclick.x -= s.GetWorldSizeInTiles().x;
			}
			if (dist2lastclick.x < -s.GetWorldSizeInTiles().x / 2)
			{
				dist2lastclick.x += s.GetWorldSizeInTiles().x;
			}
			if (attackMode && fctPos != oldCtPos && chunks(fctPos.x).GetObstacleMapAt(fctPos.y) != -1 && sqrt(pow(dist2lastclick.x, 2) + pow(dist2lastclick.y, 2)) <= focusedObst->GetAttackRange() && focusedObst->attack->GetAttacksLeft())
			{
				chunks(fctPos.x).AttackTile(Chunk::CtPos2CctPos(fctPos), focusedObst);
				focusedObst->attack->Attacked();
				if (focusedObst->attack->GetAttacksLeft() <= 0)
				{
					grit = false;
					attackMode = false;
				}
				fctPos = oldCtPos;
			}
			else if (attackMode && fctPos != oldCtPos)
			{
				attackMode = false;
				grit = false;
			}
		}
		if (buildFieldMode)
		{
			chunks(fctPos.x).SetTypeAt(fctPos.y / Settings::CellSplitUpIn, placeField);
			worldGen.UpdateConMap();
			worldGen.UpdateGroundedMap(Vei2(0,0));
		}
		if (moveMode && focusedObst != nullptr && TileIsInRange(Chunk::CtPos2CctPos(fctPos), Chunk::CtPos2CctPos(focusedObst->GetCtPos()), (float)focusedObst->stepsLeft))
		{
			chunks(focusedObst->chunkPos).MoveObstacle(chunks(Vei2(0,0)).ObstacleMapAt(focusedObst->GetCtPos()), fctPos);
			/*if (chunks(focusedObst->chunkPos).MoveObstacle(ObstacleMapAt(focusedObst->GetCtPos()), fctPos))
			{
				for (int y = -1; y < 2; y++)
				{
					for (int x = -1; x < 2; x++)
					{
						auto curChunk = PutCctPosInWorld(Chunk::CtPos2CctPos(fctPos) + Vec3_<Vei2>(Vei2(x, y), Vei2(0, 0), Vei2(0, 0)));
						chunks(curChunk.x).UpdateWhenMoved(GetChunkRect(curChunk.x));
					}
				}
				updateChunkGraphics = true;
			}
			*/
		}
		if (chunks(Vei2(0, 0)).ObstacleMapAt(fctPos) != -1)
		{
			Obstacle* hitObstacle = GetObstacleAt(fctPos);
			if (Settings::anyOfStorage(hitObstacle->type))
			{
				if (storageObst != nullptr && storageObst->type == hitObstacle->type)
				{
					storageObst = nullptr;
				}
				else if(focusedObst != nullptr)
				{
					storageObst = hitObstacle;
				}
			}
			else
			{
				focusedObst = GetObstacleAt(fctPos);
				focusedObst->Update(0.1);
			}
			updateFrameInfo = true;
		}
		else
		{
			if (focusedObst != nullptr && storageObst != nullptr)
			{
				storageObst = nullptr;
			}
			else if (storageObst == nullptr)
			{
				focusedObst = nullptr;
				attackMode = false;
				moveMode = false;
			}
			updateFrameInfo = true;
		}

		CtPos obstacleMidPos = Chunk::PutCursorInMidOfObstacle(Chunk::CctPos2CtPos(fcctPosHover), placeObstacle, chunks.GetSize());
		
		if (buildMode == true && player->GetMaterials().Has((Settings::obstacleStats[placeObstacle].neededResToBuild)) && chunks(obstacleMidPos.x).ObstaclePosAllowed(obstacleMidPos.y,placeObstacle,-1,placeCondMat))
		{
			int constructionWidth = Settings::obstacleStats[placeObstacle].size[0].x;
			ConstructionSite obstacle = ConstructionSite(Settings::obstacleStats[placeObstacle].constructionTime, placeObstacle, obstacleMidPos.y, obstacleMidPos.x, 42 + constructionWidth, resC, player);
			obstacle.n90rot = placeObstaclen90Rot;
			//chunks(fcctPos.x).PlaceObstacle(fcctPos.y * Settings::CellSplitUpIn + fcctPos.z, 42 + constructionWidth, player);
			chunks(obstacleMidPos.x).PlaceObstacleWithoutCheck(obstacleMidPos.y, &obstacle);
			player->GetMaterials().Remove(Settings::obstacleStats[placeObstacle].neededResToBuild);
			if (!player->GetMaterials().Has(Settings::obstacleStats[placeObstacle].neededResToBuild))
			{
				buildMode = false;
			}
			//chunks(fcctPos.x).UpdateGraphics();
			//updateChunkGraphics = true;
		}
		//SpawnUnitGroup(Chunk::chunkPos2Flat(fcctPos), 12, animals, 10);
	}
	if (buildMode)
	{
		CtPos obstacleMidPos = Chunk::PutCursorInMidOfObstacle(Chunk::CctPos2CtPos(fcctPosHover), placeObstacle, chunks.GetSize());
		for (int i = 0; i < 4; i++)
		{
			if (!chunks(obstacleMidPos.x).ObstaclePosAllowed(obstacleMidPos.y, placeObstacle, -1, placeCondMat))
			{
				placeObstaclen90Rot++;
				placeObstaclen90Rot %= 4;
				placeCondMat.RotateAntiClockwise(1);
			}
		}
	}
	if (Settings::zoomingEnabled)
	{
		if (e.GetType() == Mouse::Event::Type::WheelDown)
		{
			Zoom(Vei2(-100, -100));
		}
		if (e.GetType() == Mouse::Event::Type::WheelUp)
		{
			Zoom(Vei2(100, 100));
		}
	}
	posAllowed = ObstaclePosAllowed(fTile, placeObstacle);
	Vec2 cDelta = gH.MoveCamera(e);
	
	ApplyCameraChanges(cDelta);
}
void World::HandleKeyboardEvents(Keyboard::Event& e)
{
	if (e.IsRelease())
	{
		switch (e.GetCode())
		{
		case 'A':
			if (focusedObst != nullptr)
				attackMode = !attackMode;
			break;
		case 'G':
			grit = !grit;
			break;
		case 'B':
			buildMode = !buildMode;
			break;
		case 'O':
			buildFieldMode = !buildFieldMode;
			break;
		case 'R':
			placeObstaclen90Rot++;
			placeObstaclen90Rot %= 4;
			placeCondMat.RotateAntiClockwise(1);
			break;
		case 'E':
			placeObstaclen90Rot--;
			if (placeObstaclen90Rot < 0)
			{
				placeObstaclen90Rot += 4;
			}
			placeCondMat.RotateClockwise(1);
			break;
		case 'P':
			if (placeObstacle < Settings::nDiffObstacles)
			{
				placeObstacle++;
			}
			break;
		case 'M':
			if (placeObstacle > 0)
			{
				placeObstacle--;
			}
			break;
		case VK_ESCAPE:
			buildMode = false;
			grit = false;
			attackMode = false;
			break;
		}
	}
}
void World::UpdateGameLogic(float dt)
{
	auto renderRect = GetRenderRect();
	int xStart = renderRect.left;
	int xStop = renderRect.right;
	int yStart = renderRect.top;
	int yStop = renderRect.bottom;

	Vei2 chunkSize = GetChunkRectDelta(Vei2(0, 0)).GetSize();

	#ifdef _DEBUG 
	xStart = -1;
	xStop = 1;
	yStart = -1;
	yStop = 1;
	#endif
			for (int y = yStart; y <= yStop; y++)
			{
				for (int x = xStart; x <= xStop; x++)
				{
					Vei2 curChunk = Chunk::PutChunkInWorld(mChunk + Vei2(x, y), s.GetChunkAmount());
					chunks(curChunk).Update(dt);
				}
			}
			UpdateChunkRects();
}
void World::Draw(Graphics& gfx) const
{

	Vei2 mos = Graphics::GetMidOfScreen();
	auto renderRect = GetRenderRect();
	
	int xStart = renderRect.left;
	int xStop = renderRect.right;
	int yStart = renderRect.top;
	int yStop = renderRect.bottom;
	/*
	int xStart = 0;
	int xStop = 0;
	int yStart = 0;
	int yStop = 0;
	*/
#ifdef _DEBUG 
	xStart = 0;
	xStop = 0;
	yStart = 0;
	yStop = 0;
#endif
	
	
	for (int layer = 0; layer < 3; layer++)
	{
		for (int y = yStart; y <= yStop; y++)
		{
			for (int x = xStart; x <= xStop; x++)
			{
				Vei2 curChunk = Chunk::PutChunkInWorld(mChunk + Vei2(x, y),s.GetChunkAmount());
				Vei2 curChunkPos = Vei2(x * s.GetChunkSize().x, -y * s.GetChunkSize().y) + Graphics::GetMidOfScreen() - Vei2((int)c.x, (int)-c.y);
				
				switch (layer)
				{
				case 0:
					chunks(curChunk).DrawType(gfx);
					break;
				case 1:
					chunks(curChunk).DrawObstacles(gfx);
					if (grit || buildMode)
					{
						chunks(curChunk).DrawGroundedMap(gfx);
					}
					break;
				case 2:
					if (grit || buildMode)
					{
						chunks(curChunk).DrawGrit(gfx);
					}
					break;
				case 3:
					if (fctPos.x == curChunk)	//focused cell marker
					{
						//chunks(curChunk).DrawSurfaceAt(curChunkPos, fcctPos.y, s.GetChunkSize().x / Settings::chunkHasNCells, 1.5f, resC->tC.frames.at(0).GetCurSurface(), gfx);
					}
					//gfx.DrawRect(GetChunkRectDelta(Vei2(0,1)), Colors::Red);
					break;
				}
			}
		}
	}
	if (buildMode)
	{
		CtPos obstacleMidPos = Chunk::PutCursorInMidOfObstacle(Chunk::CctPos2CtPos(fcctPosHover),placeObstacle,chunks.GetSize());
		if (chunks(obstacleMidPos.x).ObstaclePosAllowed(obstacleMidPos.y , placeObstacle,-1,placeCondMat))
		{
			chunks(obstacleMidPos.x).DrawObstacleOutlines(obstacleMidPos.y, placeObstacle, GetChunkRect(obstacleMidPos.x), Colors::Green, gfx);
		}
		else
		{
			chunks(obstacleMidPos.x).DrawObstacleOutlines(obstacleMidPos.y, placeObstacle, GetChunkRect(obstacleMidPos.x), Colors::Red, gfx);
		}
		chunks(obstacleMidPos.x).DrawObstacle(obstacleMidPos.y, placeObstacle, GetChunkRect(obstacleMidPos.x),placeObstaclen90Rot, gfx);
	}
	if (storageObst != nullptr)
	{
		chunks(storageObst->chunkPos).DrawObstacleOutlines(storageObst->tilePos, storageObst->type, GetChunkRect(storageObst->chunkPos), Colors::Purple, gfx);
	}
	if (focusedObst != nullptr)
	{
		chunks(focusedObst->chunkPos).DrawObstacleOutlines(focusedObst->tilePos, focusedObst->type, GetChunkRect(focusedObst->chunkPos), Colors::Blue, gfx);
		CtPos obstacleMidPos = Chunk::GetMidPosOfObstacle(CtPos(focusedObst->chunkPos, focusedObst->tilePos), focusedObst->type, chunks.GetSize());
		if (focusedObst->heal != nullptr && focusedObst->heal->Isenabled())
		{
			DrawCircle(obstacleMidPos, Settings::obstacleStats[focusedObst->type].healRange, Colors::Green,gfx);
		}
		if (Settings::obstacleStats[focusedObst->type].opb.BoostsProd())
		{
			DrawCircle(obstacleMidPos, Settings::obstacleStats[focusedObst->type].opb.GetProdBoostrange(), Colors::Purple, gfx);
		}
		/*
		if (focusedObst->attack != nullptr && focusedObst->attack->GetReloadNextTurn())
		{
			DrawCircle(obstacleMidPos, focusedObst->GetAttackRange(), Colors::Red, gfx);
		}
		*/
	}
	if (moveMode && Settings::obstaclesOn && focusedObst != nullptr)
	{
		Vei2 bottomLeft = Chunk::chunkPos2Flat(focusedObst->GetCtPos());
		CctPos curcctPos = Chunk::Flat2ChunkPos(bottomLeft, s.GetWorldSizeInTiles());
		int moveRange = focusedObst->stepsLeft;
		for (int y = -moveRange; y <= moveRange; y++)
		{
			for (int x = -moveRange; x <= moveRange; x++)
			{
				CctPos cctDelta = { Vei2(0,0),Vei2(0,0),Vei2(x,y) };
				CctPos cctPos = PutCctPosInWorld(curcctPos + cctDelta);

				Vec2_<Vei2> ctPos = chunks(cctPos.x).GetTilePosOutOfBounds(cctPos.y * Settings::CellSplitUpIn + cctPos.z);
				int index = chunks(ctPos.x).GetObstacleIndex(focusedObst->tilePos);
				if (moveRange > 0 && (x != 0 || y != 0) && sqrt(pow(x, 2) + pow(y, 2)) <= moveRange && chunks(ctPos.x).ObstaclePosAllowed(ctPos.y,focusedObst->type, index))
				{
					chunks(ctPos.x).DrawTile(GetChunkRect(ctPos.x), ctPos.y, Colors::Green, gfx);
				}
			}
		}
	}
	if (attackMode && focusedObst->attack != nullptr && focusedObst->attack->GetAttacksLeft())
	{
		CtPos obstacleMidPos = Chunk::GetMidPosOfObstacle(CtPos(focusedObst->chunkPos,focusedObst->tilePos), focusedObst->type, chunks.GetSize());
		DrawCircle(obstacleMidPos, focusedObst->GetAttackRange(), Colors::Red, gfx);
	}
}
void World::DrawObstacle(Vei2 tilePos, int type, Graphics& gfx, Color color, int frame)const
{
	CtPos fctPos = Chunk::Flat2ChunkPosCtPos(tilePos, s.GetWorldSizeInTiles());
	chunks(fctPos.x).DrawObstacle(tilePos, type, GetChunkRect(fctPos.x),0, gfx);
	/*
	Vec2 tileSize = GetTileSize();
	//RectI dRect = (RectI)GetTileRect(tilePos);
	RectI dRect = RectI(Vei2(100,100),100,100);
	dRect.right += (int)(tileSize.x * (Settings::obstacleSizes[placeObstacle].x - 1));
	dRect.top -= (int)(tileSize.y * (Settings::obstacleSizes[placeObstacle].y - 1));
	
	if (frame == -1)
	{
		if (color == Colors::Magenta)
		{
			gfx.DrawSurface(dRect, tC->obstacles[type].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
		}
		else
		{
			gfx.DrawRect((RectF)dRect, color);
			gfx.DrawFilledRect((RectF)dRect, Colors::Red, SpriteEffect::Transparent(0.25f));
		}
	}
	else
	{
		assert(frame >= 0 && frame < tC->obstacles[type].GetNumberOfFrames());
		if (color == Colors::Magenta)
		{
			gfx.DrawSurface(dRect, tC->obstacles[type].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
		}
		else
		{
			gfx.DrawRect((RectF)dRect, color);
			gfx.DrawFilledRect((RectF)dRect, Colors::Red, SpriteEffect::Transparent(0.25f));
		}
	}
	*/
}
void World::DrawCircle(CtPos pos, int radius, Color c, Graphics& gfx)const
{
	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			CtPos ctDelta = { Vei2(0,0),Vei2(x,y) };
			CtPos ctPos = Chunk::PutCtPosInWorld(pos + ctDelta, chunks.GetSize());

			if (sqrt(pow(x, 2) + pow(y, 2)) <= radius)
			{
				gfx.DrawRect(chunks(ctPos.x).GetTileRect(ctPos.y), c);
				gfx.DrawFilledRect(chunks(ctPos.x).GetTileRect(ctPos.y), c,SpriteEffect::Transparent(0.25));
			}
		}
	}
}
bool World::NeedsConnections(Vei2 curXY)const
{
	auto ccPos = WorldGenerator::Cell2ChunkPos(curXY);
	if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
	{
		return false;
	}
	return true;
}

bool World::ObstaclePosAllowed(Vei2 tilePos, int type)
{
	auto tccPos = Chunk::Flat2ChunkPos(Chunk::PutTileInWorld(tilePos, s.GetWorldSizeInTiles()), s.GetWorldSizeInTiles());
	if (chunks(tccPos.x).ObstaclePosAllowed(tccPos.y * Settings::CellSplitUpIn + tccPos.z, Settings::obstacleStats[type].size[0]))
	{
		return true;
	}
	return false;
}
void World::Init(Settings::WorldSettings& s)
{
	worldGen.Generate(s);
	mChunk = worldGen.GetMChunk();
	c = worldGen.GetCamera();
	player = &teams->at("player");
	this->s = s;
	for (int type = 1; type < (int)tC->fields.size(); type++)		//Create connectionsmaps
	{
		//conMap.push_back(Matrix<int>(s.wSize.x, s.wSize.y, 0));
	}
	Zoom(Vei2(0,0));
	UpdateChunkRects();
}

/*
void World::GenerateExplosion(Vei2 pos, int maxLineLength, int type, int ontoType, int nRolls, int surrBy)//not
{
	for (int i = 0; i < nRolls; i++)
	{
		float rad = (float)rng.Calc(360) * 0.0174533f;
		Vec2 p1 = (Vec2)GigaMath::RotPointToOrigin<double>(1.0f, 0.0f, rad);
		Vei2 scaled = pos + (Vei2)((p1 * maxLineLength) * GigaMath::GetRandomNormDistribution());

		//cells(PutCellInWorld(pos + scaled)).type = type;
		GenerateCell(pos + scaled, type, ontoType, surrBy);
	}
}
void World::GenerateCell(Vei2 pos, int type, int ontoType, int surrBy)
{
	pos = PutCellInWorld(pos);
	Matrix<int> aMat = GetAroundMatrix(pos);
	if ((surrBy == -1 || aMat.HasValue(surrBy)) && (ontoType == -1 || ontoType == cells(pos).type))
	{
		cells(pos).type = type;
	}
}
*/