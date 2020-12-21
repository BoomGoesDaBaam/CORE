#pragma once
#include "ResourceCollection.h"
#include "Vec3.h"
#include "Rect.h"
#include "GigaMath.h"
class Obstacle
{
protected:
	sharedResC resC;
public:
	Vei2 tilePos;						//pos in chunk
	Vei2 chunkPos;
	int type, state = 0;
	int n90rot = 0;
	std::vector<Animation> animations;	//index runs through states
	Obstacle(Vei2 tilePos, Vei2 chunkPos, int type, sharedResC resC)
		:
		tilePos(tilePos),
		chunkPos(chunkPos),
		type(type),
		resC(std::move(resC))
	{
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
	void MarkObstacleMap(Vei2 tilePos, Vei2 size, int index)
	{
		if (Settings::obstaclesOn)
		{
			for (int y = tilePos.y; y < tilePos.y + size.y; y++)
			{
				for (int x = tilePos.x; x < tilePos.x + size.x; x++)
				{
					if (x == Settings::CellSplitUpIn * hasNCells)
					{
						chunks->operator()(chunkPos + Vei2(1, 0)).MarkObstacleMap(Vei2(0, tilePos.y), Vei2(size.x - (x - tilePos.x), size.y), index);
						size.x = x - tilePos.x;
						continue;
					}
					if (y == Settings::CellSplitUpIn * hasNCells)
					{
						chunks->operator()(chunkPos + Vei2(0, 1)).MarkObstacleMap(Vei2(tilePos.x, 0), Vei2(size.x, size.y - (y - tilePos.y)), index);
						size.y = y - tilePos.y;
						continue;
					}
					//assert(obstacleMap.IndexInBounds(Vei2(x, y)));
					assert(obstacleMap(Vei2(x, y)) == -1);
					obstacleMap(Vei2(x, y)) = index;
					//assert(obstacleMap(Vei2(x, y)) != -1);
				}
			}
		}
	}

	bool SurrByCellType(int tilePos, int surrByCellType, int radius)
	{
		if (surrByCellType == -1)
		{
			return true;
		}
	}
	Vec2 GetCellSize(RectF chunkRect) const
	{
		return (Vec2)chunkRect.GetSize() / Vec2(hasNCells, hasNCells);
	}
	Vec2 GetTileSize(RectF chunkRect) const
	{
		return (Vec2)chunkRect.GetSize() / (Vec2(hasNCells, hasNCells) * Settings::CellSplitUpIn);
	}
	RectF GetCellRect(RectF chunkRect, Vei2 cellPos) const
	{
		Vec2 cellSize = GetCellSize(chunkRect);
		return  RectF(Vec2(chunkRect.left, chunkRect.top), cellSize.x, cellSize.y) + Vec2(cellPos.x, -cellPos.y - 1) * cellSize;
	}
	RectF GetTileRect(RectF chunkRect, Vei2 tilePos) const		//not finished
	{
		Vec2 tileSize = GetTileSize(chunkRect);
		RectF tileRect = RectF(Vec2(chunkRect.left, chunkRect.bottom), tileSize.x, tileSize.y) + Vec2(0, -tileSize.y);
		tileRect += (Vec2)tilePos * Vec2(tileSize.x, -tileSize.y);
		return tileRect;
	}
	VecN AdjustTilePos(Vei2 tilePos) const						//Generate the overall chunk/cell/tile position
	{
		tilePos = PutTileInChunk(tilePos);
		using namespace Settings;
		VecN m = VecN(3, 1, Vei2(-1, -1));
		m[0] = (tilePos / (CellSplitUpIn * hasNCells));			//chunkPos
		m[1] = (tilePos / CellSplitUpIn) % hasNCells;			//cellPos in chunkPos
		m[2] = tilePos % CellSplitUpIn;							//tilePos in cellPos 
		return m;
	}
	VecN ChunkSwitchIsNeeded(Vei2 tilePos) const				// -1 when not needed
	{
		VecN pos = AdjustTilePos(tilePos);
		if ((Vei2)pos[0] != chunkPos)
		{
			return pos;
		}
		return VecN(3, 1, Vei2(-1, -1));
	}
	bool ObstaclePosAllowed(Vei2 tilePos, int type) const
	{
		return ObstaclePosAllowed(tilePos, Settings::obstacleSizes[type]);
	}
	bool ObstaclePosAllowed(Vei2 tilePos, Vei2 size) const
	{
		if (Settings::obstaclesOn)
		{
			for (int y = 0; y < size.y; y++)
			{
				for (int x = 0; x < size.x; x++)
				{
					Vei2 curXY = tilePos + Vei2(x, y);
					if (curXY.x >= cells.GetColums() * Settings::CellSplitUpIn)
					{
						if (!chunks->operator()(chunkPos + Vei2(1, 0)).ObstaclePosAllowed(Vei2(0, curXY.y), Vei2(size.x - x, size.y)))
						{
							return false;
						}
						size.x = x - 1;
						continue;
					}
					if (curXY.y >= cells.GetRows() * Settings::CellSplitUpIn)
					{
						if (!chunks->operator()(chunkPos + Vei2(0, 1)).ObstaclePosAllowed(Vei2(curXY.x, 0), Vei2(size.x, size.y - y)))
						{
							return false;
						}
						size.y = y - 1;
						continue;
					}
					if (obstacleMap(curXY) != -1 || groundedMap(curXY) != 1)
					{
						return false;
					}

				}
			}
		}
		return true;
	}

	void DrawObstacleOutlines(Vei2 tilePos, int type, RectF chunkRect, Color c, Graphics& gfx) const
	{
		if (Settings::obstaclesOn)
		{
			Vec2 tileSize = GetTileSize(chunkRect);
			RectF drawPos = GetTileRect(chunkRect, tilePos);
			drawPos.top -= (Settings::obstacleSizes[type].y - 1) * tileSize.y;
			drawPos.right += (Settings::obstacleSizes[type].x - 1) * tileSize.x;
			gfx.DrawFilledRect(drawPos, c, SpriteEffect::Transparent(0.5f));
			gfx.DrawRect(drawPos, c);
		}
	}
	void DrawObstacle(Vei2 tilePos, int type, RectF chunkRect, Graphics& gfx) const
	{
		if (Settings::obstaclesOn)
		{
			Vec2 tileSize = GetTileSize(chunkRect);
			/*
			RectI drawPos = GetTileRect(chunkRect, tilePos);
			drawPos.top -= (Settings::obstacleSizes[type].y - 1) * tileSize.y;
			drawPos.right += (Settings::obstacleSizes[type].x - 1) * tileSize.x;
			*/
			Obstacle o = Obstacle(tilePos,chunkPos, type, resC);
			o.Draw(GetTileRect(chunkRect, tilePos)/*,RectF(Vec2(0,0), tileSize.x, tileSize.y)*/, tilePos, chunkRect, gfx);
		}
	}
	void DrawObstacles(RectF chunkRect, Graphics& gfx) const
	{
		if (Settings::obstaclesOn)
		{
			Vei2 mos = Graphics::GetMidOfScreen();
			SpriteEffect::Transparent e = SpriteEffect::Transparent(0.6f);

			Vec2 cellSize = GetCellSize(chunkRect);

			for (int y = 0; y < hasNCells; y++)
			{
				for (int x = 0; x < hasNCells; x++)
				{
					Vei2 curXY = Vei2(x, y);
					RectF curCellRect = GetCellRect(chunkRect, curXY);

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
									if (tileLayer == 0 && obstacleMap(curTilePos) < obstacles.size() && obstacles[obstacleMap(curTilePos)].state == 0)
									{
										obstacles[obstacleMap(curTilePos)].Draw(curRect, curTilePos, chunkRect, gfx);
									}
									if (tileLayer == 1 && obstacles[obstacleMap(curTilePos)].state == 1)
									{
										obstacles[obstacleMap(curTilePos)].Draw(curRect, curTilePos, chunkRect, gfx);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	void DrawType(RectF chunkRect, Graphics& gfx)const
	{
		for (int y = 0; y < hasNCells; y++)
		{
			for (int x = 0; x < hasNCells; x++)
			{
				Vei2 curXY = Vei2(x, y);
				//const Cell& curCell = cells(curXY);
				//int cellType = curCell.type;
				RectF curCellRect = GetCellRect(chunkRect, curXY);

				assert(cells(curXY).type >= 0 && cells(curXY).type < Settings::nDiffFieldTypes);
				gfx.DrawSurface((RectI)curCellRect, RectI(Vei2(0, 0), 50, 50), resC->tC.fields.at(cells(curXY).type).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));

				for (int i = 0; i < Settings::nDiffFieldTypes; i++)
				{
					int order = Settings::typeLayer[i];
					if (conMap[order][curXY.x][curXY.y] == 1)
					{
						gfx.DrawConnections(order, Vei2(curCellRect.left, curCellRect.top), aMats(curXY), resC->fsC.FieldCon, resC->tC.fields[order].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
					}
				}
			}
		}
	}
	void DrawGroundedMap(Vei2 pos, int cellSize, Graphics& gfx)const
	{
		using namespace Settings;
		for (int y = 0; y < hasNCells; y++)
		{
			for (int x = 0; x < hasNCells; x++)
			{
				Vei2 curXY = Vei2(x, y);
				RectI curCellPos = RectI(Vei2(pos.x, pos.y), cellSize, cellSize) + Vei2(x, -y - 1) * cellSize;

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
							SpriteEffect::Nothing e = SpriteEffect::Nothing();
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
	void DrawGrit(Vei2 pos, int cellSize, Graphics& gfx)const
	{
		using namespace Settings;
		for (int y = 0; y < hasNCells; y++)
		{
			for (int x = 0; x < hasNCells; x++)
			{
				Vei2 curXY = Vei2(x, y);
				//const Cell& curCell = cells(curXY);
				//int cellType = curCell.type;
				RectI curCellPos = RectI(Vei2(pos.x, pos.y), cellSize, cellSize) + Vei2(x, -y - 1) * cellSize;
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
		}
	}
	void DrawSurfaceAt(Vei2 drawPos, Vei2 cellPos, int cellSize, float scale, const Surface& s, Graphics& gfx)const
	{
		gfx.DrawSurface(RectI(drawPos, cellSize * scale, cellSize * scale) + cellPos * Vei2(cellSize, -cellSize) + Vei2(0, -cellSize) - Vei2((cellSize * scale - cellSize) / 2, (cellSize * scale - cellSize) / 2), s, SpriteEffect::Chroma());
	}

	void UpdateAroundMatrix(Matrix<int> mat)
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
	void UpdateGroundedMap()
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
	void Update(float dt)
	{
		if (Settings::obstaclesOn)
		{
			for (auto obstacle : obstacles)
			{
				obstacle.Update(dt);
			}
		}
	}

	bool PlaceObstacle(Vei2 tilePos, int type)
	{
		if (Settings::obstaclesOn)
		{
			assert(TileIsInChunk(tilePos));
			if (ObstaclePosAllowed(tilePos, type))
			{
				int index = obstacles.size();
				MarkObstacleMap(tilePos, Settings::obstacleSizes[type], index);
				obstacles.push_back(Obstacle(tilePos, chunkPos, type, resC));
				return true;
			}
			return false;
		}
		return true;
	}
	bool TileIsInChunk(Vei2& pos)const
	{
		return pos.x >= 0 && pos.x < cells.GetColums() * Settings::CellSplitUpIn && pos.y >= 0 && pos.y < cells.GetRows() * Settings::CellSplitUpIn;
	}
	Vei2 PutTileInChunk(Vei2 pos)const
	{
		return PutTileInChunk(pos.x, pos.y);
	}
	Vei2 PutTileInChunk(int x, int y)const
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

	Vec2_<Vei2> GetTilePosOutOfBounds(Vei2 tilePos) const		// 'x' = chunkPos		'y' = tilePos
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
		if (ctPos.x.y >= 0 && ctPos.x.x >= 0)
		{
			return ctPos;
		}
		return Vec2_<Vei2>(Vei2(0,0), Vei2(0, 0));
	}

	int GetGroundedOutOfBounds(Vei2 tilePos) const
	{
		auto ctPos = GetTilePosOutOfBounds(tilePos);
		return chunks->operator()(ctPos.x).groundedMap(ctPos.y);
	}
	int GetObstacleOutOfBounds(Vei2 tilePos) const
	{
		if (Settings::obstaclesOn)
		{
			auto ctPos = GetTilePosOutOfBounds(tilePos);
			return chunks->operator()(ctPos.x).obstacleMap(ctPos.y);
		}
	}
	void SetTypeAt(Vei2 pos, int type)
	{
		cells(pos) = type;
	}

	void SetConMapAt(Vei2 pos, int type, bool value)
	{
		conMap[type](pos) = value;
	}
	int GetCellTypeAt(Vei2 pos)const
	{
		assert(cells.IndexInBounds(pos));
		return cells(pos).type;
	}
	int GetGrounedMapAt(Vei2 pos)const
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
	int GetObstacleMapAt(Vei2 pos)const
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
	Obstacle* GetObstacleAt(Vei2 pos)
	{
		assert(obstacleMap(pos) != -1 && obstacleMap(pos) < obstacles.size());
		return &obstacles[obstacleMap(pos)];
	}
	Matrix<int> GetAroundmatrix(Vei2 pos)const
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
	void SetChunkPos(Vei2 pos)
	{
		chunkPos = pos;
	}
	void PlaceLadderableTiles(int type);
	Cell& GetCellAt(Vei2 cellPos)
	{
		return cells(cellPos);
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
};

