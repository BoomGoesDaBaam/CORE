#include "WorldGenerator.h"
WorldGenerator::WorldGenerator(Matrix<Chunk>& chunks, sharedResC resC,const Settings::WorldSettings& settings, std::map<std::string, Team>* teams)
	:
	chunks(chunks),
	resC(std::move(resC)),
	settings(settings),
	teams(teams)
{

}
void WorldGenerator::InitChunks()
{
	chunks = Matrix<Chunk>(settings.GetChunkAmount().x, settings.GetChunkAmount().y, Chunk(Settings::chunkHasNCells, Cell(settings.GetDefaultType()), &chunks, resC)); //Welt erstellen & default value setzen
	for (int y = 0; y < chunks.GetColums(); y++)
	{
		for (int x = 0; x < chunks.GetRows(); x++)
		{
			chunks[x][y].SetChunkPos(Vei2(x, y));
		}
	}
}
void WorldGenerator::GenerateSprayRect(RectI sprayPos, int sprayLength, int type)
{
	for (int y = 0; y < sprayPos.GetHeight(); y++)
	{
		for (int x = 0; x < sprayPos.GetWidth(); x++)
		{
			int ll = 1 + (int)(rng.GetNormalDist() * (sprayLength - 1));
			GenerateExplosion(Vei2(sprayPos.left + x, sprayPos.top + y), ll, type);
		}
	}
}
void WorldGenerator::Generate(const Settings::WorldSettings& s)
{
	InitChunks();
	//cells = Matrix<Cell>(s.wSize.x, s.wSize.y, Cell(s.GetDefaultType()));		
	//obstacleMap = Matrix<int>(s.wSize.x * Settings::CellSplitUpIn, s.wSize.y * Settings::CellSplitUpIn, -1);

	switch (s.GetBlueprint())
	{
	case 0:
		int arcticSize = (s.GetWorldSizeInCells().y / 40);
		int subArcticSize = (int)(s.GetWorldSizeInCells().y / 7.5f);

		int poleSize = (s.GetWorldSizeInCells().y / 3);
		Vei2 wSizeInCells = s.GetWorldSizeInCells();


		GenerateSprayRect(RectI(Vei2(0, 0), s.GetWorldSizeInCells().x, 1), poleSize, 2);//south pole
		GenerateSprayRect(RectI(Vei2(0, wSizeInCells.y - 1), s.GetWorldSizeInCells().x, 1), poleSize, 2);//north pole

		/*
		for (int i = 0; i < s.GetWorldSizeInCells().y; i++)					//north & south ice
		{
			GenerateExplosion(Vei2(i, 0), (int)(rng.GetNormalDist() * arcticSize), 2);
			GenerateExplosion(Vei2(i, s.GetWorldSizeInCells().y - 1), (int)(rng.GetNormalDist() * arcticSize), 2);
		}
		*/
		/*
		for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 50; i++)	//plants
		{
			GenerateExplosion(Vei2(rng.Calc(s.GetWorldSizeInCells().y), 10 + rng.Calc(s.GetWorldSizeInCells().y - 20)), (int)(rng.GetNormalDist() * 3) + 5, 1);
		}
		for (int i = 0; i < s.GetWorldSizeInCells().x * 5; i++)	//snow
		{
			//GenerateExplosion(Vei2(rng.Calc(s.GetWorldSizeInCells().x), (int)(rng.GetNormalDist() * subArcticSize)), (int)(rng.GetNormalDist() * 3), 5, -1);
			//GenerateExplosion(Vei2(rng.Calc(s.GetWorldSizeInCells().x), s.GetWorldSizeInCells().y - (int)(rng.GetNormalDist() * subArcticSize)), (int)(rng.GetNormalDist() * 3), 5, -1);
		}
		for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 200; i++)	//coral reef
		{
			GenerateExplosion(Vei2(rng.Calc(s.GetWorldSizeInCells().y), 10 + rng.Calc(s.GetWorldSizeInCells().y - 20)), (int)(rng.GetNormalDist() * 3) + 3, 6, 0);
		}
		for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 1600; i++)	//candyland
		{
			GenerateExplosion(Vei2(rng.Calc(s.GetWorldSizeInCells().y), 10 + rng.Calc(s.GetWorldSizeInCells().y - 20)), 3, 13, 1);
		}
		for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 1600; i++)	//desert
		{
			Vei2 pos = Vei2(rng.Calc(s.GetWorldSizeInCells().y), 10 + rng.Calc(s.GetWorldSizeInCells().y - 20));

			GenerateCircle(pos, 7, 3, 1);
			GenerateExplosion(pos, (int)(rng.GetNormalDist() * 6) + 10, 3, 1);
		}
		for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 3200; i++)	//rocks
		{
			GenerateExplosion(Vei2(rng.Calc(s.GetWorldSizeInCells().y), 10 + rng.Calc(s.GetWorldSizeInCells().y - 20)), (int)(rng.GetNormalDist() * 6) + 5, 10, -1);
		}
		for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 400; i++)	//canjon + savanne
		{
			Vei2 pos = Vei2(rng.Calc(s.GetWorldSizeInCells().x), 10 + rng.Calc(s.GetWorldSizeInCells().y - 20));
			GenerateExplosion(pos, 6 + (int)(rng.GetNormalDist() * 6), 9, 1);
			GenerateExplosion(pos, 4, 11, 9);
		}
		for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 1600; i++)	//magma
		{
			Vei2 pos = Vei2(rng.Calc(s.GetWorldSizeInCells().x), 10 + rng.Calc(s.GetWorldSizeInCells().y - 20));
			GenerateExplosion(pos, 6, 12, -1);
			GenerateExplosion(pos, 4, 8, 12);
		}
		UpdateConMap();
		UpdateGroundedMap();

		if (Settings::spawnObstacles)
		{
			for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 10; i++)	//nutritious plants
			{
				GenerateExplosion(Vei2(rng.Calc(s.GetWorldSizeInCells().x), 10 + rng.Calc(s.GetWorldSizeInCells().y - 20)), (int)(rng.GetNormalDist() * 3), 4, 1, 20, 0);
			}
			for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 20; i++)	//swamp
			{
				GenerateExplosion(Vei2(rng.Calc(s.GetWorldSizeInCells().x), 10 + rng.Calc(s.GetWorldSizeInCells().y - 20)), (int)(rng.GetNormalDist() * 6), 14, 0, 20);
			}


			for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y); i++)	//Trees
			{
				Vei2 spawnAt = Vei2(rng.Calc((s.GetWorldSizeInCells().x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.GetWorldSizeInCells().y - 20) * Settings::CellSplitUpIn));
				auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn),s.GetWorldSizeInCells()));
				if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
				{
					int rT = rng.Calc(3);	//random tree
					switch (rT)
					{
					case 0:
						chunks(Vei2(0, 0)).PlaceObstacle(spawnAt, 1);
						break;
					case 1:
						chunks(Vei2(0, 0)).PlaceObstacle(spawnAt, 4);
						break;
					case 2:
						chunks(Vei2(0, 0)).PlaceObstacle(spawnAt, 8);
						break;
					}
				}
			}
			for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) * 2; i++)	//cactus
			{
				Vei2 spawnAt = Vei2(rng.Calc((s.GetWorldSizeInCells().x - 1) * Settings::CellSplitUpIn), 5 * Settings::CellSplitUpIn + rng.Calc((s.GetWorldSizeInCells().y - 10) * Settings::CellSplitUpIn));
				auto ccPos = Cell2ChunkPos(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn));

				if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 3)
				{
					chunks(Vei2(0, 0)).PlaceObstacle(spawnAt, 5);
				}
			}
			for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 20; i++)	//boxes
			{
				Vei2 spawnAt = Vei2(rng.Calc((s.GetWorldSizeInCells().x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.GetWorldSizeInCells().y - 20) * Settings::CellSplitUpIn));
				auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn), s.GetWorldSizeInCells()));

				if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
				{
					chunks(Vei2(0, 0)).PlaceObstacle(spawnAt, 6, nullptr, 3);
				}
			}
			for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y); i++)	//stones
			{
				Vei2 spawnAt = Vei2(rng.Calc((s.GetWorldSizeInCells().x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.GetWorldSizeInCells().y - 20) * Settings::CellSplitUpIn));
				auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn), s.GetWorldSizeInCells()));

				if (Settings::anyGroundedTypes(chunks(ccPos.x).GetCellTypeAt(ccPos.y)))
				{
					int rS = rng.Calc(2);
					switch (rS)
					{
					case 0:
						chunks(Vei2(0, 0)).PlaceObstacle(spawnAt, 7);
						break;
					case 1:
						chunks(Vei2(0, 0)).PlaceObstacle(spawnAt, 9);
						break;
					}
				}
			}
			for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 100; i++)	//other Trees
			{
				Vei2 spawnAt = Vei2(rng.Calc((s.GetWorldSizeInCells().x - 1) * Settings::CellSplitUpIn), 10 * Settings::CellSplitUpIn + rng.Calc((s.GetWorldSizeInCells().y - 20) * Settings::CellSplitUpIn));
				auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn), s.GetWorldSizeInCells()));

				if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
				{
					/*
					GenerateObstacleExplosion(spawnAt, 10, 100, 1, nullptr, 1, 15);
					GenerateObstacleExplosion(spawnAt + Vei2(0, 1), 10, 100, 1, nullptr, 1, 15);
					GenerateObstacleExplosion(spawnAt + Vei2(1, 0), 10, 100, 1, nullptr, 1, 15);
					GenerateObstacleExplosion(spawnAt + Vei2(1, 1), 10, 100, 1, nullptr, 1, 15);

					GenerateObstacleExplosion(spawnAt, 10, 50, 4, nullptr, 1, 15);
					GenerateObstacleExplosion(spawnAt + Vei2(0, 1), 10, 100, 4, nullptr, 1, 15);
					GenerateObstacleExplosion(spawnAt + Vei2(1, 0), 10, 100, 4, nullptr, 1, 15);
					GenerateObstacleExplosion(spawnAt + Vei2(1, 1), 10, 100, 4, nullptr, 1, 15);
					/*######
				}
			}
			for (int n = 0; n < 7; n++)
			{
				for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 100; i++)	//normal animals
				{
					Vei2 spawnAt = Vei2(rng.Calc((s.GetWorldSizeInCells().x - 1) * Settings::CellSplitUpIn), subArcticSize * Settings::CellSplitUpIn + rng.Calc((s.GetWorldSizeInCells().y - subArcticSize * 2) * Settings::CellSplitUpIn));
					auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn), s.GetWorldSizeInCells()));

					if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
					{
						SpawnUnitGroup(spawnAt, 11 + n, &teams->at("animals"), rng.Calc(10) + 5);
					}
				}
			}
			for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 20; i++)	//penguins
			{
				Vei2 spawnAtArctis = Vei2(rng.Calc((s.GetWorldSizeInCells().x - 1) * Settings::CellSplitUpIn), rng.Calc(subArcticSize * Settings::CellSplitUpIn));
				Vei2 spawnAtTopAntarktis = Vei2(rng.Calc((s.GetWorldSizeInCells().x - 1) * Settings::CellSplitUpIn), Settings::chunkHasNTiles * s.GetChunkAmount().y - rng.Calc(subArcticSize * Settings::CellSplitUpIn));

				auto ccPos1 = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAtArctis.x / Settings::CellSplitUpIn, spawnAtArctis.y / Settings::CellSplitUpIn), s.GetWorldSizeInCells()));
				auto ccPos2 = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAtTopAntarktis.x / Settings::CellSplitUpIn, spawnAtTopAntarktis.y / Settings::CellSplitUpIn), s.GetWorldSizeInCells()));

				if (chunks(ccPos1.x).GetCellTypeAt(ccPos1.y) != 0)
				{
					SpawnUnitGroup(spawnAtArctis, 18, &teams->at("animals"), rng.Calc(10) + 5);
				}
				if (chunks(ccPos2.x).GetCellTypeAt(ccPos2.y) != 0)
				{
					SpawnUnitGroup(spawnAtTopAntarktis, 18, &teams->at("animals"), rng.Calc(10) + 5);
				}
			}
			for (int n = 0; n < 2; n++)
			{
				for (int i = 0; i < (s.GetWorldSizeInCells().x * s.GetWorldSizeInCells().y) / 20; i++)	//mouses and snakes
				{
					Vei2 spawnAt = Vei2(rng.Calc((s.GetWorldSizeInCells().x - 1) * Settings::CellSplitUpIn), subArcticSize * Settings::CellSplitUpIn + rng.Calc((s.GetWorldSizeInCells().y - subArcticSize * 2) * Settings::CellSplitUpIn));
					auto ccPos = Cell2ChunkPos(PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn), s.GetWorldSizeInCells()));

					if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == 1)
					{
						SpawnUnitGroup(spawnAt, 19 + n, &teams->at("animals"), 1);
					}
				}
			}
		}
		break;
	}

	UpdateConMap();
	//UpdateGroundedMap();
	//CtPos spawnpos;
	if (Settings::obstaclesOn && Settings::spawnObstacles)
	{
		spawnpos = SpawnTeam(&teams->at("player"), Vei2(0, (s.GetChunkAmount().y * Settings::chunkHasNTiles) / 2));
		mChunk = spawnpos.x;
		c.x = ((float)spawnpos.y.x / Settings::chunkHasNTiles) * s.GetChunkSize().x;
		c.y = ((float)spawnpos.y.y / Settings::chunkHasNTiles) * s.GetChunkSize().y;

		for (int i = 2; i < 16; i++)
		{
			Vei2 enemieSpawnPoint = Chunk::chunkPos2Flat(spawnpos);
			Vei2 vers = (Vei2)(GigaMath::GetRandomPointOnUnitCircle<float>() * (rng.Calc(30) + 30));
			//float dist = std::sqrt(std::pow(vers.x, 2)+std::pow(vers.y, 2));
			enemieSpawnPoint += vers;//(Vei2)(GigaMath::GetRandomPointOnUnitCircle<float>() * (rng.Calc(50) + 30));
			enemieSpawnPoint = Chunk::PutTileInWorld(enemieSpawnPoint.x, enemieSpawnPoint.y, s.GetWorldSizeInTiles());
			std::string team = "enemie" + std::to_string(i - 2);
			teams->insert({ team,Team(i) });
			SpawnTeam(&teams->at(team), enemieSpawnPoint);
		}
	}

	/*
	teams["Fuer die Natur"] = &animals;
	teams["In dem Sinne"] = &animals;
	teams["Nichts wie Zeg"] = &animals;
	teams["Was geht!"] = &animals;
	*/
	//CutHills(1);		//NICHT ZUENDE!!!!
	}
}

void WorldGenerator::GenerateCircle(Vei2 pos, int radius, int type, int ontoType, int surrBy)
{
	for (int y = -radius; y < radius; y++)
	{
		for (int x = -radius; x < radius; x++)
		{
			if (sqrt(y * y + x * x) <= radius)
			{
				Vei2 putPos = PutCellInWorldX(pos + Vei2(x, y),settings.GetWorldSizeInCells());
				if (chunks(Vei2(0, 0)).CellIsInWorld(putPos))
				{
					Vei2 curCellPos = PutCellInWorldX(putPos,settings.GetWorldSizeInCells());
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
void WorldGenerator::GenerateLine(Vec2 p0, Vec2 p1, int type, int ontoType, int thickness, int surrBy) //bnot
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
					GenerateLine(Vec2(p1.x, p1.y + i), Vec2(p0.x, p0.y + i), type, ontoType, 1, surrBy);
				}
				else
				{
					GenerateLine(Vec2(p1.x + i, p1.y), Vec2(p0.x + i, p0.y), type, ontoType, 1, surrBy);
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
void WorldGenerator::GenerateObstacleLine(Vec2 tile0, Vec2 tile1, int type, Team* team, int ontoType, int thickness, int surrBy) //bnot
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
					GenerateObstacleLine(Vec2(tile1.x, tile1.y + i), Vec2(tile0.x, tile0.y + i), type, team, ontoType, 1, surrBy);
				}
				else
				{
					GenerateObstacleLine(Vec2(tile1.x + i, tile1.y), Vec2(tile0.x + i, tile0.y), type, team, ontoType, 1, surrBy);
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

				Vei2 tilePos = Chunk::PutTileInWorld(Vei2(x, yi), settings.GetWorldSizeInTiles());
				auto tccPos = Chunk::Flat2ChunkPos(tilePos, settings.GetWorldSizeInTiles());
				auto aMat = chunks(tccPos.x).GetAroundmatrix(tccPos.y);

				/*
				int curObstacleIndex = ObstacleMapAt(tilePos);
				if (curObstacleIndex != -1 && obstacles[curObstacleIndex]->type == ontoType)
				{
					DestroyObstacleAt(tilePos);
				}
				*/
				if (chunks(Vei2(0,0)).ObstacleMapAt(tilePos) == -1 && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
				{
					chunks(Vei2(0, 0)).PlaceObstacle(tilePos, type);
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

				Vei2 tilePos = Chunk::PutTileInWorld(Vei2(xi, y), settings.GetWorldSizeInTiles());
				auto tccPos = Chunk::Flat2ChunkPos(tilePos, settings.GetWorldSizeInTiles());
				auto aMat = chunks(tccPos.x).GetAroundmatrix(tccPos.y);
				/*
				int curObstacleIndex = ObstacleMapAt(tilePos);
				if (curObstacleIndex != -1 && obstacles[curObstacleIndex]->type == ontoType)
				{
					DestroyObstacleAt(tilePos);
				}
				*/
				if (chunks(Vei2(0,0)).ObstacleMapAt(tilePos) == -1 && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
				{
					chunks(Vei2(0, 0)).PlaceObstacle(tilePos, type, team);
				}
			}
		}
	}
}
std::vector<Obstacle*> WorldGenerator::GenerateObstacleExplosion(Vei2 pos, int nMax, int maxLineLength, int type, Team* team, int ontoType, int nRolls, int surrBy)
{
	std::vector<Obstacle*> obsts;
	for (int i = 0; i < nRolls; i++)
	{
		float rad = (float)rng.Calc(360) * 0.0174533f;
		Vec2 p1 = (Vec2)GigaMath::RotPointToOrigin<double>(1.0f, 0.0f, rad);
		Vei2 scaled = pos + (Vei2)((p1 * (float)maxLineLength) * GigaMath::GetRandomNormDistribution());

		if (chunks(Vei2(0, 0)).PlaceObstacle(scaled, type, team, ontoType, surrBy))
		{
			obsts.push_back(chunks(Vei2(0, 0)).GetObstacleAt(scaled));
			nMax--;
		}
		if (nMax <= 0)
		{
			return obsts;
		}
	}
	return obsts;
}
void WorldGenerator::GenerateExplosion(Vei2 pos, int maxLineLength, int type, int ontoType, int nRolls, int surrBy)//not
{
	for (int i = 0; i < nRolls; i++)
	{
		float rad = (float)rng.Calc(360) * 0.0174533f;
		Vec2 p1 = (Vec2)GigaMath::RotPointToOrigin<float>(1.0f, 0.0f, rad);
		Vei2 scaled = pos + Vei2(p1 * (float)(maxLineLength * 1 / 2 + rng.Calc(maxLineLength * 1 / 2)));
		if (chunks(Vei2(0,0)).CellIsInWorld(scaled))
		{
			GenerateLine(Vec2(pos), Vec2(scaled), type, ontoType, 1, surrBy);
		}
	}
}
bool WorldGenerator::GenerateCell(Vei2 pos, int type, int ontoType, int surrBy)
{
	Vei2 curCellPos = PutCellInWorldX(pos, settings.GetWorldSizeInCells());
	if (chunks(Vei2(0, 0)).CellIsInWorld(curCellPos))
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
void WorldGenerator::GenerateObstaclesInCell(Vei2 cellPos, int type, int number, Team* team, int ontoType, int surrBy)
{
	int counter = 0;
	for (int i = 0; i < number && counter < number * 4;)
	{
		Vei2 obstPos = cellPos * Settings::CellSplitUpIn + Vei2(rng.Calc(Settings::CellSplitUpIn - 1), rng.Calc(Settings::CellSplitUpIn - 1));
		if (chunks(Vei2(0,0)).ObstaclePosAllowed(obstPos, type) && chunks(Vei2(0, 0)).PlaceObstacle(obstPos, type, team, ontoType, surrBy))
		{
			i++;
		}
		counter++;
	}
}
void WorldGenerator::UpdateConMap()
{
	for (int type = 0; type < Settings::nDiffFieldTypes; type++)
	{
		for (int y = 0; y < settings.GetWorldSizeInCells().y; y++)
		{
			for (int x = 0; x < settings.GetWorldSizeInCells().x; x++)
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
	for (int y = 0; y < settings.GetChunkAmount().y; y++)
	{
		for (int x = 0; x < settings.GetChunkAmount().x; x++)
		{
			Matrix<int> mat = Matrix<int>(Settings::chunkHasNCells + 2, Settings::chunkHasNCells + 2, -1);
			for (int yInner = 0; yInner < Settings::chunkHasNCells + 2; yInner++)
			{
				for (int xInner = 0; xInner < Settings::chunkHasNCells + 2; xInner++)
				{
					auto ccPos = Cell2ChunkPos(PutCellInWorldX(x * Settings::chunkHasNCells + xInner - 1, y * Settings::chunkHasNCells + yInner - 1, settings.GetWorldSizeInCells()));

					mat[xInner][yInner] = chunks(ccPos.x).GetCellTypeAt(ccPos.y);
				}
			}
			chunks[x][y].UpdateAroundMatrix(mat);
		}
	}
}
void WorldGenerator::UpdateGroundedMap(Vei2 pos, Vei2 size)
{
	using namespace Settings;
	if (obstaclesOn)
	{
		if (size == Vei2(-1, -1) || size == settings.GetWorldSizeInCells())
		{
			size = settings.GetChunkAmount();
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
std::vector<Obstacle*> WorldGenerator::SpawnUnits(int n, int type, Team* team, Vei2 tilePos)
{
	std::vector<Obstacle*> generatedObstacles;
	for (int i = 0; i < n;)
	{
		float rad = (float)rng.Calc(360) * 0.0174533f;
		Vec2 p1 = (Vec2)GigaMath::RotPointToOrigin<float>(1.0f, 0.0f, rad);
		Vei2 spawnPos = tilePos + Vei2(p1 * (float)rng.Calc(10));
		if (chunks(Vei2(0, 0)).PlaceObstacle(spawnPos, type, team))
		{
			assert(chunks(Vei2(0, 0)).GetObstacleOutOfBounds(spawnPos) != nullptr);
			generatedObstacles.push_back(chunks(Vei2(0, 0)).GetObstacleAt(spawnPos));
			i++;
		}
	}
	return generatedObstacles;
}

CtPos WorldGenerator::SpawnTeam(Team* team, Vei2 circaSpawnPoint)
{
	CtPos spawnPointCtPos = Chunk::Flat2ChunkPosCtPos(circaSpawnPoint, settings.GetWorldSizeInTiles());
	Vei2 spawnpoint = Chunk::chunkPos2Flat(chunks(spawnPointCtPos.x).FindNearestPositionThatFits(spawnPointCtPos.y, 10));
	std::vector<Obstacle*> genObsts = SpawnUnits(5, 10, team, spawnpoint);
	CtPos actuallSpawnPoint = genObsts[0]->GetCtPos();
	std::vector<Obstacle*> box = GenerateObstacleExplosion(spawnpoint, 1, 5, 6, team, -1, 25);
	for (int i = 0; i < genObsts.size(); i++)
	{
		genObsts[i]->inv->SetItem(std::make_unique<Slot>(9), 4);
		genObsts[i]->inv->SetItem(std::make_unique<Slot>(10), 5);
		genObsts[i]->inv->SetItem(std::make_unique<Slot>(11), 6);
	}
	return actuallSpawnPoint;
}
void WorldGenerator::SpawnUnitGroup(Vei2 circaTilePos, int type, Team* team, int n)
{
	while (chunks(Vei2(0,0)).GroundedMapAt(circaTilePos) != 1)
	{
		circaTilePos.x++;
	}
	SpawnUnits(n, type, team, circaTilePos);
}
bool WorldGenerator::IsSurroundedBy(Vei2 pos, int type)
{
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			auto ccPos = WorldGenerator::Cell2ChunkPos(PutCellInWorldX(pos + Vei2(x - 1, y - 1),settings.GetWorldSizeInCells()));

			if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == type)
			{
				return true;
			}
		}
	}
	return false;
}

