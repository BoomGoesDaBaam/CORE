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
#pragma once

class World
{
public:
	class WorldSettings
	{
	public:
		WorldSettings()
		{
#ifdef _DEBUG 
			worldHasNChunks = Vei2(3, 3);
#endif
			wSize = Vei2(worldHasNChunks.x * chunkHasNCells, worldHasNChunks.y * chunkHasNCells);
		}
		int defBlueprint = 0;
		int defType = 0;
		Vei2 wSize;
		int chunkHasNCells = 5;
		Vei2 worldHasNChunks = Vei2(25, 25);
		Vei2 chunkSize = { 250, 250 };
	};
private:
	class Obstacle
	{
	protected:
		sharedResC resC;
	public:
		Vei2 tilePos;				//pos is bottomleft
		int type, state = 0;
		int n90rot = 0, curSurf = 0;
		float timePassed = 0.0f;
		Obstacle(Vei2 tilePos, int type, sharedResC resC)
			:
			tilePos(tilePos),
			type(type),
			resC(std::move(resC))
		{
			switch (type)
			{
			case 1:
			case 4:
				state = 1;
				break;

			}
			n90rot = (tilePos.x + tilePos.y) % 4;
			if (this->resC->tC.obstacles.size() > 0)
			{
				timePassed = (float)((tilePos.x + tilePos.y) % 10) / 10 * this->resC->tC.obstacles[0].GetKeepTime();
			}
		}

		void Draw(Graphics& gfx, RectF tileRect)const			//	'tileRect' = Rect of tile where (Vei2(0, -1) && Vei2(-1, 0) != index) == true
		{
			if (state == 0)
			{
				Vec2 tileSize = Vec2(tileRect.GetWidth(), tileRect.GetHeight());
				tileRect.right += tileSize.x * (Settings::obstacleSizes[type].x - 1);
				tileRect.top -= tileSize.y * (Settings::obstacleSizes[type].y - 1);
				gfx.DrawSurface((RectI)tileRect, resC->tC.obstacles[type].GetSurfaceAt(curSurf), SpriteEffect::Chroma(Colors::Magenta), n90rot);
			}
			else
			{
				Vec2 tileSize = Vec2(tileRect.GetWidth(), tileRect.GetHeight());
				int multiObstIndex = Settings::Obstacle2MultiObstacle(type);
				tileRect += Vec2(Settings::multiObstaclePos[multiObstIndex].x * tileSize.x, -Settings::multiObstaclePos[multiObstIndex].y * tileSize.y);
				Vei2 size = Settings::multiObstacleSize[multiObstIndex][(__int64)state - 1];
				tileRect.right += tileSize.x * (size.x - 1);
				tileRect.top -= tileSize.y * (size.y - 1);
				gfx.DrawSurface((RectI)tileRect, resC->tC.multiObstacles[multiObstIndex].GetSurfaceAt(curSurf), SpriteEffect::Chroma(Colors::Magenta), n90rot);
			}
		}
		void Update(float dt)
		{

			timePassed += dt;
			if (state == 0)
			{
				while (timePassed > resC->tC.obstacles[type].GetKeepTime())
				{
					curSurf++;
					timePassed = 0;
				}
				if (curSurf >= resC->tC.obstacles[type].GetNumberOfFrames())
				{
					curSurf = 0;
				}
			}
			if (state == 1)
			{
				while (timePassed > resC->tC.multiObstacles[Settings::Obstacle2MultiObstacle(type)].GetKeepTime())
				{
					curSurf++;
					timePassed = 0;
				}
				if (curSurf >= resC->tC.multiObstacles[Settings::Obstacle2MultiObstacle(type)].GetNumberOfFrames())
				{
					curSurf = 0;
				}
			}

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
		std::vector<Matrix<int>> conMap;
		Matrix<int> groundedMap;
		Matrix<Matrix<int>> aMats;
		Matrix<int> obstacleMap;				// '-1' = empty   val < -1 = index of obstacle in obstacleVec
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
			obstacleMap(Matrix<int>(hasNCells * Settings::CellSplitUpIn, hasNCells * Settings::CellSplitUpIn, -1))
		{
			conMap.clear();
			for (int i = 0; i < Settings::nDiffFieldTypes; i++)
			{
				conMap.push_back(Matrix<int>(hasNCells, hasNCells, 0));
			}
		}
		/*
		RectF GetTileRect(Vei2 tileP)const;
		Vei2 GetCellHit(Vec2 mP)const;
		Vei2 GetTileHit(Vec2 mP)const
		{
			Vec2 mos = (Vec2)Graphics::GetMidOfScreen();
			Vei2 cell = GetCellHit(mP);
			RectF cellRect = GetCellRect(cell);
			Vei2 delta = Vei2(mP.x - cellRect.left, cellRect.bottom - mP.y);
			//assert(delta.x > 0 && delta.y > 0);
			Vec2 tileSize = GetTileSize();
			Vei2 hitTile = Vei2(cell.x * Settings::CellSplitUpIn + delta.x / tileSize.x, (cell.y - 1) * Settings::CellSplitUpIn + delta.y / tileSize.y);

			return hitTile;
		}
		*/
		bool GenerateObstacleAt(Vei2 tilePos, int type, int ontoCellType = -1, int ontoGroundedTile = -1, int ontoObstacle = -1, int surrByCellType = -1, int surrByGroundedTile = -1, int surrByObstacle = -1)
		{
			
			//auto a = AdjustTilePos(tilePos);
			VecN rPos = ChunkSwitchIsNeeded(tilePos);
			if ((Vei2)rPos[0] != Vei2(-1,-1))
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
				obstacles.push_back(Obstacle(tilePos, type, resC));
				assert(tilePos.x >= 0 && tilePos.x < 125);
				MarkObstacleMap(tilePos, Settings::obstacleSizes[type], index);

				/*
				for (int y = 0; y < Settings::obstacleSizes[type].y; y++)
					{
					for (int x = 0; x < Settings::obstacleSizes[type].x; x++)
						{
							Vei2 cPos = PutTileInWorld(tilePos + Vei2(x, y));
							obstacleMap(cPos) = index;
						}
					}
					return true;
				*/

				}
		
				return false;
		}
		void MarkObstacleMap(Vei2 tilePos, Vei2 size, int index)
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

		bool SurrByCellType(int tilePos, int surrByCellType, int radius)
		{

			if (surrByCellType == -1)
			{
				return true;
			}

		}
		Vei2 GetCellSize(RectI chunkRect) const
		{
			return chunkRect.GetSize() / Vei2(hasNCells, hasNCells);
		}
		Vei2 GetTileSize(RectI chunkRect) const
		{
			return chunkRect.GetSize() / (Vei2(hasNCells, hasNCells) * Settings::CellSplitUpIn);
		}
		RectI GetCellRect(RectI chunkRect, Vei2 cellPos) const
		{
			Vei2 cellSize = GetCellSize(chunkRect);
			return  RectI(Vei2(chunkRect.left, chunkRect.top), cellSize.x, cellSize.y) + Vei2(cellPos.x, -cellPos.y - 1) * cellSize;
		}
		RectI GetTileRect(RectI chunkRect, Vei2 tilePos) const		//not finished
		{
			Vei2 tileSize = GetTileSize(chunkRect);
			return RectI(Vei2(chunkRect.left, chunkRect.top), tileSize.x, tileSize.y) + Vei2(tileSize.x, -tileSize.y - 1) * tileSize;
		}
		VecN AdjustTilePos(Vei2 tilePos) const						//Generate the overall chunk/cell/tile position
		{
			tilePos = PutTileInWorld(tilePos);
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

		void DrawObstacles(RectI chunkRect, Graphics& gfx) const
		{

			Vei2 mos = Graphics::GetMidOfScreen();
			SpriteEffect::Transparent e = SpriteEffect::Transparent(0.6f);

			Vei2 cellSize = GetCellSize(chunkRect);

			int xStart = -(mos.x / cellSize.x) * 2 - 1;
			int xStop = 1 + (mos.x / cellSize.x) * 2;
			int yStart = -(mos.y / cellSize.y) * 2;
			int yStop = 2 + (mos.y / cellSize.y) * 2;

#ifdef _DEBUG 
			xStart = -1;
			xStop = 1;
			yStart = 0;
			yStop = 2;
#endif

			for (int y = 0; y < hasNCells; y++)
			{
				for (int x = 0; x < hasNCells; x++)
				{
					Vei2 curXY = Vei2(x, y);
					//const Cell& curCell = cells(curXY);
					//int cellType = curCell.type;
					RectI curCellRect = GetCellRect(chunkRect, curXY);
					/*
					Vei2 curXY = PutCellInWorldX(mCell + Vei2(x, y) + Vei2(0, -1));
					const Cell& curCell = cells(curXY);
					int cellType = curCell.type;
					RectI curCellPos = (RectI)GetCellRect(Vei2(x, y) + mCell);
					*/

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
								if (Graphics::GetScreenRect<float>().IsOverlappingWith(curRect))
								{
									/*
									assert(!obstacleMap.HasValue(1));
									if (obstacleMap.GetPosOfValue(1).size() > 0)
									{

									}
									*/
									if (obstacleMap(curTilePos) != -1)
									{
										gfx.DrawRect(curRect, Colors::Blue);
										VecN switchV1 = ChunkSwitchIsNeeded(curTilePos - Vei2(1, 0));
										VecN switchV2 = ChunkSwitchIsNeeded(curTilePos - Vei2(0, 1));
										bool v1 = false;
										bool v2 = false;


										if ((Vei2) switchV1[0] == Vei2(-1, -1))
										{
											v1 = obstacleMap(curTilePos - Vei2(1, 0)) != obstacleMap(curTilePos);
										}
										else
										{
											assert(chunks->IndexInBounds((Vei2)switchV1[0]));
											Vei2 size = chunks->operator()((Vei2)switchV1[0]).obstacleMap.GetSize();
											assert(chunks->operator()((Vei2)switchV1[0]).obstacleMap.IndexInBounds((Vei2)switchV1[1] * Settings::CellSplitUpIn + (Vei2)switchV1[2]));
											//assert(obstacleMap.IndexInBounds(curTilePos));
											v1 = chunks->operator()((Vei2)switchV1[0]).obstacleMap((Vei2)switchV1[1] * Settings::CellSplitUpIn + (Vei2)switchV1[2]);
										}

										if ((Vei2)switchV2[0] == Vei2(-1, -1))
										{
											v2 = obstacleMap(curTilePos - Vei2(1, 0)) != obstacleMap(curTilePos);
										}
										else
										{
											v2 = chunks->operator()((Vei2)switchV2[0]).obstacleMap((Vei2)switchV2[1] * Settings::CellSplitUpIn + (Vei2)switchV2[2]);
										}


										if (v1 && v2)
										{
											if (tileLayer == 1 && Settings::Obstacle2MultiObstacle(obstacles[obstacleMap(curTilePos)].type) != -1)
											{
												obstacles[obstacleMap(curTilePos)].Draw(gfx, (RectF)GetTileRect(chunkRect, curTilePos));
											}
											if (tileLayer == 0 && Settings::Obstacle2MultiObstacle(obstacles[obstacleMap(curTilePos)].type) == -1)
											{
												obstacles[obstacleMap(curTilePos)].Draw(gfx, (RectF)GetTileRect(chunkRect, curTilePos));
											}
										}
									}
									
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
		}
		void DrawType(RectI chunkRect, Graphics& gfx)const
		{
			for (int y = 0; y < hasNCells; y++)
			{
				for (int x = 0; x < hasNCells; x++)
				{
					Vei2 curXY = Vei2(x, y);
					//const Cell& curCell = cells(curXY);
					//int cellType = curCell.type;
					RectI curCellRect = GetCellRect(chunkRect, curXY);

					assert(cells(curXY).type >= 0 && cells(curXY).type < Settings::nDiffFieldTypes);
					gfx.DrawSurface(curCellRect, RectI(Vei2(0, 0), 50, 50), resC->tC.fields.at(cells(curXY).type).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));

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
					//const Cell& curCell = cells(curXY);
					//int cellType = curCell.type;
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
								/*
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
								*/
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
		

		bool TileIsInWorldd(Vei2& pos)const
		{
			return pos.x >= 0 && pos.x < cells.GetColums() * Settings::CellSplitUpIn && pos.y >= 0 && pos.y < cells.GetRows() * Settings::CellSplitUpIn;
		}
		Vei2 PutTileInWorld(Vei2 pos)const
		{
			return PutTileInWorld(pos.x, pos.y);
		}
		Vei2 PutTileInWorld(int x, int y)const
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

		void SetValueAt(Vei2 pos, int type)
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
		int GetGrounedMapAt(Vei2 pos)
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
			return chunks->operator()(PutChunkInWorld(chunkAt,Vei2(chunks->GetColums(), chunks->GetRows()))).groundedMap(pos);
		}
		Matrix<int> GetAroundmatrix(Vei2 pos)const
		{
			if (aMats.IndexInBounds(pos))
			{
				return aMats(pos);
			}
			else
			{
				return Matrix<int>(3,3,-1);
			}
		}
		void SetChunkPos(Vei2 pos)
		{
			chunkPos = pos;
		}
		void PlaceLadderableTiles(int type);
	};
	//Resourcen
	std::shared_ptr<ResourceCollection> resC;
	TexturesCollection* tC;
	FramesizeCollection* fsC;
	RandyRandom rng;
	WorldSettings s;
	//Gamevars
	Vei2 fCell = { 0,0 };					//angeklickte Zelle
	Vei2 mChunk = { 0,0 };
	Vei2 fTile = { 0,0 };
	Vec2_<Vei2> fccPos = { {0,0},{0,0} };

	Vec2& c;								//Camera

	//Zelle in der Mitte des Bildschirms auf dem Debugzeiger ist
	Vei2 mCunk = { 0,0 };
	Matrix<Cell> cells;
	Matrix<Chunk> chunks;
	std::vector<Matrix<int>> conMap;		//Connectionmap	 (1 = needsConnections, 0 = does not		index for type)
	Matrix<int> groundedMap;				// '0' = spot is not grounded, '1' = is grounded, '-1' = not identified yet (will be 0 if not changed), '2' = hill
	Matrix<int> obstacleMap;				// '-1' = empty   < -1 index of obstacle in obstacleVec
	std::vector<std::unique_ptr<Obstacle>> obstacles;
	
	//std::vector<Team> enemys;
	//Team player;

	bool grit = false;						//show grit
	bool buildMode = false;					//place something
	int placeObstacle = 0;
	bool posAllowed = true;

	Team player = Team("Die reinlich raeudigen Raucher");

	Vei2 GetChunkHit(Vec2 mP)const;
	RectF GetChunkRect(Vei2 pos)const;


	bool CellIsInWorld(Vei2& pos)const;
	bool TileIsInWorld(Vei2& pos)const;
	bool CellIsInWorldY(int y)const;		
	bool TileIsInWorldY(int y)const;

	bool ChunkIsInWorld(Vei2& cellPos)const;

	Vec2_<Vei2> Cell2ChunkPos(Vei2 CellPos)const;			//	'x' = chunkPos	'y' = cellPos in chunk
	Matrix<Vei2> Tile2ChunkPos(Vei2 cellPos)const;	//	1x3 Matrix		'0' = chunkPos		'1' = cellPos		'2' = tilePos

	Vei2 PutCellInWorldX(Vei2 pos)const;		//Calculates coordinates when x negativ or > cSize.x  
	Vei2 PutCellInWorldX(int x, int y)const;

	static Vei2 World::PutChunkInWorld(Vei2 pos, Vei2 worldHasNChunks)
	{
		return PutChunkInWorld(pos.x, pos.y, worldHasNChunks);
		//return Vei2(pos.x % worldHasNChunks.y, pos.y % worldHasNChunks.y);
	}
	static Vei2 World::PutChunkInWorld(int x, int y, Vei2 worldHasNChunks)
	{
		return Vei2(GigaMath::NegMod(x, worldHasNChunks.x), GigaMath::NegMod(y, worldHasNChunks.y));
	}
	Vei2 PutTileInWorld(Vei2 pos)const;
	Vei2 PutTileInWorld(int x, int y)const;
	
	Vei2 TileIsInCell(Vei2 tilePos);

	void DestroyObstacleAt(Vei2 tilePos);
	Matrix<int> GetAroundMatrix(Vei2 cell)const;
	Matrix<int> GetObstacleAroundMatrix(Vei2 cell)const;
	void UpdateConMap();							//
	void UpdateGroundedMap(Vei2 pos=Vei2(0,0), Vei2 size = Vei2(-1,-1));						// VERY performance heavy - UpdateConMap must be called before UpdateGroundedMap
	//void SetTilesAT(Vei2 pos, int value);
	//void SetTilesAT(Vei2 pos, Matrix<int> matrix);	//sets tile(x0,y0) in Cell(x1,y1) to type when matrix at(x0,y0) != 0
	bool IsSurroundedBy(Vei2 pos, int type);		//3x3 around pos
	//Private not const Funktions
	void Zoom(Vei2 delta);				//Delta == delta cSize
	void ApplyCameraChanges(Vec2 cDelta);


	void Init(WorldSettings& s);																									// 'ontoType' defines the required Celltype 
	void Generate(WorldSettings& s);																								// 'surrBy' defines the required nearbyCell
	void GenerateCircle(Vei2 pos, int radius, int type, int ontoType = -1, int surrBy = -1); 
	void GenerateLine(Vec2 p0, Vec2 p1, int type, int ontoType = -1, int thickness = 1, int surrBy = -1);
	void GenerateExplosion(Vei2 pos, int maxLineLength, int type, int ontoType = -1, int nRolls = 100, int surrBy = -1);
	bool GenerateCell(Vei2 pos, int type, int ontoType = -1, int surrBy = -1);
	bool FIDF(int first, int second)const;//First is drawn first
	void CutHills(int replaceTo);
	bool ObstaclePosAllowed(Vei2 tilePos, int type);
	
	bool GenerateObstacle(Vei2 tilePos, int type, int ontoType = -1, int surrBy = -1);
	void GenerateObstacleExplosion(Vei2 pos, int maxLineLength, int type, int ontoType = -1, int nRolls = 25, int surrBy = -1);
	void GenerateObstacleLine(Vec2 tile0, Vec2 tile1, int type, int ontoType = -1, int thickness = 1, int surrBy = -1);
	void GenerateObstaclesInCell(Vei2 cellPos, int type, int number, int ontoType = -1, int surrBy = -1);

public:
																							//Konstruktor + Operatoren
	World(WorldSettings wSettings, std::shared_ptr<ResourceCollection> resC, Vec2& camera);
	//change game values
	int World::GetxStart() {
		Vei2 mos = Graphics::GetMidOfScreen();
		return -(mos.x / GetcSize().x) * 2;
	};
	//Handles
	void HandleMouseEvents(Mouse::Event& e, GrabHandle& gH);
	void HandleKeyboardEvents(Keyboard::Event& e);
	void UpdateGameLogic(float dt);

	//Grafiken + Einbindung dieser in groundedMap
	void Draw(Graphics& gfx)const;
	void DrawObstacle(Vei2 tilePos, int type, Graphics& gfx, Color color = Colors::Magenta, int frame = -1)const;

	std::vector<SubAnimation> GetConnectionsOfTypes(Vei2 pos, int* types);
	bool NeedsConnections(Vei2 curXY)const;
	void SetBuildMode(int obstacle);
	//
	Vei2 GetwSize()const { return s.wSize; }
	Vei2 GetcSize()const { return s.chunkSize / s.chunkHasNCells; }
	Vec2 GetTileSize()const { return (Vec2)GetcSize()/Settings::CellSplitUpIn; }
	Vei2 GetfCell()const { return fCell; }
	Vei2 GetfTile()const { return fTile; }
	int GetChunksDrawnToLeft()const { return -(Graphics::GetMidOfScreen().x / s.chunkSize.x) * 2 - 1; }
	int GetfCellType()const { return chunks(fccPos.x).GetCellTypeAt(fccPos.y); }
	Vei2 GetmChunk()const { return mChunk; }
	Team& GetPlayer() { return player; }
};

