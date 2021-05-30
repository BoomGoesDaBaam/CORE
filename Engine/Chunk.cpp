#include "Chunk.h"
void Chunk::PlaceLadderableTiles(int type)
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

void Chunk::ChangeGroundedVal(int from, int to)
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
int Chunk::countNumberOf(Vei2 where, int radius, int type)
{
	/*
	Vei2 bottomLeft = chunkPos2Flat(CtPos2CctPos(CtPos(chunkPos,where), cells.GetSize().x));
	CctPos curcctPos = Flat2ChunkPos(bottomLeft, GetWorldSizeInTiles());
	*/
	int count = 0;
	CctPos curcctPos = CtPos2CctPos(CtPos(chunkPos, where), cells.GetSize().x);
	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			//CctPos cctDelta = { Vei2(0,0),Vei2(0,0),Vei2(x,y) };
			CtPos ctPos = GetTilePosOutOfBounds(where + Vei2(x, y));
			//Vec2_<Vei2> ctPos = chunks(cctPos.x).GetTilePosOutOfBounds(cctPos.y * Settings::CellSplitUpIn + cctPos.z);
			if (radius > 0 && (x != 0 || y != 0) && sqrt(pow(x, 2) + pow(y, 2)) <= radius && chunks->operator()(ctPos.x).GetObstacleAt(ctPos.y)->type == type)
			{
				count++;
			}
		}
	}
	return count;
}
CctPos Chunk::CtPos2CctPos(CtPos pos, int chunkHasNCells)
{
	return CctPos(pos.x, pos.y / chunkHasNCells, pos.y % chunkHasNCells);
}
void Chunk::CastHeal(CtPos pos, int deltaHP, int radius)
{
	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			CtPos ctDelta = { Vei2(0,0),Vei2(x,y) };
			CtPos ctPos = PutCtPosInWorld(pos + ctDelta,chunks->GetSize());
			if(chunks->operator()(ctPos.x).GetObstacleMapAt(ctPos.y) != -1 && (x != 0 || y != 0) && sqrt(pow(x, 2) + pow(y, 2)) <= radius)
				chunks->operator()(ctPos.x).GetObstacleAt(ctPos.y)->Heal(deltaHP);
		}
	}
}
void Chunk::PlaceTilesForMaskedField(Vei2 pos, int value, int valOfMixed, int valueOfZero, int type)
{
	SubAnimation sa = SubAnimation(resC->tC.maskedFields[Settings::translateIntoMaskedType(type)], RectI(Vei2(0, 0), 50, 50), RectI(Vei2(0, 0), 50, 50));

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
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, true, a);
				}
				else
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, false, a);
				}
				for (int c = 0; c < (int)newVecs.size(); c++)
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
void Chunk::PlaceConnectionsIntoCelltiles(Vei2 pos, int value, int valOfMixed, int valueOfZero, const int* types)
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
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, true, a);
				}
				else
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, false, a);
				}
				for (int c = 0; c < (int)newVecs.size(); c++)
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
void Chunk::SetTilesAT(Vei2 pos, int value)
{
	for (int yInner = 0; yInner < Settings::CellSplitUpIn; yInner++)
	{
		for (int xInner = 0; xInner < Settings::CellSplitUpIn; xInner++)
		{
			groundedMap[(__int64)pos.x * Settings::CellSplitUpIn + xInner][(__int64)pos.y * Settings::CellSplitUpIn + yInner] = value;
		}
	}
}
void Chunk::SetTilesAT(Vei2 pos, Matrix<int> matrix)
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

void Chunk::MarkObstacleMap(Vei2 tilePos, Vei2 size, int index)
{
	if (Settings::obstaclesOn)
	{
		for (int y = tilePos.y; y < tilePos.y + size.y; y++)
		{
			for (int x = tilePos.x; x < tilePos.x + size.x; x++)
			{
				if (x >= Settings::CellSplitUpIn * hasNCells)
				{
					chunks->operator()(PutChunkInWorld(chunkPos + Vei2(1, 0), chunks->GetSize())).MarkObstacleMap(Vei2(0, tilePos.y), Vei2(size.x - (x - tilePos.x), size.y), index);
					size.x = x - tilePos.x;
					continue;
				}
				if (y >= Settings::CellSplitUpIn * hasNCells)
				{
					chunks->operator()(PutChunkInWorld(chunkPos + Vei2(0, 1), chunks->GetSize())).MarkObstacleMap(Vei2(tilePos.x, 0), Vei2(size.x, size.y - (y - tilePos.y)), index);
					size.y = y - tilePos.y;
					continue;
				}
				//assert(obstacleMap.IndexInBounds(Vei2(x, y)));
				//assert(obstacleMap(Vei2(x, y)) == -1);
				obstacleMap(Vei2(x, y)) = index;
				//assert(obstacleMap(Vei2(x, y)) != -1);
			}
		}
	}
}
bool Chunk::MoveObstacle(int index, Vec3_<Vei2> newPos)
{
	Vei2 oldPos = chunkPos2Flat(Vec3_<Vei2>(chunkPos, Vei2(obstacles[index]->tilePos.x / Settings::CellSplitUpIn, obstacles[index]->tilePos.y / Settings::CellSplitUpIn), Vei2(obstacles[index]->tilePos.x % Settings::CellSplitUpIn, obstacles[index]->tilePos.y % Settings::CellSplitUpIn)));
	int dist = (int)std::ceil(GetDistBetween2tiles(chunkPos2Flat(newPos), oldPos, chunks->GetColums() * Settings::chunkHasNTiles));

	Vei2 onMap = newPos.y * Settings::CellSplitUpIn + newPos.z;
	if (dist > 0 && chunks->operator()(newPos.x).ObstaclePosAllowed(onMap, obstacles[index]->type, index))
	{
		if (chunkPos == newPos.x)	//move inside a chunk
		{
			MarkObstacleMap(obstacles[index]->tilePos, Settings::obstacleStats[obstacles[index]->type].size[0], -1);
			obstacles[index]->tilePos = onMap;
			MarkObstacleMap(onMap, Settings::obstacleStats[obstacles[index]->type].size[0], index);
		}
		else     //jump into an other chunk
		{
			Obstacle o = *obstacles[index].get();
			DeleteObstacle(obstacles[index]->tilePos);
			o.chunkPos = newPos.x;
			o.tilePos = onMap;
			o.stepsLeft -= dist;
			chunks->operator()(newPos.x).PlaceObstacle(onMap, &o);
		}
		obstacles[index]->stepsLeft -= dist;
		return true;
		//PlaceObstacle(newPos, obstacles[index]->type);
	}
	return false;
}
Vec2 Chunk::GetCellSize(RectF chunkRect) const
{
	return (Vec2)chunkRect.GetSize() / Vec2((float)hasNCells, (float)hasNCells);
}
Vec2 Chunk::GetTileSize(RectF chunkRect) const
{
	return (Vec2)chunkRect.GetSize() / (Vec2((float)hasNCells, (float)hasNCells) * Settings::CellSplitUpIn);
}
RectF Chunk::GetCellRect(RectF chunkRect, Vei2 cellPos) const
{
	Vec2 cellSize = GetCellSize(chunkRect);
	return  RectF(Vec2(chunkRect.left, chunkRect.top), cellSize.x, cellSize.y) + Vec2((float)cellPos.x, (float)(-cellPos.y - 1)) * cellSize;
}
RectF Chunk::GetTileRect(RectF chunkRect, Vei2 tilePos) const
{
	Vec2 tileSize = GetTileSize(chunkRect);
	RectF tileRect = RectF(Vec2(chunkRect.left, chunkRect.bottom), tileSize.x, tileSize.y) + Vec2(0, -tileSize.y);
	tileRect += (Vec2)tilePos * Vec2(tileSize.x, -tileSize.y);
	return tileRect;
}
RectF Chunk::GetTileRect(Vei2 tilePos) const
{
	if (TileIsInChunk(tilePos))
	{
		return tilesRect(tilePos);
	}
	CtPos ctPos = PutCtPosInWorld(CtPos(chunkPos, tilePos), chunks->GetSize());
	chunks->operator()(ctPos.x).GetTileRect(ctPos.y);
}
bool Chunk::ObstaclePosAllowed(Vei2 tilePos, int type, int except) const
{
	return ObstaclePosAllowed(tilePos, Settings::obstacleStats[type].size[0], except);
}
bool Chunk::ObstaclePosAllowed(Vei2 tilePos, Vei2 size, int except) const
{
	if (this == nullptr)
	{
		return false;
	}
	if (Settings::obstaclesOn)
	{
		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				Vei2 curXY = tilePos + Vei2(x, y);
				using namespace Settings;
				if (RectI(Vei2(0, 0), chunkHasNTiles, chunkHasNTiles).Contains(curXY) && ((obstacleMap(curXY) != except && obstacleMap(curXY) != -1) || groundedMap(curXY) != 1))
				{
					return false;
				}
				/*
				else if (!RectI(Vei2(0, 0), chunkHasNTiles, chunkHasNTiles).Contains(curXY))
				{
					auto fcctPos = GetTilePosOutOfBounds(tilePos);
					if (!chunks->operator()(fcctPos.x).ObstaclePosAllowed(curXY, Vei2(1, 1)))
					{
						return false;
					}
				}
				*/

				if (curXY.x >= cells.GetColums() * Settings::CellSplitUpIn)
				{
					if (!chunks->operator()(PutChunkInWorld(chunkPos + Vei2(1, 0), chunks->GetSize())).ObstaclePosAllowed(Vei2(0, curXY.y), Vei2(size.x - x, size.y)))
					{
						return false;
					}
					size.x = x - 1;
					continue;
				}
				if (curXY.y >= cells.GetRows() * Settings::CellSplitUpIn)
				{
					if (!chunks->operator()(PutChunkInWorld(chunkPos + Vei2(0, 1), chunks->GetSize())).ObstaclePosAllowed(Vei2(curXY.x, 0), Vei2(size.x, size.y - y)))
					{
						return false;
					}
					size.y = y - 1;
					continue;
				}

			}
		}
	}
	return true;
}
bool Chunk::CheckIfObstacleBaseTile(Vei2 tilePos)
{
	return GetObstacleTypeOutOfBounds(tilePos) != GetObstacleTypeOutOfBounds(tilePos - Vei2(1, 0)) && GetObstacleTypeOutOfBounds(tilePos) != GetObstacleTypeOutOfBounds(tilePos - Vei2(0, 1));
}
bool Chunk::CheckIfObstacleBaseTileX(Vei2 tilePos)
{
	return GetObstacleTypeOutOfBounds(tilePos) != GetObstacleTypeOutOfBounds(tilePos - Vei2(1, 0));
}
bool Chunk::CheckIfObstacleBaseTileY(Vei2 tilePos)
{
	return GetObstacleTypeOutOfBounds(tilePos) != GetObstacleTypeOutOfBounds(tilePos - Vei2(0, 1));
}
int Chunk::GetObstacleIndex(Vei2 tilePos)const
{
	for (int i = 0; i < (int)obstacles.size(); i++)
	{
		if (obstacles[i]->tilePos == tilePos)
		{
			return i;
		}
	}
	return -1;
}
void Chunk::DeleteObstacle(Vei2 tilePos)
{
	if (CheckIfObstacleBaseTile(tilePos))
	{
		int index = obstacleMap(tilePos);
		MarkObstacleMap(tilePos, Settings::obstacleStats[obstacles[index]->type].size[0], -1);
		obstacles[index]->tilePos = Vei2(-1, -1);
		obstaclesIndexNotUsed.push_back(index);
	}
}
void Chunk::DrawObstacleOutlines(Vei2 tilePos, int type, RectF chunkRect, Color c, Graphics& gfx) const
{
	if (Settings::obstaclesOn)
	{
		Vec2 tileSize = GetTileSize(chunkRect);
		RectF drawPos = GetTileRect(chunkRect, tilePos);
		drawPos.top -= (Settings::obstacleStats[type].size[0].y - 1) * tileSize.y;
		drawPos.right += (Settings::obstacleStats[type].size[0].x - 1) * tileSize.x;
		gfx.DrawFilledRect(drawPos, c, SpriteEffect::Transparent(0.5f));
		gfx.DrawRect(drawPos, c);
	}
}
void Chunk::DrawObstacle(Vei2 tilePos, int type, RectF chunkRect, Graphics& gfx) const
{
	if (Settings::obstaclesOn)
	{
		Vec2 tileSize = GetTileSize(chunkRect);

		RectF drawPos = GetTileRect(chunkRect, tilePos);
		drawPos.top -= (Settings::obstacleStats[type].size[0].y - 1) * tileSize.y;
		drawPos.right += (Settings::obstacleStats[type].size[0].x - 1) * tileSize.x;

		Obstacle o(tilePos, chunkPos, type, resC);
		o.rect = drawPos;
		o.Draw(gfx);
	}
}
void Chunk::DrawObstacles(Graphics& gfx) const
{
	//chunkRect = chunkRect - Vec2(0, chunkRect.GetSize().y);
	//chunkRect -= GetTileSize(chunkRect) * nTilesOver;
	//gfx.DrawSurface((RectI)chunkRect, surf_obstacles, SpriteEffect::Chroma(), 0);

	if (Settings::obstaclesOn)
	{
		/*
		for (int y = 0; y < hasNCells; y++)
		{
			for (int x = 0; x < hasNCells; x++)
			{
			*/
			//Vei2 curXY = Vei2(x, y);
			//RectF curCellRect = GetCellRect(chunkRect, curXY);
		for (int layer = 0; layer < 2; layer++)
		{
			for (int i = 0; i < obstacles.size(); i++)
			{
				bool notUsed = false;
				if (std::any_of(obstaclesIndexNotUsed.begin(), obstaclesIndexNotUsed.end(), [&](int notUsed) {
					return notUsed == i;
					}))
				{
					continue;
				}
					if (obstacles[i]->state == 0 && layer == 0)
						obstacles[i]->Draw(gfx);
					if (obstacles[i]->state == 1 && layer == 1)
						obstacles[i]->Draw(gfx);
			}
		}
		/*
		using namespace Settings;
		for (int tileLayer = 0; tileLayer < 2; tileLayer++)
		{
			for (int tilePosX = 0; tilePosX < CellSplitUpIn; tilePosX++)
			{
				for (int tilePosY = 0; tilePosY < CellSplitUpIn; tilePosY++)
				{
					Vei2 curTilePos = Vei2(tilePosX, tilePosY) + curXY * Settings::CellSplitUpIn;
					float xPos = curCellRect.left + ((float)tilePosX / CellSplitUpIn) * curCellRect.GetWidth();
					float yPos = curCellRect.bottom - ((float)(tilePosY + 1) / CellSplitUpIn) * curCellRect.GetHeight();

					RectF curRect = RectF(Vec2(xPos, yPos), (float)std::ceil((double)curCellRect.GetWidth() / CellSplitUpIn), (float)std::ceil((double)curCellRect.GetHeight() / CellSplitUpIn));
					if (obstacleMap(curTilePos) != -1 && obstacleMap(curTilePos) != GetObstacleOutOfBounds(curTilePos - Vei2(1, 0)) && obstacleMap(curTilePos) != GetObstacleOutOfBounds(curTilePos - Vei2(0, 1)))
					{
						if (tileLayer == 0 && obstacleMap(curTilePos) < (int)obstacles.size() && obstacles[obstacleMap(curTilePos)]->state == 0)
						{
							obstacles[obstacleMap(curTilePos)]->Draw(curRect, curTilePos, chunkRect, gfx);
						}
						if (tileLayer == 1 && obstacles[obstacleMap(curTilePos)]->state == 1)
						{
							obstacles[obstacleMap(curTilePos)]->Draw(curRect, curTilePos, chunkRect, gfx);
						}
					}
				}
			}
		}
	}
}
*/
	}

}
void Chunk::DrawType(Graphics& gfx)const
{
	//chunkRect = chunkRect - Vec2(0, chunkRect.GetSize().y);
	//gfx.DrawSurface((RectI)chunkRect, surf_typesAndObstacles, SpriteEffect::Nothing(), 0);

	//Type
	for (int y = 0; y < hasNCells; y++)
	{
		for (int x = 0; x < hasNCells; x++)
		{
			Vei2 curXY = Vei2(x, y);
			//const Cell& curCell = cells(curXY);
			//int cellType = curCell->type;
			RectF curCellRect = cellsRect(curXY); //GetCellRect(chunkRect, curXY); 

			assert(cells(curXY).type >= 0 && cells(curXY).type < Settings::nDiffFieldTypes);
			gfx.DrawSurface((RectI)curCellRect, RectI(Vei2(0, 0), 50, 50), resC->tC.fields.at(cells(curXY).type).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));

			for (int i = 0; i < Settings::nDiffFieldTypes; i++)
			{
				int order = Settings::typeLayer[i];
				if (conMap[order][curXY.x][curXY.y] == 1)
				{
					gfx.DrawConnections(order, Vei2((int)curCellRect.left, (int)curCellRect.top), aMats(curXY), resC->fsC.FieldCon, resC->tC.fields[order].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
				}
			}
		}
	}
}
void Chunk::DrawGroundedMap(Graphics& gfx)const
{
	using namespace Settings;
	for (int y = 0; y < hasNCells; y++)
	{
		for (int x = 0; x < hasNCells; x++)
		{
			Vei2 curXY = Vei2(x, y);
			for (int xOnCell = 0; xOnCell < CellSplitUpIn; xOnCell++)
			{
				for (int yOnCell = 0; yOnCell < CellSplitUpIn; yOnCell++)
				{
					Vei2 curTileXY = (curXY * CellSplitUpIn + Vei2(xOnCell, yOnCell));
					assert(curTileXY.x < 5000 && curTileXY.x >= 0);
					assert(curTileXY.y < 5000 && curTileXY.y >= 0);

					RectF curP = tilesRect(curTileXY); //RectF(Vec2(xPos, yPos), (float)std::ceil((double)curCellPos.GetWidth() / CellSplitUpIn), (float)std::ceil((double)curCellPos.GetHeight() / CellSplitUpIn));
					if (Graphics::GetScreenRect<float>().IsOverlappingWith(curP))
					{
						SpriteEffect::Nothing e = SpriteEffect::Nothing();
						curP.PutInto(Graphics::GetScreenRect<float>());
						if (groundedMap(curTileXY) == 0)
						{
							gfx.DrawFilledRect(curP, Colors::Red, e);
						}
						else if (groundedMap(curTileXY) == 1)
						{
							//gfx.DrawRect(curP, Colors::Green, e);
						}
						else if (groundedMap(curTileXY) == -1)
						{
							gfx.DrawFilledRect(curP, Colors::Magenta, e);
						}
						else if (groundedMap(curTileXY) == 2)
						{
							gfx.DrawFilledRect(curP, Colors::Blue, e);
						}
						else if (groundedMap(curTileXY) == 3)
						{
							gfx.DrawFilledRect(curP, Colors::Cyan, SpriteEffect::Transparent(0.5f));
						}
						/*
						if (v == fTile)
						{
							gfx.DrawFilledRect(curP, Colors::Black, SpriteEffect::Transparent(0.5f));
						}
						*/
					}
				}
			}
			/*
		if (buildMode)
		{
			DrawObstacle(fTile, placeObstacle, gfx, Colors::Magenta, 0);
			if (!posAllowed)
			{
				DrawObstacle(fTile, placeObstacle, gfx, Colors::Red, 0);
			}
			//gfx.DrawSurface(GetTileRect(fTile), Colors::Black, SpriteEffect::Rainbow());
		}
		*/
		}
	}
}
void Chunk::DrawGrit(Graphics& gfx)const
{
	using namespace Settings;
	int chunkHasNTiles = hasNCells * CellSplitUpIn;
	for (int i = 0; i < chunkHasNTiles; i++)	//vertical Lines
	{
		Vec2 p1 = Vec2(tilesRect(Vei2(i, 0)).left, tilesRect(Vei2(i, 0)).bottom);
		Vec2 p2 = Vec2(tilesRect(Vei2(i, chunkHasNTiles - 1)).left, tilesRect(Vei2(i, chunkHasNTiles - 1)).top);
		if (i % CellSplitUpIn == 0)
		{
			gfx.DrawLine(p1, p2, SpriteEffect::OneColor(Colors::Black), 3);
		}
		else
		{
			gfx.DrawLine(p1, p2, SpriteEffect::OneColor(Colors::Black), 1);
		}
	}
	for (int i = 0; i < chunkHasNTiles; i++)	//horizontal Lines
	{
		Vec2 p1 = Vec2(tilesRect(Vei2(0, i)).left, tilesRect(Vei2(0, i)).top);
		Vec2 p2 = Vec2(tilesRect(Vei2(chunkHasNTiles - 1, i)).right, tilesRect(Vei2(chunkHasNTiles - 1, i)).top);
		if (i % CellSplitUpIn == 0)
		{
			gfx.DrawLine(p1, p2, SpriteEffect::OneColor(Colors::Black), 3);
		}
		else
		{
			gfx.DrawLine(p1, p2, SpriteEffect::OneColor(Colors::Black), 1);
		}
	}
	/*
	for (int y = 0; y < hasNCells; y++)
	{
		for (int x = 0; x < hasNCells; x++)
		{
			Vei2 curXY = Vei2(x, y);
			//const Cell& curCell = cells(curXY);
			//int cellType = curCell->type;
			RectI curCellPos = (RectI)cellsRect(curXY);	// RectI(Vei2(pos.x, pos.y), cellSize, cellSize) + Vei2(x, -y - 1) * cellSize;

			//Vei2 tileStart = Vei2(x, y) * Settings::CellSplitUpIn;
			for (int xOnCell = 0; xOnCell < Settings::CellSplitUpIn; xOnCell++)
			{
				if (xOnCell == 0 || xOnCell == CellSplitUpIn)
				{
					gfx.DrawLine(Vec2((float)curCellPos.left, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), Vec2((float)curCellPos.right, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black), 3);
					gfx.DrawLine(Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.top), Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.bottom), SpriteEffect::OneColor(Colors::Black), 3);
					//gfx.DrawLine(tilesRect()
				}
				else
				{
					gfx.DrawLine(Vec2((float)curCellPos.left, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), Vec2((float)curCellPos.right, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black));
					gfx.DrawLine(Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.top), Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.bottom), SpriteEffect::OneColor(Colors::Black));
				}
			}
		}
	}
	*/
}
void Chunk::DrawSurfaceAt(Vei2 drawPos, Vei2 cellPos, int cellSize, float scale, const Surface& s, Graphics& gfx)const
{
	gfx.DrawSurface(RectI(drawPos, (int)(cellSize * scale), (int)(cellSize * scale)) + cellPos * Vei2(cellSize, -cellSize) + Vei2(0, -cellSize) - Vei2((int)((cellSize * scale - cellSize) / 2), (int)((cellSize * scale - cellSize) / 2)), s, SpriteEffect::Chroma());
}
void Chunk::DrawTile(RectF chunkRect, Vei2 tilePos, Color c, Graphics& gfx)const
{
	gfx.DrawFilledRect(GetTileRect(chunkRect, tilePos), c, SpriteEffect::Transparent());
	gfx.DrawRect(GetTileRect(chunkRect, tilePos), c);
}
/*
void Chunk::UpdateTypeSurface(RectF chunkRect)
{
	graphicsWidth = (int)chunkRect.GetWidth();
	surf_typesAndObstacles = Surface((int)chunkRect.GetWidth(), (int)chunkRect.GetHeight());
	Vec2 cellSize = (Vec2)GetCellSize(chunkRect);
	Vec2 chunkSize = (Vec2)chunkRect.GetSize();
	std::vector<RectI> offset = resC->fsC.GetConOffset((Vei2)cellSize);

	for (int y = 0; y < hasNCells; y++)
	{
		for (int x = 0; x < hasNCells; x++)
		{
			Vei2 curXY = Vei2(x, y);
			UpdateTypeSurfaceCell(chunkRect, curXY, cellSize, chunkSize);
		}
	}
	graphicsUpToDate = 1;
}

void Chunk::UpdateTypeSurfaceCell(RectF chunkRect, Vei2 curXY, Vec2 cellSize, Vec2 chunkSize)
{
	int x = curXY.x;
	int y = curXY.y;

	const Cell& curCell = cells(curXY);
	int cellType = curCell->type;
	//RectI curCellRect = RectI(Vei2(curXY.x, surf_types.GetHeight()-curXY.y - 1) * cellSize,cellSize.x, cellSize.y);
	RectI curCellRect = RectI(Vei2(x * cellSize.x, chunkSize.y - cellSize.y - cellSize.y * y), cellSize.x, cellSize.y);
	curCellRect.right += 1;
	curCellRect.bottom += 1;
	if (Settings::anyMaskedType(cellType))
	{
		surf_typesAndObstacles.AddLayer(curCellRect, RectI(Vei2(0, 0), 50, 50), resC->tC.fields[0].GetCurSurface());
		surf_typesAndObstacles.AddLayer(curCellRect, RectI(Vei2(0, 0), 50, 50), resC->tC.fields[cellType].GetCurSurface());
	}
	surf_typesAndObstacles.AddLayer(curCellRect, RectI(Vei2(0, 0), 50, 50), resC->tC.fields[cellType].GetCurSurface());

	for (int i = 0; i < Settings::nDiffFieldTypes; i++)
	{
		int order = Settings::typeLayer[i];
		if (conMap[order][curXY.x][curXY.y] == 1)
		{
			//surf_types.AddLayer(curCellRect, RectI(Vei2(0, 0), 50, 50), resC->tC.fields[cellType].GetCurSurface());
			std::vector<SubAnimation> a = resC->fsC.GetConnectionAnimationVec(order, Settings::anyMaskedType(order), aMats(curXY));
			//Surface s = resC->fsC.GetConnectionAnimationVec(order, Settings::anyMaskedType(order), aMats(curXY))[0].a.GetFrames()[0];
			for (int n = 0; n < a.size(); n++)
			{
				int xStart = ((float)a[n].posIn50x50grit.left / 50) * (float)curCellRect.GetWidth();
				int yStart = ((float)a[n].posIn50x50grit.top / 50) * (float)curCellRect.GetHeight();

				surf_typesAndObstacles.AddLayer(RectI(Vei2(xStart, yStart) + curCellRect.GetTopLeft<int>(), curCellRect.GetWidth() / 2 + 1, curCellRect.GetHeight() / 2 + 1), a[n].sourceR, a[n].a.GetCurSurface());
			}

			//gfx.DrawConnections(order, Vei2(curCellRect.left, curCellRect.top), aMats(curXY), resC->fsC.FieldCon, resC->tC.fields[order].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
		}
	}

}

void Chunk::UpdateObstacleSurface(RectF chunkRect)
{
	Vec2 chunkSize = (Vec2)chunkRect.GetSize();
	Vec2 cellSize = (Vec2)GetCellSize(chunkRect);
	Vec2 tileSize = (Vec2)GetTileSize(chunkRect);
	/*
	surf_obstacles = Surface((int)chunkRect.GetWidth(), (int)chunkRect.GetHeight());
	for (int y = 0; y < surf_obstacles.GetHeight(); y++)
	{
		for (int x = 0; x < surf_obstacles.GetWidth(); x++)
		{
			surf_obstacles.PutPixel(x, y, Colors::Magenta);
		}
	}
	/*#####

	if (Settings::obstaclesOn)
	{
		Vei2 mos = Graphics::GetMidOfScreen();
		SpriteEffect::Transparent e = SpriteEffect::Transparent(0.6f);

		for (int y = 0; y < hasNCells; y++)
		{
			for (int x = 0; x < hasNCells; x++)
			{
				Vei2 curXY = Vei2(x, y);
				UpdateObstacleSurfaceCell(chunkRect, curXY, cellSize, (Vec2)chunkRect.GetSize(), tileSize);
			}
		}
	}
	graphicsUpToDate = 2;
}
void Chunk::UpdateObstacleSurfaceCell(RectF chunkRect, Vei2 curXY, Vec2 cellSize, Vec2 chunkSize, Vec2 tileSize)
{
	int x = curXY.x;
	int y = curXY.y;

	RectI curCellRect = RectI(Vei2(x * cellSize.x, chunkSize.y - cellSize.y - cellSize.y * y), cellSize.x, cellSize.y);

	using namespace Settings;
	for (int tileLayer = 0; tileLayer < 2; tileLayer++)
	{
		for (int tilePosX = 0; tilePosX < CellSplitUpIn; tilePosX++)
		{
			for (int tilePosY = 0; tilePosY < CellSplitUpIn; tilePosY++)
			{
				Vei2 curTilePos = Vei2(tilePosX, tilePosY) + curXY * Settings::CellSplitUpIn;
				//UpdateObstacleSurfaceTile(curTilePos.x, curTilePos.y, chunkRect);

				float xPos = curCellRect.left + ((float)tilePosX / CellSplitUpIn) * curCellRect.GetWidth();
				float yPos = curCellRect.bottom - ((float)(tilePosY + 1) / CellSplitUpIn) * curCellRect.GetHeight();

				RectF curRect = RectF(Vec2(xPos, yPos), (float)std::ceil((double)curCellRect.GetWidth() / CellSplitUpIn), (float)std::ceil((double)curCellRect.GetHeight() / CellSplitUpIn));

				if (obstacleMap(curTilePos) != -1 && obstacleMap(curTilePos) != GetObstacleTypeOutOfBounds(curTilePos - Vei2(1, 0)) && obstacleMap(curTilePos) != GetObstacleTypeOutOfBounds(curTilePos - Vei2(0, 1)))
				{
					Obstacle* obst = &obstacles[obstacleMap(curTilePos)];
					RectF surfSize = RectF(Vec2(0, 0), chunkSize.x, chunkSize.y);
					if (tileLayer == 0 && obstacleMap(curTilePos) < (int)obstacles.size() && obstacles[obstacleMap(curTilePos)]->state == 0)
					{
						curRect.right += tileSize.x * (Settings::obstacleSizes[obst->type].x - 1);
						curRect.top -= tileSize.y * (Settings::obstacleSizes[obst->type].y - 1);
						surf_typesAndObstacles.AddLayer((RectI)curRect, obst->animations[0].GetCurSurface().GetRect(), obst->animations[0].GetCurSurface());
						DrawObstacleProtrud(curRect, chunkSize, obst->animations[0].GetCurSurface());
					}
					if (tileLayer == 1 && obstacles[obstacleMap(curTilePos)]->state == 1)
					{
						int multiObstIndex = Settings::Obstacle2MultiObstacle(obst->type);
						curRect += Vec2(Settings::multiObstaclePos[multiObstIndex].x * tileSize.x, -Settings::multiObstaclePos[multiObstIndex].y * tileSize.y);
						Vei2 size = Settings::multiObstacleSize[multiObstIndex][(__int64)obst->state - 1];
						curRect.right += tileSize.x * (size.x - 1);
						curRect.top -= tileSize.y * (size.y - 1);
						surf_typesAndObstacles.AddLayer((RectI)curRect, obst->animations[1].GetCurSurface().GetRect(), obst->animations[1].GetCurSurface());
						//gfx.DrawSurface((RectI)tileRect, animations[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta), 0);
						//DrawObstacleProtrud(curRect,chunkSize, obst->animations[1].GetCurSurface());

						DrawObstacleProtrud(curRect, chunkSize, obst->animations[1].GetCurSurface());
					}
				}
			}
		}
	}
}
void Chunk::DrawObstacleProtrud(RectF curRect, Vec2 chunkSize,const Surface& s)
{
	RectF surfSize = RectF(Vec2(0, 0), chunkSize.x, chunkSize.y);
	bool  nLeft = surfSize.HasLeftProtrud(curRect);
	bool  nTop = surfSize.HasTopProtrud(curRect);
	bool  nRight = surfSize.HasRightProtrud(curRect);
	bool  nBottom = surfSize.HasBottomProtrud(curRect);

	if (nLeft)
	{
		Vei2 otherchunk = chunkPos + Vei2(-1, 0);
		chunks->operator()(PutChunkInWorld(otherchunk.x, otherchunk.y, chunks->GetSize()))->DrawObstacleOnBuffer(curRect + Vec2(chunkSize.x, 0), s);
	}
	if (nRight)
	{
		Vei2 otherchunk = chunkPos + Vei2(1, 0);
		chunks->operator()(PutChunkInWorld(otherchunk.x, otherchunk.y, chunks->GetSize()))->DrawObstacleOnBuffer(curRect - Vec2(chunkSize.x, 0), s);
	}
	if (nTop)
	{
		Vei2 otherchunk = chunkPos + Vei2(0, 1);
		chunks->operator()(PutChunkInWorld(otherchunk.x, otherchunk.y, chunks->GetSize()))->DrawObstacleOnBuffer(curRect + Vec2(0, chunkSize.x), s);
	}
	if (nBottom)
	{
		Vei2 otherchunk = chunkPos + Vei2(0, -1);
		chunks->operator()(PutChunkInWorld(otherchunk.x, otherchunk.y, chunks->GetSize()))->DrawObstacleOnBuffer(curRect - Vec2(0, chunkSize.x), s);
	}

	if (nTop && nLeft)
	{
		Vei2 otherchunk = chunkPos + Vei2(-1, 1);
		chunks->operator()(PutChunkInWorld(otherchunk.x, otherchunk.y, chunks->GetSize()))->DrawObstacleOnBuffer(curRect + Vec2(chunkSize.x, chunkSize.x), s);
	}
	if (nTop && nRight)
	{
		Vei2 otherchunk = chunkPos + Vei2(1, 1);
		chunks->operator()(PutChunkInWorld(otherchunk.x, otherchunk.y, chunks->GetSize()))->DrawObstacleOnBuffer(curRect + Vec2(-chunkSize.x, chunkSize.x), s);
	}
	if (nBottom && nLeft)
	{
		Vei2 otherchunk = chunkPos + Vei2(-1, -1);
		chunks->operator()(PutChunkInWorld(otherchunk.x, otherchunk.y, chunks->GetSize()))->DrawObstacleOnBuffer(curRect + Vec2(chunkSize.x, -chunkSize.x), s);
	}
	if (nBottom && nRight)
	{
		Vei2 otherchunk = chunkPos + Vei2(1, -1);
		chunks->operator()(PutChunkInWorld(otherchunk.x, otherchunk.y, chunks->GetSize()))->DrawObstacleOnBuffer(curRect + Vec2(-chunkSize.x, -chunkSize.x), s);
	}
}
void Chunk::DrawObstacleOnBuffer(RectF curRect,const Surface& s)
{
	surf_typesAndObstacles.AddLayer((RectI)curRect, s.GetRect(), s);
}
*/
void Chunk::UpdateRects(RectF chunkRect)
{
	cellsRect = Matrix<RectF>(cells.GetColums(), cells.GetRows(), RectF(Vec2(0, 0), 0, 0));
	tilesRect = Matrix<RectF>(cells.GetColums() * Settings::CellSplitUpIn, cells.GetRows() * Settings::CellSplitUpIn, RectF(Vec2(0, 0), 0, 0));

	for (int y = 0; y < hasNCells; y++)
	{
		for (int x = 0; x < hasNCells; x++)
		{
			Vei2 curXY = Vei2(x, y);
			cellsRect(curXY) = GetCellRect(chunkRect, curXY);

			Vei2 tileStart = curXY * Settings::CellSplitUpIn;
			for (int tilesY = 0; tilesY < Settings::CellSplitUpIn; tilesY++)
			{
				for (int tilesX = 0; tilesX < Settings::CellSplitUpIn; tilesX++)
				{
					Vei2 curTileXY = tileStart + Vei2(tilesX, tilesY);
					tilesRect(curTileXY) = GetTileRect(chunkRect, curTileXY) - Vec2(0, chunkRect.GetHeight());
				}
			}
		}
	}
	for (int i = 0; i < obstacles.size(); i++)
	{
		Vei2 pos = obstacles[i]->tilePos;
		obstacles[i]->UpdateRect(GetTileRect(chunkRect, pos) - Vec2(0, chunkRect.GetWidth()), pos, chunkRect);
	}
}
void Chunk::DeleteRects()
{
	//cellsRect = Matrix<RectF>(1, 1, RectF(Vec2(0, 0), 0, 0));
	//tilesRect = Matrix<RectF>(1, 1, RectF(Vec2(0, 0), 0, 0));
}
void Chunk::UpdateAroundMatrix(Matrix<int> mat)
{
	aMats = Matrix<Matrix<int>>(hasNCells, hasNCells, Matrix<int>());
	for (int y = 0; y < hasNCells; y++)
	{
		for (int x = 0; x < hasNCells; x++)
		{
			aMats[x][y] = mat.GetAroundMatrix(Vei2(x + 1, y + 1));
		}
	}
}
void Chunk::UpdateGroundedMap()
{
	using namespace Settings;
	groundedMap = Matrix<int>(cells.GetRows() * CellSplitUpIn, cells.GetColums() * CellSplitUpIn, -1);

	for (int y = 0; y < cells.GetRows(); y++)
	{
		for (int x = 0; x < cells.GetColums(); x++)
		{
			Vei2 pos = Vei2(x, y);


			int curType = cells[x][y].type;
			if (Settings::anyGroundedTypes(curType))				//grounded
			{
				SetTilesAT(pos, 1);
				PlaceConnectionsIntoCelltiles(pos, 2, 2, -1, hillTypesARE);
			}
			else if (Settings::anyOfHillTypes(curType))	//hills
			{
				SetTilesAT(pos, 2);
			}
			else if (Settings::anyLiquidType(curType))	//liqids
			{
				PlaceConnectionsIntoCelltiles(pos, 2, 2, -1, hillTypesARE);

				PlaceConnectionsIntoCelltiles(pos, 1, -1, -1, groundedTypesARE);
				PlaceConnectionsIntoCelltiles(pos, 1, -1, -1, maskTypesARE);
			}
			else if (Settings::anyMaskedType(curType))	//swamp
			{
				PlaceTilesForMaskedField(pos, 1, -1, -1, 14);

				PlaceConnectionsIntoCelltiles(pos, 2, 2, -1, hillTypesARE);
				PlaceConnectionsIntoCelltiles(pos, 1, -1, -1, groundedTypesARE);
			}
		}
	}
	ChangeGroundedVal(-1, 0);
}
void Chunk::Update(float dt)
{
	if (Settings::obstaclesOn)
	{
		for (int i = 0; i < obstacles.size(); i++)
		{
			if (std::any_of(obstaclesIndexNotUsed.begin(), obstaclesIndexNotUsed.end(), [&](int notUsed) {
				return notUsed == i;
				}))
			{
				continue;
			}
				obstacles[i]->Update(dt);
		}
	}
}

void Chunk::UpdateWhenMoved(RectF chunkRect)
{
	for (int i = 0; i < obstacles.size(); i++)
	{

		if (Settings::anyOfChangeSizeWhenNear(obstacles[i]->type) && UnitIsAround(obstacles[i]->tilePos, 5))
		{
			obstacles[i]->state = 0;
		}
		else if (Settings::anyOfChangeSizeWhenNear(obstacles[i]->type))
		{
			Vec2 chunkSize = (Vec2)chunkRect.GetSize();
			Vec2 cellSize = GetCellSize(chunkRect);
			Vec2 tileSize = GetTileSize(chunkRect);
			obstacles[i]->state = 1;
			//UpdateTypeSurfaceCell(chunkRect, obstacles[i]->tilePos / Settings::CellSplitUpIn, cellSize, chunkSize);
			//UpdateObstacleSurfaceCell(chunkRect, obstacles[i]->tilePos / Settings::CellSplitUpIn, cellSize, chunkSize, tileSize);

		}
	}
}
bool Chunk::UnitIsAround(Vei2 tilePos, int range)
{
	CtPos ctPos = CtPos(chunkPos, tilePos);
	for (int y = -range; y <= range; y++)
	{
		for (int x = -range; x <= range; x++)
		{
			CtPos ctDelta = { Vei2(0,0),Vei2(x,y) };
			CtPos curctPos = PutCtPosInWorld(ctPos + ctDelta, Vei2(chunks->GetColums(), chunks->GetRows()));
			if (chunks->operator()(curctPos.x).GetObstacleMapAt(curctPos.y) != -1)
			{
				Obstacle* obst = chunks->operator()(curctPos.x).GetObstacleAt(curctPos.y);
				if (sqrt(pow(x, 2) + pow(y, 2)) <= range && obst != nullptr && Settings::anyOfUnit(obst->type))
				{
					return true;
				}
			}
		}
	}
	return false;
}
bool Chunk::PlaceObstacle(Vei2 tilePos, int type, Team* team, int ontoType, int surrByObst)
{
	
	CtPos ctPos  = Chunk::CctPos2CtPos(Chunk::Flat2ChunkPos(tilePos, GetWorldSizeInTiles()));
	//Matrix<int> aMat = chunks->operator()(ctPos.x).GetAroundmatrix(ctPos.y / Vei2(Settings::CellSplitUpIn,Settings::CellSplitUpIn));// GetAroundMatrix(tileIsInCell);
	//if (ObstaclePosAllowed(tilePos, type) && (ontoType == -1 || ontoType == cells(tileIsInCell).type) && (surrBy == -1 || aMat.HasValue(surrBy)))
	//{
	if (Settings::spawnObstacles)
	{
		Obstacle obstacle = Obstacle(ctPos.y, ctPos.x, type, resC, team);
		return chunks->operator()(ctPos.x).PlaceObstacle(ctPos.y, &obstacle);
	}
	return true;
	//}
	//return false;
	/*
	tilePos = Chunk::PutTileInWorld(tilePos, s.wSizeInTiles);
	Vei2 tileIsInCell = TileIsInCell(tilePos);
	auto ccPos = Cell2ChunkPos(tileIsInCell);
	Matrix<int> aMat = chunks(ccPos.x).GetAroundmatrix(ccPos.y);// GetAroundMatrix(tileIsInCell);
	//if (ObstaclePosAllowed(tilePos, type) && (ontoType == -1 || ontoType == cells(tileIsInCell).type) && (surrBy == -1 || aMat.HasValue(surrBy)))
	//{
	CctPos cctPos = Chunk::Flat2ChunkPos(tilePos, s.wSizeInTiles);
	CtPos ctPos = Chunk::CctPos2CtPos(cctPos);
	if (Settings::spawnObstacles)
	{
		Obstacle obstacle = Obstacle(ctPos.y, ctPos.x, type, resC, team);
		return chunks(ctPos.x).PlaceObstacle((Vei2)cctPos.y * Settings::CellSplitUpIn + cctPos.z, &obstacle);
	}
	return true;
	//}
	//return false;
	*/
}
bool Chunk::PlaceObstacle(Vei2 tilePos, Obstacle* o)
{
	if (Settings::obstaclesOn && ObstaclePosAllowed(tilePos, o->type))
	{
		assert(TileIsInChunk(tilePos));
		if (obstaclesIndexNotUsed.size() == 0)
		{
			int index = obstacles.size();
			MarkObstacleMap(tilePos, Settings::obstacleStats[o->type].size[0], index);
			
			if (ConstructionSite* conObst = dynamic_cast<ConstructionSite*>(o))
			{
				obstacles.push_back(std::make_unique<ConstructionSite>(*conObst));
			}
			else
			{
				obstacles.push_back(std::make_unique<Obstacle>(*o));
			}
		}
		else
		{
			int index = obstaclesIndexNotUsed[0];
			obstaclesIndexNotUsed.erase(obstaclesIndexNotUsed.begin());
			MarkObstacleMap(tilePos, Settings::obstacleStats[o->type].size[0], index);
			if (ConstructionSite* conObst = dynamic_cast<ConstructionSite*>(o))
			{
				obstacles[index] = std::make_unique<ConstructionSite>(*conObst);
			}
			else
			{
				obstacles[index] = std::make_unique<Obstacle>(*o);
			}
		}
		return true;
	}
	return false;
}
bool Chunk::TileIsInChunk(Vei2& pos)const
{
	return pos.x >= 0 && pos.x < cells.GetColums()* Settings::CellSplitUpIn&& pos.y >= 0 && pos.y < cells.GetRows()* Settings::CellSplitUpIn;
}
Vei2 Chunk::PutTileInChunk(Vei2 pos)const
{
	return PutTileInChunk(pos.x, pos.y);
}
Vei2 Chunk::PutTileInChunk(int x, int y)const
{
	if (y < 0)
	{
		y = 0;
	}
	if (y >= cells.GetRows() * Settings::CellSplitUpIn)
	{
		y = cells.GetRows() * Settings::CellSplitUpIn - 1;
	}
	while (x < 0)
	{
		x += cells.GetColums() * Settings::CellSplitUpIn;
	}
	while (x >= cells.GetColums() * Settings::CellSplitUpIn)
	{
		x -= cells.GetColums() * Settings::CellSplitUpIn;
	}
	return Vei2(x, y);
}
CtPos Chunk::FindNearestPositionThatFits(Vei2 tilePos, int type)
{
	int range = 0;
	while (true)
	{
		CtPos ctMidPos = CtPos(chunkPos, tilePos);
		for (int y = -range; y <= range; y++)
		{
			for (int x = -range; x <= range; x++)
			{
				CtPos ctDelta = { Vei2(0,0),Vei2(x,y) };
				CtPos ctPos = PutCtPosInWorld(ctMidPos + ctDelta, chunks->GetSize());
				if (chunks->operator()(ctPos.x).ObstaclePosAllowed(ctPos.y,type) && (x != 0 || y != 0) && sqrt(pow(x, 2) + pow(y, 2)) <= range)
				{
					return ctPos;
				}
			}
		}
		range++;
	}
	return CtPos(Vei2(-1, -1), Vei2(-1, -1));
}
Vec2_<Vei2> Chunk::GetTilePosOutOfBounds(Vei2 tilePos) const		// 'x' = chunkPos		'y' = tilePos
{
	if (TileIsInChunk(tilePos))
	{
		return Vec2_<Vei2>(chunkPos, tilePos);
	}
	Vei2 deltaChunks = Vei2(0, 0);
	int chunkHasNTiles = hasNCells * Settings::CellSplitUpIn;
	while (tilePos.x < 0)
	{
		deltaChunks.x--;
		tilePos.x += chunkHasNTiles;
	}
	while (tilePos.x >= chunkHasNTiles)
	{
		deltaChunks.x++;
		tilePos.x -= chunkHasNTiles;
	}
	while (tilePos.y < 0)
	{
		deltaChunks.y--;
		tilePos.y += chunkHasNTiles;
	}
	while (tilePos.y >= chunkHasNTiles)
	{
		deltaChunks.y++;
		tilePos.y -= chunkHasNTiles;
	}
	Vec2_<Vei2> ctPos = Vec2_<Vei2>(chunkPos + deltaChunks, tilePos);
	ctPos.x = PutChunkInWorld(ctPos.x, chunks->GetSize());
	if (ctPos.x.y >= 0 && ctPos.x.x >= 0)
	{
		return ctPos;
	}
	return Vec2_<Vei2>(Vei2(-1, -1), Vei2(-1, -1));
}

int Chunk::GetGroundedOutOfBounds(Vei2 tilePos) const
{
	auto ctPos = GetTilePosOutOfBounds(tilePos);
	return chunks->operator()(ctPos.x).groundedMap(ctPos.y);
}
int Chunk::GetObstacleTypeOutOfBounds(Vei2 tilePos) const
{
	if (Settings::obstaclesOn)
	{
		if (tilePos.x == -1)
		{
			int k = 23;
		}
		auto ctPos = GetTilePosOutOfBounds(tilePos);
		return chunks->operator()(ctPos.x).obstacleMap(ctPos.y);
	}
	return -1;
}
Obstacle* Chunk::GetObstacleOutOfBounds(Vei2 tilePos) const
{
	if (Settings::obstaclesOn)
	{
		auto ctPos = GetTilePosOutOfBounds(tilePos);
		return chunks->operator()(ctPos.x).GetObstacleAt(ctPos.y);
	}
	return nullptr;
}
void Chunk::SetTypeAt(Vei2 pos, int type)
{
	cells(pos) = type;
}
void Chunk::NextTurnFirst(std::map<std::string, Team*> teams)
{
	for (int i = 0; i < obstacles.size(); i++)			//Update Map
	{
		if (obstacles[i]->education != nullptr && obstacles[i]->education->Educates())
		{
			obstacles[i]->education->TurnPassed();
			if (obstacles[i]->education->EducationFinished() && teams["player"]->GetMaterials().values["units"] + 1 <= teams["player"]->GetMaterials().values["maxUnits"])
			{
				int uunit = teams["player"]->GetMaterials().values["units"] + 1;
				int max = teams["player"]->GetMaterials().values["maxUnits"] + 1;
				CtPos spawnPoint = FindNearestPositionThatFits(obstacles[i]->tilePos, obstacles[i]->education->GetFinishedType());
				Obstacle obstacle = Obstacle(spawnPoint.y, spawnPoint.x, obstacles[i]->education->GetFinishedType(), resC, obstacles[i]->team);
				chunks->operator()(spawnPoint.x).PlaceObstacle(obstacle.tilePos, &obstacle);
				obstacles[i]->education->ResetTurns();
			}
		}
		if (obstacles[i]->heal != nullptr && obstacles[i]->heal->Isenabled())
		{
			CastHeal(GetMidPosOfObstacle(CtPos(chunkPos, obstacles[i]->tilePos), obstacles[i]->type, chunks->GetSize()),Settings::obstacleStats[obstacles[i]->type].healNumber, Settings::obstacleStats[obstacles[i]->type].healRange);
		}
	}
}
void Chunk::NextTurnSecond(std::map<std::string, Team*> teams)
{
	RandyRandom rr;
	for (int i = 0; i < obstacles.size(); i++)
	{
		obstacles[i]->stepsLeft = Settings::obstacleStats[obstacles[i]->type].movesPerTurn;
		if (obstacles[i]->attack != nullptr && obstacles[i]->attack->GetAttackMode())
		{
			int type = obstacles[i]->type;
			int much = Settings::obstacleStats[obstacles[i]->type].attacksPerTurn;
			obstacles[i]->attack->SetAttacksleft(Settings::obstacleStats[obstacles[i]->type].attacksPerTurn);
		}
		//Obstacle expandation
		if ((Settings::anyOfPlants(obstacles[i]->type) || Settings::anyOfAnimals(obstacles[i]->type)) && rr.Calc(Settings::probToGrow) == 1)
		{
			Vei2 pos = obstacles[i]->tilePos;
			Vec2 dir = GigaMath::GetRandomPointOnUnitCircle<float>();
			//dir = GigaMath::RotPointToOrigin<float>(dir.x, dir.y, )
			Vei2 newPos = pos + (Vei2)(dir * 4 + dir * 15 * GigaMath::GetRandomNormDistribution());
			CtPos ctPos = GetTilePosOutOfBounds(newPos);

			Obstacle obstacle = Obstacle(ctPos.y, ctPos.x, obstacles[i]->type, resC, teams["Fuer die Natur"]);
			chunks->operator()(ctPos.x).PlaceObstacle(ctPos.y, &obstacle);
		}
		//Add meterials
		if (obstacles[i]->team != nullptr)
			ApplyObstacleEffect(obstacles[i].get());
		//COnstructions finished
		if (ConstructionSite* conObst = dynamic_cast<ConstructionSite*>(obstacles[i].get()))
		{
			conObst->TurnPassed();
			if (conObst->BuildingFinished())
			{
				Obstacle obstacle = Obstacle(conObst->tilePos, conObst->chunkPos, conObst->GetTypeWhenFinished(), resC, conObst->team);
				DeleteObstacle(conObst->tilePos);
				PlaceObstacle(obstacle.tilePos, &obstacle);
			}
		}
	}
}
void Chunk::ApplyObstacleEffect(Obstacle* obstacle)
{
	Team* team = obstacle->team;
	switch (obstacle->type)
	{
	case 0:
		team->GetMaterials().Add({ {"maxUnits",2} });
		break;
	case 2:
		CastHeal(CtPos(obstacle->chunkPos, obstacle->tilePos), 20, 10);
		break;
	case 10:
		team->GetMaterials().Add({ {"units",1} });
		break;
	}
}
void Chunk::AttackTile(CctPos pos, Obstacle* attacker)
{
	if (pos.x != chunkPos)
	{
		chunks->operator()(pos.x).AttackTile(pos, attacker);
	}
	Vei2 tilePos = pos.y * Settings::CellSplitUpIn + pos.z;
	if (obstacleMap(tilePos) != -1)
	{
		obstacles[obstacleMap(tilePos)]->hp -= attacker->GetDmg();
	}
}
void Chunk::SetConMapAt(Vei2 pos, int type, bool value)
{
	conMap[type](pos) = value;
}
int Chunk::GetCellTypeAt(Vei2 pos)const
{
	assert(cells.IndexInBounds(pos));
	return cells(pos).type;
}
int Chunk::GetGrounedMapAt(Vei2 pos)const
{
	if (RectI(Vei2(0, 0), groundedMap.GetColums(), groundedMap.GetRows()).Contains(pos))
	{
		return groundedMap(pos);
	}

	Vei2 chunkAt = chunkPos;
	while (pos.y < 0)
	{
		pos.y += cells.GetRows() * Settings::CellSplitUpIn;
		chunkAt.y--;
	}
	while (pos.y >= cells.GetRows() * Settings::CellSplitUpIn)
	{
		pos.y -= cells.GetRows() * Settings::CellSplitUpIn;
		chunkAt.y++;
	}
	while (pos.x < 0)
	{
		pos.x += cells.GetColums() * Settings::CellSplitUpIn;
		chunkAt.x--;
	}
	while (pos.x >= cells.GetColums() * Settings::CellSplitUpIn)
	{
		pos.x -= cells.GetColums() * Settings::CellSplitUpIn;
		chunkAt.x++;
	}
	return chunks->operator()(PutChunkInWorld(chunkAt, Vei2(chunks->GetColums(), chunks->GetRows()))).groundedMap(pos);
}
int Chunk::GetObstacleMapAt(Vei2 pos)const
{
	if (Settings::obstaclesOn)
	{
		if (RectI(Vei2(0, 0), obstacleMap.GetColums(), obstacleMap.GetRows()).Contains(pos))
		{
			return obstacleMap(pos);
		}

		Vei2 chunkAt = chunkPos;
		while (pos.y < 0)
		{
			pos.y += cells.GetRows() * Settings::CellSplitUpIn;
			chunkAt.y--;
		}
		while (pos.y >= cells.GetRows() * Settings::CellSplitUpIn)
		{
			pos.y -= cells.GetRows() * Settings::CellSplitUpIn;
			chunkAt.y++;
		}
		while (pos.x < 0)
		{
			pos.x += cells.GetColums() * Settings::CellSplitUpIn;
			chunkAt.x--;
		}
		while (pos.x >= cells.GetColums() * Settings::CellSplitUpIn)
		{
			pos.x -= cells.GetColums() * Settings::CellSplitUpIn;
			chunkAt.x++;
		}
		return chunks->operator()(PutChunkInWorld(chunkAt, Vei2(chunks->GetColums(), chunks->GetRows()))).obstacleMap(pos);
	}
	return -1;
}
Obstacle* Chunk::GetObstacleAt(Vei2 pos)
{
	assert(obstacleMap(pos) != -1);
	Vei2 baseTile = pos;
	while (!CheckIfObstacleBaseTileX(baseTile))
	{
		baseTile.x--;
	}
	while (!CheckIfObstacleBaseTileY(baseTile))
	{
		baseTile.y--;
	}
	auto CtPos = GetTilePosOutOfBounds(baseTile);
	if (TileIsInChunk(baseTile))
	{
		return obstacles[obstacleMap(baseTile)].get();
	}
	return chunks->operator()(CtPos.x).GetObstacleAt(CtPos.y);//GetObstacleOutOfBounds(baseTile);//&obstacles[obstacleMap(pos)];
}
Matrix<int> Chunk::GetAroundmatrix(Vei2 pos)const
{
	if (aMats.GetSize() != Vei2(1, 1) && aMats.IndexInBounds(pos))
	{
		return aMats(pos);
	}
	else
	{
		return Matrix<int>(3, 3, -1);
	}
}
void Chunk::SetChunkPos(Vei2 pos)
{
	chunkPos = pos;
}
Cell& Chunk::GetCellAt(Vei2 cellPos)
{
	return cells(cellPos);
}