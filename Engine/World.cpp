#include "World.h"

World::World(WorldSettings wSettings, std::shared_ptr<ResourceCollection> resC, Vec2& camera) 
	:
	resC(std::move(resC)),
	c(camera),
	cells(wSettings.wSize.x, wSettings.wSize.y, wSettings.defType),
	tC(&this->resC->tC),
	fsC(&this->resC->fsC)
{
	Cell();
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
	return Vec2_<Vei2>(cellPos / s.chunkHasNCells, cellPos % s.chunkHasNCells);
}
Matrix<Vei2> World::Tile2ChunkPos(Vei2 tilePos)const
{
	tilePos = PutTileInWorld(tilePos);

	using namespace Settings;
	VecN m = VecN(3, 1, Vei2(-1, -1));
	m[0] = (tilePos / (CellSplitUpIn * s.chunkHasNCells));	//chunkPos
	m[1] = (tilePos / CellSplitUpIn) % s.chunkHasNCells;		//cellPos in chunk
	m[2] = tilePos % CellSplitUpIn;							//tilePos in cellPos 

	//*(Vei2*)m[1] = Vei2(2, 2);
	//return Vec2_<Vei2>(tilePos / CellSplitUpIn, tilePos % s.chunkHasNCells);
	return m;
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
Matrix<int> World::GetObstacleAroundMatrix(Vei2 cell)const
{
	Matrix<int> m = Matrix<int>(3, 3, 0);

	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			Vei2 tilePos = PutTileInWorld(Vei2(cell.x + x - 1, cell.y + y - 1));

			if (!TileIsInWorldY(y))
			{
				m[x][y] = -1;
			}
			else
			{
				if (obstacleMap(tilePos) != -1)
				{
					m[x][y] = obstacles[obstacleMap(tilePos)]->type;
				}
				else
				{
					m[x][y] = -1;
				}
			}
		}
	}
	return m;
}
void World::UpdateConMap()
{
	conMap.clear();
	for (int type = 0; type < Settings::nDiffFieldTypes;type++)
	{
		conMap.push_back(Matrix<int>(s.wSize.x, s.wSize.y, 0));
		for (int y = 0; y < s.wSize.y; y++)
		{
			for (int x = 0; x < s.wSize.x; x++)
			{
				auto ccP = Cell2ChunkPos(Vei2(x, y));

				if (type != cells(Vei2(x, y)).type && IsSurroundedBy(Vei2(x, y), type))
				{
					conMap[type][x][y] = 1;
					chunks(ccP.x).SetConMapAt(ccP.y, type, 1);
				}
				else
				{
					conMap[type][x][y] = 0;
					chunks(ccP.x).SetConMapAt(ccP.y, type, 0);
				}
			}
		}
	}
	for (int y = 0; y < s.worldHasNChunks.y; y++)
	{
		for (int x = 0; x < s.worldHasNChunks.x; x++)
		{
			Matrix<int> mat = Matrix<int>(s.chunkHasNCells + 2, s.chunkHasNCells + 2, -1);
			for (int yInner = 0; yInner < s.chunkHasNCells + 2; yInner++)
			{
				for (int xInner = 0; xInner < s.chunkHasNCells + 2; xInner++)
				{
					mat[xInner][yInner] = cells(PutCellInWorldX(x * s.chunkHasNCells + xInner - 1,y * s.chunkHasNCells + yInner - 1)).type;
				}
			}
			chunks[x][y].UpdateAroundMatrix(mat);
		}
	}
}
void World::UpdateGroundedMap(Vei2 pos, Vei2 size)
{
	using namespace Settings;
	if (size == Vei2(-1, -1) || size == s.wSize)
	{
		size = s.worldHasNChunks;
		groundedMap = Matrix<int>(s.wSize.x * CellSplitUpIn, s.wSize.y * CellSplitUpIn, -1);
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

	/*	
	assert(pos.y >= 0 && pos.y < s.wSize.y);

	using namespace Settings;
	if (size == Vei2(-1, -1) || size == s.wSize)
	{
		size = s.wSize;
		groundedMap = Matrix<int>(s.wSize.x * CellSplitUpIn, s.wSize.y * CellSplitUpIn, -1);
	}

	for (int y = pos.y; y < pos.y+size.y; y++)
	{
		for (int x = pos.x; x < pos.x+size.x; x++)
		{
			Vei2 pos = PutCellInWorldX(Vei2(x, y));
			assert(CellIsInWorld(pos));
			SetTilesAT(pos, -1);

			int curType = cells[pos.x][pos.y].type;
			if (Settings::anyGroundedTypes(curType))				//grounded
			{
				SetTilesAT(pos, 1);
				PlaceConnectionsIntoCelltiles(pos, 2, 2, -1, hillTypesARE);
			}
			else if(Settings::anyOfHillTypes(curType))	//hills
			{
				SetTilesAT(pos, 2);
			}
			else if (Settings::anyLiquidType(curType))	//liqids
			{
				PlaceConnectionsIntoCelltiles(pos, 2,2,-1, hillTypesARE);

				PlaceConnectionsIntoCelltiles(pos, 1,-1,-1, groundedTypesARE);
				PlaceConnectionsIntoCelltiles(pos, 1, -1, -1, maskTypesARE);
			}
			else if (Settings::anyMaskedType(curType))	//swamp
			{
				PlaceTilesForMaskedField(pos,1, -1, -1, 14);

				PlaceConnectionsIntoCelltiles(pos, 2, 2, -1, hillTypesARE);
				PlaceConnectionsIntoCelltiles(pos, 1, -1, -1, groundedTypesARE);
			}
		}
	}
	ChangeGroundedVal(-1, 0);
	PlaceLadderableTiles(3);
	*/
}

void World::Chunk::PlaceLadderableTiles(int type)
{
	for (int y = 0; y < cells.GetColums() * Settings::CellSplitUpIn; y++)
	{
		for (int x = 0; x < cells.GetRows() * Settings::CellSplitUpIn; x++)
		{
			if (groundedMap[x][y] == 2)
			{
				if (GetGrounedMapAt(Vei2(x, y + 2)) == 0 || GetGrounedMapAt(Vei2(x, y + 2)) == 1)
				{
					groundedMap[x][y] = type;
				}
				if (GetGrounedMapAt(Vei2(x, y - 2)) == 0 || GetGrounedMapAt(Vei2(x, y - 2)) == 1)
				{
					groundedMap[x][y] = type;
				}
				if (GetGrounedMapAt(Vei2(x + 2, y)) == 0 || GetGrounedMapAt(Vei2(x + 2, y)) == 1)
				{
					groundedMap[x][y] = type;
				}
				if (GetGrounedMapAt(Vei2(x - 2, y)) == 0 || GetGrounedMapAt(Vei2(x - 2, y)) == 1)
				{
					groundedMap[x][y] = type;
				}
			}
		}
	}
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
	if (obstacleMap(tilePos) != -1)
	{
		int index = obstacleMap(tilePos);
		Obstacle* curObst = obstacles[obstacleMap(tilePos)].get();
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
}
void World::Chunk::ChangeGroundedVal(int from, int to)
{
	for (int y = 0; y < cells.GetColums() * Settings::CellSplitUpIn; y++)
	{
		for (int x = 0; x < cells.GetRows() * Settings::CellSplitUpIn; x++)
		{
			if (groundedMap[x][y] == from)
				groundedMap[x][y] = to;
		}
	}
}
void World::Chunk::PlaceTilesForMaskedField(Vei2 pos, int value, int valOfMixed, int valueOfZero, int type)
{
	SubAnimation sa = SubAnimation(resC->tC.maskedFields[Settings::translateIntoMaskedType(type)],RectI(Vei2(0,0),50,50), RectI(Vei2(0, 0), 50, 50));

	Matrix<int> chromaM1 = sa.chromaM;
	chromaM1.Sort(value, valueOfZero);
	chromaM1.HalfSize(chromaM1, valOfMixed);
	chromaM1.MirrowVertical();
	//
	auto a = aMats(pos);
	std::vector<SubAnimation> sAVec;

	for (int i = 0; i < Settings::ArrSize(Settings::groundedTypesARE); i++)
	{
		int curType = Settings::groundedTypesARE[i];
		if (a.HasValue(curType))
		{
			if (conMap[curType][pos.x][pos.y] == 1)
			{
				std::vector<SubAnimation> newVecs;
				if (Settings::anyMaskedType(curType))
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, pos, true, a);
				}
				else
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, pos, false, a);
				}
				for (int c = 0; c < newVecs.size(); c++)
				{
					sAVec.push_back(newVecs[c]);
				}
			}
		}
	}
	Matrix<int> chromaM2 = SubAnimation::PutOnTopOfEachOther(sAVec, Vei2(50, 50), 1, 0);
	chromaM2.Sort(value, valueOfZero);
	chromaM2.HalfSize(chromaM2, valOfMixed);
	chromaM2.MirrowVertical();
	//
//	chromaM1.CombineWith(chromaM2);			//HIER
	SetTilesAT(pos, chromaM1);

}
void World::Chunk::PlaceConnectionsIntoCelltiles(Vei2 pos, int value, int valOfMixed, int valueOfZero, const int* types)
{
	auto a = aMats(pos);
	std::vector<SubAnimation> sAVec;

	for (int i = 0; i < Settings::ArrSize(types); i++)
	{
		int curType = types[i];
		if (a.HasValue(curType))
		{
			if (conMap[curType][pos.x][pos.y] == 1)
			{
				std::vector<SubAnimation> newVecs;
				if (Settings::anyMaskedType(curType))
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, pos, true, a);
				}
				else
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, pos, false, a);
				}
				for (int c = 0; c < newVecs.size(); c++)
				{
					sAVec.push_back(newVecs[c]);
				}
			}
		}
	}
	Matrix<int> chromaM = SubAnimation::PutOnTopOfEachOther(sAVec, Vei2(50, 50), 1, 0);
	chromaM.Sort(value, valueOfZero);
	chromaM.HalfSize(chromaM, valOfMixed);
	chromaM.MirrowVertical();
	SetTilesAT(pos, chromaM);
}
Matrix<int> World::GetAroundMatrix(Vei2 cell) const
{
	/*
	auto ccP = Cell2ChunkPos(cell);
	ccP.x = PutChunkInWorld(ccP.x,s.worldHasNChunks);
	ccP.y = ccP.y % 5;
	*/

	Matrix<int> m = Matrix<int>(3, 3, 0);

	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			Vei2 curCell = PutCellInWorldX(Vei2(cell.x + x - 1, cell.y + y - 1));

			if (!CellIsInWorldY(y))
			{
				m[x][y] = -1;
			}
			else
			{
				m[x][y] = cells(curCell).type;
			}
		}
	}
	return m;
	//return chunks(ccP.x).GetAroundmatrix(ccP.y);
}
std::vector<SubAnimation> World::GetConnectionsOfTypes(Vei2 pos, int* types)
{
	std::vector<SubAnimation> cons;
	for (int i = 0; i < Settings::ArrSize(types); i++)
	{
		std::vector<SubAnimation> newC = resC->fsC.GetConnectionAnimationVec(types[i], pos, false, GetAroundMatrix(pos));
		for (int n = 0; n < newC.size(); n++)
		{
			cons.push_back(newC[n]);
		}
	}
	return cons;
}
void World::Chunk::SetTilesAT(Vei2 pos, int value)
{
	for (int yInner = 0; yInner < Settings::CellSplitUpIn; yInner++)
	{
		for (int xInner = 0; xInner < Settings::CellSplitUpIn; xInner++)
		{
			groundedMap[(__int64)pos.x * Settings::CellSplitUpIn + xInner][(__int64)pos.y * Settings::CellSplitUpIn + yInner] = value;
		}
	}
}
void World::Chunk::SetTilesAT(Vei2 pos, Matrix<int> matrix)
{
	assert(matrix.GetColums() == Settings::CellSplitUpIn);
	assert(matrix.GetRows() == Settings::CellSplitUpIn);


	for (int yInCell = 0; yInCell < Settings::CellSplitUpIn; yInCell++)
	{
		for (int xInCell = 0; xInCell < Settings::CellSplitUpIn; xInCell++)
		{
			int curX = (__int64)pos.x * Settings::CellSplitUpIn + xInCell;
			int curY = (__int64)pos.y * Settings::CellSplitUpIn + yInCell;

			if (groundedMap[curX][curY] != 0)
			{
				if (matrix[xInCell][yInCell] != -1 && (groundedMap[curX][curY] == -1 || groundedMap[curX][curY] == 1))
				{
					groundedMap[curX][curY] = matrix[xInCell][yInCell];
				}
			}
		}
	}
}
bool World::IsSurroundedBy(Vei2 pos, int type)
{
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			if (cells(PutCellInWorldX(pos+Vei2(x-1,y-1))).type == type)
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

	return PutChunkInWorld(deltaChunks + mChunk, s.worldHasNChunks);
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

	v.x = PutChunkInWorld(deltaChunks + mChunk, s.worldHasNChunks);

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
	fsC->Update(s.chunkSize / s.chunkHasNCells);
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
	Vec2 mP = (Vec2)e.GetPos();
	fcctPos = GetHitTile(mP);

	if (e.GetType() == Mouse::Event::LRelease && !gH.IsLocked())
	{
		//fCell = GetCellHit(mP);
		//fTile = GetTileHit(mP);

		fccPos.x = GetChunkHit(mP);
		RectF pos = GetChunkRect(fccPos.x);
		Vei2 delta = Vei2(mP.x - pos.left, pos.bottom - mP.y);

		fccPos.y = delta / (s.chunkSize.x / s.chunkHasNCells);

		if (buildMode == true)
		{
			//GenerateObstacle(fTile, placeObstacle);
			auto fcctPos = GetHitTile(mP);
			chunks(fcctPos.x).PlaceObstacle(fcctPos.y * Settings::CellSplitUpIn + fcctPos.z, placeObstacle);
		}
		//chunks(GetChunkHit(mP)).GenerateObstacleAt(chunks(GetChunkHit(mP)).getHitTile(, 3);
		//GenerateObstacleExplosion(fTile, 50, 4);
		/*
		cells(fCell).type = 14;
		UpdateConMap();
		UpdateGroundedMap(Vei2(fCell.x-1,fCell.y-1),Vei2(3,3));
		*/
	}
	if (e.GetType() == Mouse::Event::WheelDown)
	{
		Zoom(Vei2(-50, -50));
	}
	if (e.GetType() == Mouse::Event::WheelUp)
	{
		Zoom(Vei2(50, 50));
	}
	//fTile = GetTileHit(mP);
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
	std::for_each(obstacles.begin(), obstacles.end(), [&](auto& obst)
	{
		obst->Update(dt);
	});
	//obstacles[0]->Update(dt);
}
void World::Draw(Graphics& gfx) const
{
	Vei2 mos = Graphics::GetMidOfScreen();
	int xStart = - 1 - (mos.x / s.chunkSize.x) * 1.5f;
	int xStop = 1 + (mos.x / s.chunkSize.x) * 1.5f;
	int yStart = - 1 - (mos.y / s.chunkSize.y) * 1.5f;
	int yStop = 1 + (mos.y / s.chunkSize.y) * 1.5f;

#ifdef _DEBUG 
	xStart = 0;
	xStop = 0;
	yStart = 0;
	yStop = 0;
#endif


	for (int layer = 0; layer < 4; layer++)
	{
		for (int y = yStart; y <= yStop; y++)
		{
			for (int x = xStart; x <= xStop; x++)
			{
				Vei2 curChunk = PutChunkInWorld(mChunk + Vei2(x, y),s.worldHasNChunks);
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
						chunks(curChunk).DrawGroundedMap(curChunkPos, s.chunkSize.x / s.chunkHasNCells, gfx);
					}
					break;
				case 2:
					if (grit || buildMode)
					{
						chunks(curChunk).DrawGrit(curChunkPos, s.chunkSize.x / s.chunkHasNCells, gfx);
						//DrawObstacle(fTile, placeObstacle, gfx);
					}
					break;
				case 3:
					chunks(curChunk).DrawObstacles(curChunkRect, gfx);
					if (fccPos.x == curChunk)
					{
						chunks(curChunk).DrawSurfaceAt(curChunkPos, fccPos.y, s.chunkSize.x / s.chunkHasNCells, 1.5f, resC->tC.frames.at(0).GetCurSurface(), gfx);
					}
					gfx.DrawRect(GetChunkRect(mChunk), Colors::Red);
					break;
				}
			}
		}
	}
	if (buildMode )
	{
		if (chunks(fcctPos.x).ObstaclePosAllowed(fcctPos.y * Settings::CellSplitUpIn + fcctPos.z, placeObstacle))
		{
			chunks(fcctPos.x).DrawObstacleOutlines(fcctPos.y * Settings::CellSplitUpIn + fcctPos.z, placeObstacle, GetChunkRect(fcctPos.x), Colors::Green, gfx);
		}
		else
		{
			chunks(fcctPos.x).DrawObstacleOutlines(fcctPos.y * Settings::CellSplitUpIn + fcctPos.z, placeObstacle, GetChunkRect(fcctPos.x), Colors::Red, gfx);
		}
		chunks(fcctPos.x).DrawObstacle(fcctPos.y * Settings::CellSplitUpIn + fcctPos.z, placeObstacle, GetChunkRect(fcctPos.x), gfx);
	}


	/*			3x3
	for (int y = 0; y <= 2; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
	*/


	/*
	Vei2 mos = Graphics::GetMidOfScreen();
	SpriteEffect::Transparent e = SpriteEffect::Transparent(0.6f);

	int xStart = -(mos.x / cSize.x) *2 - 1;
	int xStop = 1 + (mos.x / cSize.x) * 2;
	int yStart = -(mos.y / cSize.y) * 2;
	int yStop = 2 + (mos.y / cSize.y) * 2;

	#ifdef _DEBUG 
	xStart = -1;
	xStop = 1;
	yStart = 0;
	yStop = 2;
	#endif

	for (int layer = 0; layer <= 2; layer++)
	{
		for (int y = yStart; y <= yStop; y++)
		{
			for (int x = xStart; x <= xStop; x++)
			{
				Vei2 curXY = PutCellInWorldX(mCell + Vei2(x, y) + Vei2(0, -1));
				const Cell& curCell = cells(curXY);
				int cellType = curCell.type;
				RectI curCellPos = (RectI)GetCellRect(Vei2(x, y) + mCell);

				switch (layer)
				{
				case 0:
					assert(cellType >= 0 && cellType < Settings::nDiffFieldTypes);
					gfx.DrawSurface(curCellPos, RectI(Vei2(0, 0), 50, 50), tC->fields.at(cellType).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
					
					for (int i = 0; i < Settings::nDiffFieldTypes; i++)
					{
						int order = Settings::typeLayer[i];
						if (conMap[order][curXY.x][curXY.y] == 1)
						{
							gfx.DrawConnections(order, Vei2(curCellPos.left, curCellPos.top), GetAroundMatrix(curXY), fsC->FieldCon, tC->fields[order].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						}
					}
					break;
				case 1:
						using namespace Settings;
						for (int tileLayer = 0; tileLayer < 2; tileLayer++)
						{
							for (int xOnCell = 0; xOnCell < CellSplitUpIn; xOnCell++)
							{
								for (int yOnCell = 0; yOnCell < CellSplitUpIn; yOnCell++)
								{
									Vei2 v = (curXY * CellSplitUpIn + Vei2(xOnCell, yOnCell));
									assert(v.x < 5000 && v.x >= 0);
									assert(v.y < 5000 && v.y >= 0);


									float xPos = curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth();
									float yPos = curCellPos.bottom - ((float)(yOnCell + 1) / CellSplitUpIn) * curCellPos.GetHeight();
									RectF curP = RectF(Vec2(xPos, yPos), (float)std::ceil((double)curCellPos.GetWidth() / CellSplitUpIn), (float)std::ceil((double)curCellPos.GetHeight() / CellSplitUpIn));
									if (Graphics::GetScreenRect<float>().IsOverlappingWith(curP))
									{
										if (obstacleMap(v) != -1)
										{
											if (obstacleMap(PutTileInWorld(v - Vei2(1, 0))) != obstacleMap(v) && obstacleMap(PutTileInWorld(v - Vei2(0, 1))) != obstacleMap(v))
											{
												if (tileLayer == 1 && Settings::Obstacle2MultiObstacle(obstacles[obstacleMap(v)]->type) != -1)
												{
													obstacles[obstacleMap(v)]->Draw(gfx, GetTileRect(v));
												}
												if (tileLayer == 0 && Settings::Obstacle2MultiObstacle(obstacles[obstacleMap(v)]->type) == -1)
												{
													obstacles[obstacleMap(v)]->Draw(gfx, GetTileRect(v));
												}
											}
										}

										if (grit)
										{
											curP.PutInto(Graphics::GetScreenRect<float>());
											if (groundedMap(v) == 0)
											{
												gfx.DrawFilledRect(curP, Colors::Red, e);
											}
											else if (groundedMap(v) == 1)
											{
												//gfx.DrawRect(curP, Colors::Green, e);
											}
											else if (groundedMap(v) == -1)
											{
												gfx.DrawFilledRect(curP, Colors::Magenta, e);
											}
											else if (groundedMap(v) == 2)
											{
												gfx.DrawFilledRect(curP, Colors::Blue, e);
											}
											else if (groundedMap(v) == 3)
											{
												gfx.DrawFilledRect(curP, Colors::Cyan, SpriteEffect::Transparent(0.5f));
											}
											/*
											if (v == fTile)
											{
												gfx.DrawFilledRect(curP, Colors::Black, SpriteEffect::Transparent(0.5f));
											}
											''''
										}
									}
								}
							}
							if (grit || buildMode)
							{
								for (int xOnCell = 0; xOnCell < Settings::CellSplitUpIn; xOnCell++)
								{
									if (xOnCell == 0 || xOnCell == CellSplitUpIn)
									{
										gfx.DrawLine(Vec2((float)curCellPos.left, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), Vec2((float)curCellPos.right, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black), 3);
										gfx.DrawLine(Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.top), Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.bottom), SpriteEffect::OneColor(Colors::Black), 3);
									}
									else
									{
										gfx.DrawLine(Vec2((float)curCellPos.left, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), Vec2((float)curCellPos.right, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black));
										gfx.DrawLine(Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.top), Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.bottom), SpriteEffect::OneColor(Colors::Black));
									}
								}
							}

							if (buildMode)
							{
								DrawObstacle(fTile, placeObstacle, gfx, Colors::Magenta, 0);
								if (!posAllowed)
								{
									DrawObstacle(fTile, placeObstacle, gfx, Colors::Red, 0);
								}
								//gfx.DrawSurface(GetTileRect(fTile), Colors::Black, SpriteEffect::Rainbow());
							}
						}
					break;
				case 2:
					if (curXY == fCell)
					{
						gfx.DrawSurface(curCellPos.GetExpanded(cSize.x / 5), tC->frames.at(0).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
					}
					break;
				}
				
			}
		}
	}
	*/
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
	if (cells(curXY).type == 1)
	{
		return false;
	}
	return true;
}
bool World::FIDF(int first, int second)const
{
	if (first != second)
	{
		for (int i = 0; i < Settings::nDiffFieldTypes; i++)
		{
			if (Settings::typeLayer[i] == first || Settings::typeLayer[i] == second)
			{
				return Settings::typeLayer[i] == first;
			}
		}
	}
	return false;
}
void World::CutHills(int replaceTo)
{
	using namespace Settings;
	for (int y = 0; y < s.wSize.y; y++)
	{
		for (int x = 0; x < s.wSize.x; x++)
		{
			int arrSize = sizeof(hillTypesARE) / sizeof(hillTypesARE[0]);
			bool test = false;
			for (int i = 0; i < arrSize; i++)
			{
				if (cells[x][y].type == hillTypesARE[i])
				{
					test = true;
				}
			}
			if (test)
			{
				for (int i = 0; i < arrSize; i++)
				{
					auto aMat = GetAroundMatrix(Vei2(x, y));
					if (hillTypesARE[i] == aMat[1][1])
					{
						continue;
					}
					Vei2 problem = aMat.GetPosOfValue(hillTypesARE[i])[0];
					if (problem != Vei2(-1,-1))
					{
						Vei2 where = problem + Vei2(x, y) + Vei2(-1,-1);
						cells(where).type = replaceTo;
						i=0;
					}
				}
			}
		}
	}
}
bool World::GenerateObstacle(Vei2 tilePos, int type, int ontoType, int surrBy)
{
	tilePos = PutTileInWorld(tilePos);
	Vei2 tileIsInCell = TileIsInCell(tilePos);
	Matrix<int> aMat = GetAroundMatrix(tileIsInCell);
	//if (ObstaclePosAllowed(tilePos, type) && (ontoType == -1 || ontoType == cells(tileIsInCell).type) && (surrBy == -1 || aMat.HasValue(surrBy)))
	//{
		VecN cctP = Tile2ChunkPos(tilePos);
		
		//chunks(cctP[0]).GenerateObstacleAt((Vei2)cctP[1] * Settings::CellSplitUpIn + cctP[2], type, ontoType, surrBy,-1,-1,-1,-1);
		chunks(cctP[0]).PlaceObstacle((Vei2)cctP[1] * Settings::CellSplitUpIn + cctP[2], type);

		int index = obstacles.size();
		obstacles.emplace_back(std::make_unique<Obstacle>(tilePos, type, resC));
		
		for (int y = 0; y < Settings::obstacleSizes[type].y; y++)
		{
			for (int x = 0; x < Settings::obstacleSizes[type].x; x++)
			{
				Vei2 cPos = PutTileInWorld(tilePos + Vei2(x, y));
				assert(obstacleMap(Vei2(x, y)) == -1);
				obstacleMap(cPos) = index;
			}
		}
		return true;
	//}
	//return false;
}

bool World::ObstaclePosAllowed(Vei2 tilePos, int type)
{
	bool posAllowed = true;
	for (int y = 0; y < Settings::obstacleSizes[type].y; y++)
	{
		for (int x = 0; x < Settings::obstacleSizes[type].x; x++)
		{
			Vei2 tileInWorld = PutTileInWorld(tilePos + Vei2(x, y));
			
			if (obstacleMap(tileInWorld) != -1 || groundedMap(tileInWorld) != 1 || y < 0 || y >= s.wSize.y)
			{
				posAllowed = false;
			}
			
		}
	}
	return posAllowed;
}
void World::Init(WorldSettings& s)
{
	this->s = s;
	mChunk = Vei2(10, s.worldHasNChunks.y / 2);
	for (int type = 1; type < tC->fields.size(); type++)		//Create connectionsmaps
	{
		conMap.push_back(Matrix<int>(s.wSize.x, s.wSize.y, 0));
	}
	Zoom(Vei2(0,0));
}

void World::Generate(WorldSettings& s)
{
	chunks = Matrix<Chunk>(s.worldHasNChunks.x, s.worldHasNChunks.y, Chunk(s.chunkHasNCells, Cell(s.defType), &chunks,  resC)); //Welt erstellen & default value setzen
	for (int y = 0; y < chunks.GetColums(); y++)
	{
		for (int x = 0; x < chunks.GetRows(); x++)
		{
			chunks[x][y].SetChunkPos(Vei2(x, y));
		}
	}

	cells = Matrix<Cell>(s.wSize.x, s.wSize.y, s.defType);		
	obstacleMap = Matrix<int>(s.wSize.x * Settings::CellSplitUpIn, s.wSize.y * Settings::CellSplitUpIn, -1);
	
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
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 10; i++)	//nutritious plants
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.x), 10 + rng.Calc(s.wSize.y - 20)), rng.GetNormalDist() * 3, 4, 1, 20, 0);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 10; i++)	//nutritious plants
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.x), 10 + rng.Calc(s.wSize.y - 20)), rng.GetNormalDist() * 3, 14, 1, 20, 0);
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y); i++)	//nutritious plants
		{
			GenerateExplosion(Vei2(rng.Calc(s.wSize.x), 10 + rng.Calc(s.wSize.y - 20)), rng.GetNormalDist() * 3, 14, 0, 20, 14);
		}
		UpdateConMap();
		UpdateGroundedMap();


		
		for (int i = 0; i < (s.wSize.x * s.wSize.y); i++)	//Trees
		{
			Vei2 spawnAt = Vei2(rng.Calc((s.wSize.x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.wSize.y - 20) * Settings::CellSplitUpIn));
			if (cells(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn))).type == 1)
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
			if (cells(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn))).type == 1)
			{
				GenerateObstacle(spawnAt, 5);
			}
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 20; i++)	//boxes
		{
			Vei2 spawnAt = Vei2(rng.Calc((s.wSize.x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.wSize.y - 20) * Settings::CellSplitUpIn));
			if (cells(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn))).type == 1)
			{
				GenerateObstacle(spawnAt, 6, 3);
			}
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 10; i++)	//stones
		{
			Vei2 spawnAt = Vei2(rng.Calc((s.wSize.x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.wSize.y - 20) * Settings::CellSplitUpIn));
			if (cells(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn))).type == 1)
			{
				int rS = rng.Calc(2);
				switch (rS)
				{
				case 0:
					GenerateObstacle(spawnAt, 7);
					break;
				case 1:
					GenerateObstacle(spawnAt, 9);
					break;
				}
			}
		}
		for (int i = 0; i < (s.wSize.x * s.wSize.y) / 100; i++)	//other Trees
		{
			Vei2 spawnAt = Vei2(rng.Calc((s.wSize.x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.wSize.y - 20) * Settings::CellSplitUpIn));
			if (cells(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn))).type == 1)
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
					Cell& curCell = cells(curCellPos);
					auto aMat = GetAroundMatrix(putPos);
					
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
				auto aMat = GetAroundMatrix(tilePos);
				int curObstacleIndex = obstacleMap(tilePos);
				if (curObstacleIndex != -1 && obstacles[curObstacleIndex]->type == ontoType)
				{
					DestroyObstacleAt(tilePos);
				}
				if (obstacleMap(tilePos) == -1 && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
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
				auto aMat = GetAroundMatrix(tilePos);
				int curObstacleIndex = obstacleMap(tilePos);
				if (curObstacleIndex != -1 && obstacles[curObstacleIndex]->type == ontoType)
				{
					DestroyObstacleAt(tilePos);
				}
				if (obstacleMap(tilePos) == -1 && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
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
		Matrix<int> aMat = GetAroundMatrix(pos);
		if ((surrBy == -1 || aMat.HasValue(type)) && (ontoType == -1 || chunks(ccPos.x).GetCellTypeAt(ccPos.y) == ontoType) && aMat[1][1] != surrBy)
		{
			cells(PutCellInWorldX(pos)).type = type;
			chunks(ccPos.x).SetValueAt(ccPos.y, type);
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