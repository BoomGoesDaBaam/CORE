#include "World.h"

World::World(WorldSettings wSettings, std::shared_ptr<ResourceCollection> resC, Vec2& camera) 
	:
	resC(std::move(resC)),
	c(camera),
	tC(&this->resC->tC),
	fsC(&this->resC->fsC)
{
	Init(wSettings);
	Generate(wSettings);
}
//conMap(Matrix<int>(s.wSize.x, s.wSize.y, 0))
bool World::CellIsInWorld(Vei2& v)const
{
	return v.x >= 0 && v.x < s.wSize.x && v.y >= 0 && v.y < s.wSize.y;
}
bool World::ChunkIsInWorld(Vei2& pos)const
{
	return pos.x >= 0 && pos.x < s.worldHasNChunks.x && pos.y >= 0 && pos.y < s.worldHasNChunks.y;
}
bool World::CellIsInWorldY(int y)const
{
	return y >= 0 && y < s.wSize.y;
}
bool World::TileIsInWorldY(int y)const
{
	return y >= 0 && y < s.wSize.y * Settings::CellSplitUpIn;
}
bool World::TileIsInWorld(Vei2& pos)const
{
	return pos.x >= 0 && pos.x < s.wSize.x * Settings::CellSplitUpIn && pos.y >= 0 && pos.y < s.wSize.y * Settings::CellSplitUpIn;
}

Vec2_<Vei2> World::Cell2ChunkPos(Vei2 cellPos)const
{
	return Vec2_<Vei2>(cellPos / Settings::chunkHasNCells, cellPos % Settings::chunkHasNCells);
}

Vec3_<Vei2> World::Tile2ChunkPos(Vei2 tilePos)const
{
	tilePos = PutTileInWorld(tilePos);

	using namespace Settings;
	Vec3_<Vei2> v3 = Vec3_<Vei2>(Vei2(-1, -1), Vei2(-1, -1), Vei2(-1, -1));

	v3.x = (tilePos / (CellSplitUpIn * Settings::chunkHasNCells));		//chunkPos
	v3.y = (tilePos / CellSplitUpIn) % Settings::chunkHasNCells;		//cellPos in chunk
	v3.z = tilePos % CellSplitUpIn;								//tilePos in cellPos 

	return v3;
}
Vec3_<Vei2> World::PutCctPosInWorld(CctPos cctPos)const
{
	Vei2 pos = chunkPos2Flat(cctPos);
	return Tile2ChunkPos(pos);
}
int World::ObstacleMapAt(Vei2 tilePos)const
{
	auto tccPos = Tile2ChunkPos(tilePos);
	return chunks(tccPos.x).GetObstacleMapAt(tccPos.y * Settings::CellSplitUpIn + tccPos.z);
}
int World::ObstacleMapAt(Vec3_<Vei2> cctPos)const
{
	return chunks(cctPos.x).GetObstacleMapAt(cctPos.y * Settings::CellSplitUpIn + cctPos.z);
}
int World::GroundedMapAt(Vei2 tilePos)const
{
	auto tccPos = Tile2ChunkPos(tilePos);
	return chunks(tccPos.x).GetGrounedMapAt(tccPos.y * Settings::CellSplitUpIn + tccPos.z);
}
Obstacle* World::GetObstacleAt(Vec3_<Vei2> cctPos)
{
	return chunks(cctPos.x).GetObstacleAt(cctPos.y * Settings::CellSplitUpIn + cctPos.z);
}
Vei2 World::PutCellInWorldX(Vei2 v)const
{
	return PutCellInWorldX(v.x, v.y);
}
Vei2 World::PutCellInWorldX(int x, int y)const
{
	if (x < 0)
	{
		x = -x;
		x %= s.wSize.x;
		x = s.wSize.x - x;
	}
	if (x >= s.wSize.x)
	{
		x = x % s.wSize.x;
	}

	if (y < 0)
	{
		y = 0;
	}
	if (y >= s.wSize.y)
	{
		y = s.wSize.y - 1;
	}
	return Vei2(x,y);
}
void World::UpdateConMap()
{
	for (int type = 0; type < Settings::nDiffFieldTypes;type++)
	{
		for (int y = 0; y < s.wSize.y; y++)
		{
			for (int x = 0; x < s.wSize.x; x++)
			{
				auto ccPos = Cell2ChunkPos(Vei2(x, y));
				int curCellType = chunks(ccPos.x).GetCellTypeAt(ccPos.y);
	
				if (type != curCellType && IsSurroundedBy(Vei2(x, y), type))
				{
					chunks(ccPos.x).SetConMapAt(ccPos.y, type, 1);
				}
				else
				{
					chunks(ccPos.x).SetConMapAt(ccPos.y, type, 0);
				}
			}
		}
	}
	for (int y = 0; y < s.worldHasNChunks.y; y++)
	{
		for (int x = 0; x < s.worldHasNChunks.x; x++)
		{
			Matrix<int> mat = Matrix<int>(Settings::chunkHasNCells + 2, Settings::chunkHasNCells + 2, -1);
			for (int yInner = 0; yInner < Settings::chunkHasNCells + 2; yInner++)
			{
				for (int xInner = 0; xInner < Settings::chunkHasNCells + 2; xInner++)
				{
					auto ccPos = Cell2ChunkPos(PutCellInWorldX(x * Settings::chunkHasNCells + xInner - 1, y * Settings::chunkHasNCells + yInner - 1));

					mat[xInner][yInner] = chunks(ccPos.x).GetCellTypeAt(ccPos.y);
				}
			}
			chunks[x][y].UpdateAroundMatrix(mat);
		}
	}
}
void World::UpdateGroundedMap(Vei2 pos, Vei2 size)
{
	using namespace Settings;
	if (obstaclesOn)
	{
		if (size == Vei2(-1, -1) || size == s.wSize)
		{
			size = s.worldHasNChunks;
		}

		for (int y = 0; y < pos.y + size.y; y++)
		{
			for (int x = pos.x; x < pos.x + size.x; x++)
			{
				chunks[x][y].UpdateGroundedMap();
			}
		}
		for (int y = 0; y < pos.y + size.y; y++)
		{
			for (int x = pos.x; x < pos.x + size.x; x++)
			{
				chunks[x][y].PlaceLadderableTiles(3);
			}
		}
	}
}
void World::SpawnUnits(int n, Team& inTeam, Vei2 tilePos)
{
	for (int i = 0; i < n; i++)
	{
		float rad = (float)rng.Calc(360) * 0.0174533f;
		Vec2 p1 = (Vec2)GigaMath::RotPointToOrigin<float>(1.0f, 0.0f, rad);
		GenerateObstacle(tilePos + Vei2(p1 * 5), 10);
	}
}
void World::SpawnPlayer()
{
	Vei2 spawnpoint = Vei2(0, s.wSize.y/2);
	while (GroundedMapAt(spawnpoint) != 1)
	{
		spawnpoint.x++;
	}
	SpawnUnits(5, player, spawnpoint);
	auto cctPos = Tile2ChunkPos(spawnpoint);
	mChunk = cctPos.x;

}
Vei2 World::PutTileInWorld(Vei2 pos)const
{
	return PutTileInWorld(pos.x, pos.y);
}

void World::SetBuildMode(int obstacle)
{
	buildMode = true;
	placeObstacle = obstacle;
}

Vei2 World::PutTileInWorld(int x, int y)const
{
	if (y < 0)
	{
		y = 0;
	}
	if (y >= s.wSize.y * Settings::CellSplitUpIn)
	{
		y = s.wSize.y * Settings::CellSplitUpIn - 1;
	}
	while (x < 0)
	{
		x += s.wSize.x * Settings::CellSplitUpIn;
	}
	while (x >= s.wSize.x * Settings::CellSplitUpIn)
	{
		x -= s.wSize.x * Settings::CellSplitUpIn;
	}
	return Vei2(x, y);
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
		auto ccPos = Cell2ChunkPos(pos);
		std::vector<SubAnimation> newC = resC->fsC.GetConnectionAnimationVec(types[i], pos, false, chunks(ccPos.x).GetAroundmatrix(ccPos.y));
		for (int n = 0; n < newC.size(); n++)
		{
			cons.push_back(newC[n]);
		}
	}
	return cons;
}

bool World::IsSurroundedBy(Vei2 pos, int type)
{
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			auto ccPos = Cell2ChunkPos(PutCellInWorldX(pos + Vei2(x - 1, y - 1)));

			if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == type)
			{
				return true;
			}
		}
	}
	return false;
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

	deltaChunks.y = (deltaPixel.y / s.chunkSize.y);
	if (deltaPixel.y < 0)
		deltaChunks.y--;
	deltaChunks.x = -(deltaPixel.x / s.chunkSize.x) - 1;
	if (deltaPixel.x < 0)
		deltaChunks.x++;

	return Chunk::PutChunkInWorld(deltaChunks + mChunk, s.worldHasNChunks);
}
RectF World::GetChunkRect(Vei2 chunkPos)const
{
	Vei2 d = chunkPos - mChunk;

	if (d.x > s.worldHasNChunks.x / 2)
	{
		d.x -= s.worldHasNChunks.x;
	}
	if (d.x < -s.worldHasNChunks.x / 2)
	{
		d.x += s.worldHasNChunks.x;
	}
	if (d.y > s.worldHasNChunks.y / 2)
	{
		d.y -= s.worldHasNChunks.y;
	}
	if (d.y < -s.worldHasNChunks.y / 2)
	{
		d.y += s.worldHasNChunks.y;
	}

	Vei2 mos = Graphics::GetMidOfScreen();

	mos.x += d.x * s.chunkSize.x;
	mos.y -= d.y * s.chunkSize.y;
	return RectF(Vec2(mos) - Vec2(0, s.chunkSize.y), (float)s.chunkSize.x, (float)s.chunkSize.y) + Vec2(-(int)c.x, +(int)c.y);
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

	deltaChunks.y = (deltaPixel.y / s.chunkSize.y);
	if (deltaPixel.y < 0)
	{
		deltaChunks.y--;
	}
	deltaChunks.x = -(deltaPixel.x / s.chunkSize.x) - 1;
	if (deltaPixel.x < 0)
	{
		deltaChunks.x++;
	}

	v.x = Chunk::PutChunkInWorld(deltaChunks + mChunk, s.worldHasNChunks);

	//Cell
	deltaPixel += Vei2(deltaChunks.x, -deltaChunks.y) * s.chunkSize;
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

	deltaTiles.y = (deltaPixel.y / GetTileSize().y);
	if (deltaPixel.y < 0)
		deltaTiles.y--;
	deltaTiles.x = -(deltaPixel.x / GetTileSize().x);
	if (deltaPixel.x < 0)
		deltaTiles.x++;

	v.z = deltaTiles + Vei2(-1,0);
	

	return v;
}

bool World::TileIsInRange(CctPos tPos1, CctPos tPos2, float range)
{
	Vei2 delta = chunkPos2Flat(tPos1) - chunkPos2Flat(tPos2);
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
	float deltaX = c.x / s.chunkSize.x;
	float deltaY = c.y / s.chunkSize.y;
	if (delta.x + s.chunkSize.x >= 50 && delta.x + s.chunkSize.x <= 1200)
	{
		s.chunkSize.x += delta.x;
		c.x = s.chunkSize.x * deltaX;
	}
	if (delta.y + s.chunkSize.y >= 50 && delta.y + s.chunkSize.y <= 1200)
	{
		s.chunkSize.y += delta.y;
		c.y += delta.y / 2;
		c.y = s.chunkSize.y * deltaY;
	}
	fsC->Update(s.chunkSize / Settings::chunkHasNCells);
}

void World::ApplyCameraChanges(Vec2 cDelta)
{
	c.x += -cDelta.x;
	c.y += cDelta.y;
	
	if (c.x < 0)						//move Camera
	{
		mChunk.x--;
		c.x += s.chunkSize.x;
	}
	if (c.x > s.chunkSize.x)
	{
		mChunk.x++;
		c.x -= s.chunkSize.x;
	}
	if (c.y < 0)
	{
		mChunk.y--;
		c.y += s.chunkSize.y;
	}
	if (c.y > s.chunkSize.y)
	{
		mChunk.y++;
		c.y -= s.chunkSize.y;
	}
	
	if (mChunk.x < 0)
	{
		mChunk.x += s.worldHasNChunks.x;
	}
	if (mChunk.x > s.worldHasNChunks.x - 1)
	{
		mChunk.x -= s.worldHasNChunks.x;
	}
	Vec2 mos = (Vec2) Graphics::GetMidOfScreen();		//Move mCell
	/*
	if (mChunk.y < mos.y / s.chunkSize.y)
	{
		mChunk.y =(int) mos.y / s.chunkSize.y;
		c.y = 0;
	}
	if (mChunk.y > s.wSize.y - (mos.y / s.chunkSize.y) - 1)
	{
		mChunk.y = (int)(s.wSize.y - (mos.y / s.chunkSize.y) - 1);
		c.y = s.chunkSize.y-0.001f;
	}
	*/
	if (mos.y > (s.worldHasNChunks.y - 1 - mChunk.y) * s.chunkSize.y + (s.chunkSize.y - c.y))
	{
		mChunk.y = s.worldHasNChunks.y - 1 - ((int)mos.y / s.chunkSize.y);
		c.y = s.chunkSize.y -((int)mos.y % (int)s.chunkSize.y);
	}
	if (mos.y > mChunk.y * s.chunkSize.y + c.y)
	{
		mChunk.y = mos.y / s.chunkSize.y;
		c.y = (int)mos.y % (int)s.chunkSize.y;
	}
}
void World::HandleMouseEvents(Mouse::Event& e, GrabHandle& gH)
{
	CctPos lastcctPos = fcctPos;
	Vec2 mP = (Vec2)e.GetPos();
	fcctPosHover = GetHitTile(mP);
	if (e.GetType() == Mouse::Event::LRelease && !gH.IsLocked())
	{
		fcctPos = GetHitTile(mP);
		if (ObstacleMapAt(fcctPos) != -1)
		{
			focusedObst = GetObstacleAt(fcctPos);
		}
		else
		{
			focusedObst = nullptr;
		}

		if (buildMode == true)
		{
			auto fcctPos = GetHitTile(mP);
			chunks(fcctPos.x).PlaceObstacle(fcctPos.y * Settings::CellSplitUpIn + fcctPos.z, 10);
		}
		if (moveMode && TileIsInRange(lastcctPos, fcctPos,moveRange))
		{
			buildMode = true;
		}
	}

	if (e.GetType() == Mouse::Event::WheelDown)
	{
		Zoom(Vei2(-50, -50));
	}
	if (e.GetType() == Mouse::Event::WheelUp)
	{
		Zoom(Vei2(50, 50));
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
		case 'G':
			grit = !grit;
			break;
		case 'B':
			buildMode = !buildMode;
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
				Vei2 curChunk = Chunk::PutChunkInWorld(mChunk + Vei2(x, y), s.worldHasNChunks);
				chunks(curChunk).Update(dt);
			}
		}
	//obstacles[0]->Update(dt);
}
void World::Draw(Graphics& gfx) const
{
	Vei2 mos = Graphics::GetMidOfScreen();
	auto renderRect = GetRenderRect();
	int xStart = renderRect.left;
	int xStop = renderRect.right;
	int yStart = renderRect.top;
	int yStop = renderRect.bottom;

#ifdef _DEBUG 
	xStart = -1;
	xStop = 1;
	yStart = -1;
	yStop = 1;
#endif
	
	
	for (int layer = 0; layer < 4; layer++)
	{
		for (int y = yStart; y <= yStop; y++)
		{
			for (int x = xStart; x <= xStop; x++)
			{
				Vei2 curChunk = Chunk::PutChunkInWorld(mChunk + Vei2(x, y),s.worldHasNChunks);
				Vei2 curChunkPos = Vei2(x * s.chunkSize.x, -y * s.chunkSize.y) + Graphics::GetMidOfScreen() - Vei2(c.x, -c.y);
				RectF curChunkRect = RectF((Vec2)curChunkPos, s.chunkSize.x, s.chunkSize.y);

				switch (layer)
				{
				case 0:
					chunks(curChunk).DrawType(curChunkRect, gfx);
					break;
				case 1:
					if (grit || buildMode)
					{
						chunks(curChunk).DrawGroundedMap(curChunkPos, s.chunkSize.x / Settings::chunkHasNCells, gfx);
					}
					break;
				case 2:
					if (grit || buildMode)
					{
						chunks(curChunk).DrawGrit(curChunkPos, s.chunkSize.x / Settings::chunkHasNCells, gfx);
					}
					break;
				case 3:
					chunks(curChunk).DrawObstacles(curChunkRect, gfx);
					if (fcctPos.x == curChunk)
					{
						chunks(curChunk).DrawSurfaceAt(curChunkPos, fcctPos.y, s.chunkSize.x / Settings::chunkHasNCells, 1.5f, resC->tC.frames.at(0).GetCurSurface(), gfx);
					}
					gfx.DrawRect(GetChunkRect(mChunk), Colors::Red);
					break;
				}
			}
		}
	}
	if (buildMode )
	{
		if (chunks(fcctPosHover.x).ObstaclePosAllowed(fcctPosHover.y * Settings::CellSplitUpIn + fcctPosHover.z, placeObstacle))
		{
			chunks(fcctPosHover.x).DrawObstacleOutlines(fcctPosHover.y * Settings::CellSplitUpIn + fcctPosHover.z, placeObstacle, GetChunkRect(fcctPosHover.x), Colors::Green, gfx);
		}
		else
		{
			chunks(fcctPosHover.x).DrawObstacleOutlines(fcctPosHover.y * Settings::CellSplitUpIn + fcctPosHover.z, placeObstacle, GetChunkRect(fcctPosHover.x), Colors::Red, gfx);
		}
		chunks(fcctPosHover.x).DrawObstacle(fcctPosHover.y * Settings::CellSplitUpIn + fcctPosHover.z, placeObstacle, GetChunkRect(fcctPosHover.x), gfx);
	}
	if (focusedObst != nullptr)
	{
		chunks(focusedObst->chunkPos).DrawObstacleOutlines(focusedObst->tilePos, focusedObst->type, GetChunkRect(focusedObst->chunkPos), Colors::Blue, gfx);
	}
	if (moveMode)
	{
		Vei2 bottomLeft = chunkPos2Flat(fcctPos);
		CctPos curcctPos = Tile2ChunkPos(bottomLeft);
		for (int y = -moveRange; y <= moveRange; y++)
		{
			for (int x = -moveRange; x <= moveRange; x++)
			{
				CctPos cctDelta = { Vei2(0,0),Vei2(0,0),Vei2(x,y) };
				CctPos cctPos = PutCctPosInWorld(curcctPos + cctDelta);
				if (sqrt(pow(x, 2) + pow(y, 2)) <= moveRange)
				{
					chunks(cctPos.x).DrawTile(GetChunkRect(cctPos.x), cctPos.y * Settings::CellSplitUpIn + cctPos.z, Colors::Green, gfx);
				}
			}
		}
	}
}
void World::DrawObstacle(Vei2 tilePos, int type, Graphics& gfx, Color color, int frame)const
{
	Vec2 tileSize = GetTileSize();
	//RectI dRect = (RectI)GetTileRect(tilePos);
	RectI dRect = RectI(Vei2(100,100),100,100);
	dRect.right += (tileSize.x * (Settings::obstacleSizes[placeObstacle].x - 1));
	dRect.top -= (tileSize.y * (Settings::obstacleSizes[placeObstacle].y - 1));
	
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
}
bool World::NeedsConnections(Vei2 curXY)const
{
	auto ccPos = Cell2ChunkPos(curXY);
	if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
	{
		return false;
	}
	return true;
}
bool World::GenerateObstacle(Vei2 tilePos, int type, int ontoType, int surrBy)
{
	tilePos = PutTileInWorld(tilePos);
	Vei2 tileIsInCell = TileIsInCell(tilePos);
	auto ccPos = Cell2ChunkPos(tileIsInCell);
	Matrix<int> aMat = chunks(ccPos.x).GetAroundmatrix(ccPos.y);// GetAroundMatrix(tileIsInCell);
	//if (ObstaclePosAllowed(tilePos, type) && (ontoType == -1 || ontoType == cells(tileIsInCell).type) && (surrBy == -1 || aMat.HasValue(surrBy)))
	//{
		CctPos cctPos = Tile2ChunkPos(tilePos);
		chunks(cctPos.x).PlaceObstacle((Vei2)cctPos.y * Settings::CellSplitUpIn + cctPos.z, type);
		return true;
	//}
	//return false;
}

bool World::ObstaclePosAllowed(Vei2 tilePos, int type)
{
	auto tccPos = Tile2ChunkPos(PutTileInWorld(tilePos));
	if (chunks(tccPos.x).ObstaclePosAllowed(tccPos.y * Settings::CellSplitUpIn + tccPos.z, Settings::obstacleSizes[type]))
	{
		return true;
	}
	return false;
}
void World::Init(WorldSettings& s)
{
	this->s = s;
	mChunk = Vei2(10, s.worldHasNChunks.y / 2);
	for (int type = 1; type < tC->fields.size(); type++)		//Create connectionsmaps
	{
		//conMap.push_back(Matrix<int>(s.wSize.x, s.wSize.y, 0));
	}
	Zoom(Vei2(0,0));
}

void World::Generate(WorldSettings& s)
{
	chunks = Matrix<Chunk>(s.worldHasNChunks.x, s.worldHasNChunks.y, Chunk(Settings::chunkHasNCells, Cell(s.defType), &chunks,  resC)); //Welt erstellen & default value setzen
	for (int y = 0; y < chunks.GetColums(); y++)
	{
		for (int x = 0; x < chunks.GetRows(); x++)
		{
			chunks[x][y].SetChunkPos(Vei2(x, y));
		}
	}

	//cells = Matrix<Cell>(s.wSize.x, s.wSize.y, Cell(s.defType));		
	//obstacleMap = Matrix<int>(s.wSize.x * Settings::CellSplitUpIn, s.wSize.y * Settings::CellSplitUpIn, -1);
	
	switch (s.defBlueprint)
	{
	case 0:
		int arcticSize = (s.wSize.y / 40);
		int subArcticSize = (s.wSize.y / 7.5f);

		for (int i = 0; i < s.wSize.y; i++)					//north & south ice
		{
			GenerateExplosion(Vei2(i, 0), rng.GetNormalDist() * arcticSize, 2);
			GenerateExplosion(Vei2(i, s.wSize.y - 1), rng.GetNormalDist() * arcticSize, 2);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 50; i++)	//plants
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.y), 10 + rng.Calc(s.wSize.y - 20)), rng.GetNormalDist() * 3 + 5, 1);
		}
		for (int i = 0; i < s.wSize.x * 5; i++)	//snow
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.x), rng.GetNormalDist() * subArcticSize), rng.GetNormalDist() * 3, 5, -1);
			GenerateExplosion(Vei2(rng.Calc(s.wSize.x), s.wSize.y - rng.GetNormalDist() * subArcticSize), rng.GetNormalDist() * 3, 5, -1);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 200; i++)	//coral reef
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.y), 10 + rng.Calc(s.wSize.y - 20)), rng.GetNormalDist() * 3 + 3, 6, 0);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 6400; i++)	//candyland
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.y), 10 + rng.Calc(s.wSize.y - 20)), 3, 13, 1);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 1600; i++)	//desert
		{
			Vei2 pos = Vei2(rng.Calc(s.wSize.y), 10 + rng.Calc(s.wSize.y - 20));

			GenerateCircle(pos, 7, 3, 1);
			GenerateExplosion(pos, rng.GetNormalDist() * 6 + 10, 3, 1);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 3200; i++)	//rocks
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.y), 10 + rng.Calc(s.wSize.y - 20)), rng.GetNormalDist() * 6 + 5, 10, -1);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 3200; i++)	//canjon + savanne
		{
			Vei2 pos = Vei2(rng.Calc(s.wSize.x), 10 + rng.Calc(s.wSize.y - 20));
			GenerateExplosion(pos, 6 + rng.GetNormalDist() * 6, 9, 1);
			GenerateExplosion(pos, 4, 11, 9);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 6400; i++)	//magma
		{
			Vei2 pos = Vei2(rng.Calc(s.wSize.x), 10 + rng.Calc(s.wSize.y - 20));
			GenerateExplosion(pos, 6, 12, -1);
			GenerateExplosion(pos, 4, 8, 12);
		}
		UpdateConMap();
		UpdateGroundedMap();


		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 10; i++)	//nutritious plants
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.x), 10 + rng.Calc(s.wSize.y - 20)), rng.GetNormalDist() * 3, 4, 1, 20, 0);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 20; i++)	//swamp
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.x), 10 + rng.Calc(s.wSize.y - 20)), rng.GetNormalDist() * 6, 14, 0, 20);
		}

		
		for (int i = 0; i < (s.wSize.x * s.wSize.y); i++)	//Trees
		{
			Vei2 spawnAt = Vei2(rng.Calc((s.wSize.x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.wSize.y - 20) * Settings::CellSplitUpIn));
			auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn)));
			if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
				{
				int rT = rng.Calc(3);
				switch (rT)
				{
				case 0:
					GenerateObstacle(spawnAt, 1);
					break;
				case 1:
					GenerateObstacle(spawnAt, 4);
					break;
				case 2:
					GenerateObstacle(spawnAt, 8);
					break;
				}
			}
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) * 2; i++)	//cactus
		{
			Vei2 spawnAt = Vei2(rng.Calc((s.wSize.x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.wSize.y - 20) * Settings::CellSplitUpIn));
			auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn)));
			
			if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
			{
				GenerateObstacle(spawnAt, 5);
			}
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 20; i++)	//boxes
		{
			Vei2 spawnAt = Vei2(rng.Calc((s.wSize.x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.wSize.y - 20) * Settings::CellSplitUpIn));
			auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn)));
			
			if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
			{
				GenerateObstacle(spawnAt, 6, 3);
			}
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 10; i++)	//stones
		{
			Vei2 spawnAt = Vei2(rng.Calc((s.wSize.x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.wSize.y - 20) * Settings::CellSplitUpIn));
			auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn)));
			
			if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
			{
				int rS = rng.Calc(2);
				switch (rS)
				{
				case 0:
					//GenerateObstacle(spawnAt, 7);
					break;
				case 1:
					//GenerateObstacle(spawnAt, 9);
					break;
				}
			}
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 100; i++)	//other Trees
		{
			Vei2 spawnAt = Vei2(rng.Calc((s.wSize.x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.wSize.y - 20) * Settings::CellSplitUpIn));
			auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn)));
			
			if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
			{
				GenerateObstacleExplosion(spawnAt, 100, 1, 1, 15);
				GenerateObstacleExplosion(spawnAt + Vei2(0,1), 100, 1, 1, 15);
				GenerateObstacleExplosion(spawnAt + Vei2(1, 0), 100, 1, 1, 15);
				GenerateObstacleExplosion(spawnAt + Vei2(1, 1), 100, 1, 1, 15);

				GenerateObstacleExplosion(spawnAt, 50, 4, 1, 15);
				GenerateObstacleExplosion(spawnAt + Vei2(0, 1), 100, 4, 1, 15);
				GenerateObstacleExplosion(spawnAt + Vei2(1, 0), 100, 4, 1, 15);
				GenerateObstacleExplosion(spawnAt + Vei2(1, 1), 100, 4, 1, 15);
			}
		}
		
		break;
	}
	
	UpdateConMap();
	UpdateGroundedMap();

	SpawnPlayer();
	//CutHills(1);		//NICHT ZUENDE!!!!
}
void World::GenerateCircle(Vei2 pos, int radius,int type, int ontoType, int surrBy)
{
	for (int y = -radius; y < radius; y++)
	{
		for (int x = -radius; x < radius; x++)
		{
			if (sqrt(y * y + x * x) <= radius)
			{
				Vei2 putPos = PutCellInWorldX(pos + Vei2(x, y));
				if (CellIsInWorld(putPos))
				{
					Vei2 curCellPos = PutCellInWorldX(putPos);
					auto ccPos = Cell2ChunkPos(curCellPos);
					Cell& curCell = chunks(ccPos.x).GetCellAt(ccPos.y);

					auto aMat = chunks(ccPos.x).GetAroundmatrix(ccPos.y);
					
					if ((curCell.type == ontoType || -1 == ontoType) && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
					{
						curCell.type = type;
						GenerateCell(curCellPos, type, ontoType, surrBy);
					}
				}
			}
		}
	}
}
void World::GenerateLine(Vec2 p0, Vec2 p1, int type, int ontoType, int thickness, int surrBy) //bnot
{
	if (p0 != p1) {
		float m = 0.0f;
		if (p1.x != p0.x)
		{
			m = ((float)p1.y - p0.y) / ((float)p1.x - p0.x);
		}
		if (thickness > 1)
		{
			for (int i = -thickness / 2; i < thickness / 2; i++)
			{
				if (std::abs(m) <= 1.0f)
				{
					GenerateLine(Vec2(p1.x, p1.y + i), Vec2(p0.x, p0.y + i), type, ontoType,1, surrBy);
				}
				else
				{
					GenerateLine(Vec2(p1.x + i, p1.y), Vec2(p0.x + i, p0.y), type, ontoType,1, surrBy);
				}
			}
		}
		//	######
		if (p1.x != p0.x && std::abs(m) <= 1.0f)
		{
			if (p0.x > p1.x)
			{
				std::swap(p0, p1);
			}

			const float b = p0.y - m * p0.x;

			for (int x = (int)p0.x; x < (int)p1.x; x++)
			{
				const float y = m * (float)x + b;

				const int yi = (int)y;
				GenerateCell(Vei2(x, yi), type, ontoType, surrBy);
			}
		}
		else
		{
			if (p0.y > p1.y)
			{
				std::swap(p0, p1);
			}

			const float w = (p1.x - p0.x) / (p1.y - p0.y);
			const float p = p0.x - w * p0.y;

			for (int y = (int)p0.y; y < (int)p1.y; y++)
			{
				const float x = w * (float)y + p;

				const int xi = (int)x;
				
				GenerateCell(Vei2(xi, y), type, ontoType, surrBy);
			}
		}
	}
}
void World::GenerateObstacleLine(Vec2 tile0, Vec2 tile1, int type, int ontoType, int thickness, int surrBy) //bnot
{
	if (tile0 != tile1) {
		float m = 0.0f;
		if (tile1.x != tile0.x)
		{
			m = ((float)tile1.y - tile0.y) / ((float)tile1.x - tile0.x);
		}
		if (thickness > 1)
		{
			for (int i = -thickness / 2; i < thickness / 2; i++)
			{
				if (std::abs(m) <= 1.0f)
				{
					GenerateLine(Vec2(tile1.x, tile1.y + i), Vec2(tile0.x, tile0.y + i), type, ontoType, 1, surrBy);
				}
				else
				{
					GenerateLine(Vec2(tile1.x + i, tile1.y), Vec2(tile0.x + i, tile0.y), type, ontoType, 1, surrBy);
				}
			}
		}
		//	######
		if (tile1.x != tile0.x && std::abs(m) <= 1.0f)
		{
			if (tile0.x > tile1.x)
			{
				std::swap(tile0, tile1);
			}

			const float b = tile0.y - m * tile0.x;

			for (int x = (int)tile0.x; x < (int)tile1.x; x++)
			{
				const float y = m * (float)x + b;

				const int yi = (int)y;

				Vei2 tilePos = PutTileInWorld(Vei2(x, yi));
				auto tccPos = Tile2ChunkPos(tilePos);
				auto aMat = chunks(tccPos.x).GetAroundmatrix(tccPos.y);

				int curObstacleIndex = ObstacleMapAt(tilePos);
				if (curObstacleIndex != -1 && obstacles[curObstacleIndex]->type == ontoType)
				{
					DestroyObstacleAt(tilePos);
				}
				if (ObstacleMapAt(tilePos) == -1 && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
				{
					GenerateObstacle(tilePos, type);
				}
			}
		}
		else
		{
			if (tile0.y > tile1.y)
			{
				std::swap(tile0, tile1);
			}

			const float w = (tile1.x - tile0.x) / (tile1.y - tile0.y);
			const float p = tile0.x - w * tile0.y;

			for (int y = (int)tile0.y; y < (int)tile1.y; y++)
			{
				const float x = w * (float)y + p;

				const int xi = (int)x;
				
				Vei2 tilePos = PutTileInWorld(Vei2(xi, y));
				auto tccPos = Tile2ChunkPos(tilePos);
				auto aMat = chunks(tccPos.x).GetAroundmatrix(tccPos.y);
				int curObstacleIndex = ObstacleMapAt(tilePos);
				if (curObstacleIndex != -1 && obstacles[curObstacleIndex]->type == ontoType)
				{
					DestroyObstacleAt(tilePos);
				}
				if (ObstacleMapAt(tilePos) == -1 && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
				{
					GenerateObstacle(tilePos, type);
				}
			}
		}
	}
}
void World::GenerateObstacleExplosion(Vei2 pos, int maxLineLength, int type, int ontoType, int nRolls, int surrBy)
{
	for (int i = 0; i < nRolls; i++)
	{
		float rad = (float)rng.Calc(360) * 0.0174533f;
		Vec2 p1 = (Vec2)GigaMath::RotPointToOrigin<double>(1.0f, 0.0f, rad);
		Vei2 scaled = pos + (Vei2)((p1 * maxLineLength) * GigaMath::GetRandomNormDistribution());

		GenerateObstacle(scaled, type, ontoType, surrBy);
	}
}
void World::GenerateExplosion(Vei2 pos, int maxLineLength, int type,int ontoType, int nRolls, int surrBy)//not
{
	for (int i = 0; i < nRolls; i++)
	{
		float rad = (float)rng.Calc(360) * 0.0174533f;
		Vec2 p1 = (Vec2) GigaMath::RotPointToOrigin<float>(1.0f,0.0f, rad);
		Vei2 scaled = pos + Vei2(p1 * (maxLineLength*1/2+ rng.Calc(maxLineLength*1/2)));
		if (CellIsInWorld(scaled))
		{
			GenerateLine(Vec2(pos), Vec2(scaled), type, ontoType, 1, surrBy);
		}
	}
}
bool World::GenerateCell(Vei2 pos, int type, int ontoType, int surrBy)
{
	Vei2 curCellPos = PutCellInWorldX(pos);
	if (CellIsInWorld(curCellPos))
	{
		auto ccPos = Cell2ChunkPos(curCellPos);
		//Matrix<int> aMat = chunks(ccPos.x).GetAroundmatrix(ccPos.y);
		
		Matrix<int> aMat = chunks(ccPos.x).GetAroundmatrix(ccPos.y);
		if (surrBy == 0)
		{
			int test = 0;
		}
		if ((surrBy == -1 || aMat.HasValue(surrBy)) && (ontoType == -1 || chunks(ccPos.x).GetCellTypeAt(ccPos.y) == ontoType))
		{
			chunks(ccPos.x).SetTypeAt(ccPos.y, type);
			return true;
		}
	}
	return false;
}
void World::GenerateObstaclesInCell(Vei2 cellPos, int type, int number, int ontoType, int surrBy)
{
	int counter = 0;
	for (int i = 0; i < number && counter < number * 4;)
	{
		Vei2 obstPos = cellPos * Settings::CellSplitUpIn + Vei2(rng.Calc(Settings::CellSplitUpIn - 1), rng.Calc(Settings::CellSplitUpIn - 1));
		if (ObstaclePosAllowed(obstPos, type) && GenerateObstacle(obstPos, type, ontoType, surrBy))
		{
			i++;
		}
		counter++;
	}
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