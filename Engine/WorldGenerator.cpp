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
void WorldGenerator::GenerateExplosionRect(RectI sprayPos, int maxLineLength, int type, int nRolls)
{
	for (int y = 0; y < sprayPos.GetHeight(); y++)
	{
		for (int x = 0; x < sprayPos.GetWidth(); x++)
		{
			GenerateExplosion(Vei2(sprayPos.left + x, sprayPos.top + y), maxLineLength, type,-1,nRolls);
		}
	}
}
std::vector<Vei2> WorldGenerator::GenerateRandomExplosions(RectI bounds, int maxLineLength, int type, float density, int ontoType, int nRolls, int surrBy)
{
	std::vector<Vei2> spawnPoints;
	int nSpawns = density * bounds.GetWidth() * bounds.GetHeight();
	int trys = 0;
	for (int i = 0; i < nSpawns;)
	{
		Vei2 spawnPoint = Vei2(bounds.left + rng.Calc(bounds.GetWidth()-1), bounds.top + rng.Calc(bounds.GetHeight()));
		CtPos spawnPointCt = Cell2ChunkPos(spawnPoint);
		if (ontoType == -1 || chunks(spawnPointCt.x).GetCellTypeAt(spawnPointCt.y / Settings::CellSplitUpIn) == ontoType)
		{
			spawnPoints.push_back(spawnPoint);
			GenerateExplosion(spawnPoint, maxLineLength, type, ontoType, nRolls, surrBy);
			i++;
		}
		if (trys >= 10000)
		{
			break;
		}
		trys++;
	}
	return spawnPoints;
}
void WorldGenerator::GenerateExplosions(std::vector<Vei2> spawnPoints, int maxLineLength, int type, int ontoType, int nRolls, int surrBy)
{
	for (int i = 0; i < spawnPoints.size();i++)
	{
		GenerateExplosion(spawnPoints[i], maxLineLength, type, ontoType, nRolls, surrBy);
	}
}
void WorldGenerator::GenerateObstaclesOnRandomPosition(RectI bounds, int type, float density, std::vector<int> ontoTypes, Team* team)
{
	RectI tileRect = bounds * Settings::CellSplitUpIn;
	int nSpawns = density * tileRect.GetWidth() * tileRect.GetHeight();
	for (int i = 0; i < nSpawns; i++)
	{
		Vei2 spawnAt = Chunk::PutTileInWorld(Vei2(tileRect.left + rng.Calc(tileRect.GetWidth()), tileRect.top + rng.Calc(tileRect.GetHeight())),settings.GetWorldSizeInTiles());
		
		CtPos ccPos = Chunk::Flat2ChunkPosCtPos(spawnAt, settings.GetWorldSizeInTiles());// (PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn), s.GetWorldSizeInCells()));
		if (std::any_of(ontoTypes.begin(), ontoTypes.end(), [&](const int& val) {return chunks(ccPos.x).GetCellTypeAt(ccPos.y / Settings::CellSplitUpIn) == val; }))
		{
			chunks(Vei2(0, 0)).PlaceObstacle(spawnAt, type, team);
		}
	}
}
void WorldGenerator::GenerateObstaclegroupOnRandomPosition(RectI bounds, int type, float density, std::vector<int> ontoTypes, Team* team)
{
	RectI tileRect = bounds * Settings::CellSplitUpIn;
	int nSpawns = density * tileRect.GetWidth() * tileRect.GetHeight();
	for (int i = 0; i < nSpawns; i++)
	{
		Vei2 spawnAt = Chunk::PutTileInWorld(Vei2(tileRect.left + rng.Calc(tileRect.GetWidth()), tileRect.top + rng.Calc(tileRect.GetHeight())), settings.GetWorldSizeInTiles());

		CtPos ccPos = Chunk::Flat2ChunkPosCtPos(spawnAt, settings.GetWorldSizeInTiles());// (PutCellInWorldX(Vei2(spawnAt.x / Settings::CellSplitUpIn, spawnAt.y / Settings::CellSplitUpIn), s.GetWorldSizeInCells()));
		if (std::any_of(ontoTypes.begin(), ontoTypes.end(), [&](const int& val) {return chunks(ccPos.x).GetCellTypeAt(ccPos.y / Settings::CellSplitUpIn) == val; }))
		{
			SpawnUnitGroup(spawnAt, type, team, rng.Calc(10) + 5);
		}
	}
}
CtPos WorldGenerator::SpawnPlayer(Vei2 tilePos)
{
	CtPos spawnpos = SpawnTeam(&teams->at("player"), tilePos);
	mChunk = spawnpos.x;
	c.x = ((float)spawnpos.y.x / Settings::chunkHasNTiles) * settings.GetChunkSize().x;
	c.y = ((float)spawnpos.y.y / Settings::chunkHasNTiles) * settings.GetChunkSize().y;
}
void WorldGenerator::InitEnemies()
{
	for (int i = 2; i < 16; i++)
	{
		std::string team = "enemie" + std::to_string(i - 2);
		teams->insert({ team,Team(i) });
		//SpawnTeam(&teams->at(team), enemieSpawnPoint);
	}
}
void WorldGenerator::SpawnEnemie(int spawnCode, Team* team, RectI bounds)
{
	RectI tileRect = bounds * Settings::CellSplitUpIn;
	Vei2 spawnAt = Chunk::PutTileInWorld(Vei2(tileRect.left + rng.Calc(tileRect.GetWidth()), tileRect.top + rng.Calc(tileRect.GetHeight())), settings.GetWorldSizeInTiles());
	SpawnTeam(team, spawnAt);
}
void WorldGenerator::Generate(const Settings::WorldSettings& s)
{
	InitChunks();
	InitEnemies();
	//cells = Matrix<Cell>(s.wSize.x, s.wSize.y, Cell(s.GetDefaultType()));		
	//obstacleMap = Matrix<int>(s.wSize.x * Settings::CellSplitUpIn, s.wSize.y * Settings::CellSplitUpIn, -1);

	switch (s.GetBlueprint())
	{
	case 0:

		//rects in cellSize
		int poleSize = (s.GetWorldSizeInCells().y / 5);
		Vei2 wSizeInCells = s.GetWorldSizeInCells();

		RectI northpole = RectI(Vei2(0, wSizeInCells.y / 5 * 4), wSizeInCells.x, poleSize);
		RectI mediumWidthsTop = RectI(Vei2(0, wSizeInCells.y / 5 * 3), wSizeInCells.x, wSizeInCells.y / 5);
		RectI tropicsZone = RectI(Vei2(0, wSizeInCells.y/5*2), wSizeInCells.x, wSizeInCells.y / 5);
		RectI mediumWidthsBottom = RectI(Vei2(0, wSizeInCells.y / 5 * 1), wSizeInCells.x, wSizeInCells.y / 5);
		RectI southpole = RectI(Vei2(0, 0), wSizeInCells.x, poleSize);
		
		RectI nonPolarZone = RectI(Vei2(0, wSizeInCells.y / 5 * 1), wSizeInCells.x, wSizeInCells.y / 5*3);

		//set fieldtypes
		GenerateExplosionRect(RectI(Vei2(0, 0), wSizeInCells.x, 1), poleSize, 2, 2);//south pole
		GenerateRandomExplosions(RectI(Vei2(0, 0), wSizeInCells.x, 1), 1, 5, 0.5f, -1, 5);//snow
		GenerateExplosionRect(RectI(Vei2(0, wSizeInCells.y - 1), s.GetWorldSizeInCells().x, 1), poleSize, 2, 2);//north pole
		GenerateRandomExplosions(RectI(Vei2(0, wSizeInCells.y - 1), s.GetWorldSizeInCells().x,1), 1, 5, 0.5f, -1, 5);//snow

		GenerateRandomExplosions(mediumWidthsBottom, 3, 1, 0.15f,-1,5);//dirt bottom
		GenerateRandomExplosions(tropicsZone,2, 4, 0.1f, -1, 5);//nutritious tropics
		GenerateRandomExplosions(mediumWidthsTop, 3, 1, 0.15f, -1, 5);//dirt top
		GenerateRandomExplosions(nonPolarZone, 2, 14, 0.025f, -1, 5);//swamps

		GenerateRandomExplosions(tropicsZone, 2, 6, 0.05f, 0, 5);//coral reef
		GenerateRandomExplosions(mediumWidthsTop, 1, 13, 0.09f, 1, 5);//candyland
		GenerateRandomExplosions(nonPolarZone, 5, 3, 0.02f, 1, 5);//desert

		GenerateRandomExplosions(mediumWidthsTop, 2, 10, 0.06f, -1, 5);//rocks
		std::vector<Vei2> savannePos = GenerateRandomExplosions(mediumWidthsTop, 4, 9, 0.06f, -1, 5);//savanne
		GenerateExplosions(savannePos, 2, 11, -1, 5);//canjon
		std::vector<Vei2> lavaPos = GenerateRandomExplosions(mediumWidthsTop, 3, 12, 0.03f, -1, 15);//lava
		GenerateExplosions(lavaPos, 2, 8, -1, 5);//lavahills

		//
		UpdateConMap();
		UpdateGroundedMap();
		
		//set obstacles
		if (Settings::obstaclesOn && Settings::spawnObstacles)
		{
			GenerateObstaclesOnRandomPosition(nonPolarZone, 1, 0.03f, { 1, 4 });//trees
			GenerateObstaclesOnRandomPosition(nonPolarZone, 8, 0.005f, { 1, 4 });//firs
			GenerateObstaclesOnRandomPosition(nonPolarZone, 6, 0.001f, { 1, 4 });//boxes
			GenerateObstaclesOnRandomPosition(nonPolarZone, 5, 0.01f, { 3 });//cactus

			GenerateObstaclesOnRandomPosition(nonPolarZone, 7, 0.01f, { 1,3 });//stones
			GenerateObstaclesOnRandomPosition(nonPolarZone, 9, 0.0025f, { 1,3 });//big stones

			for (int i = 0; i < 7; i++) {
				GenerateObstaclegroupOnRandomPosition(nonPolarZone, 11+i, 0.0001f, { 1,3 }, &teams->at("animals"));//animals
			}
			GenerateObstaclegroupOnRandomPosition(northpole, 18, 0.0006f, { 2,5 }, &teams->at("animals"));//penguins
			GenerateObstaclegroupOnRandomPosition(southpole, 18, 0.0003f, { 2,5 }, &teams->at("animals"));//penguins

			GenerateObstaclesOnRandomPosition(nonPolarZone, 19, 0.00048f, { 1,3 }, &teams->at("animals"));//mices
			GenerateObstaclesOnRandomPosition(nonPolarZone, 20, 0.00048f, { 1,3 }, &teams->at("animals"));//snakes

			SpawnPlayer(Vei2(0, (poleSize + 1) * Settings::CellSplitUpIn));
			//Vei2 playerSpawnFlat = Chunk::chunkPos2Flat(playerSpawn);
			//RectI enemieRect = RectI(Vei2())
			for (int i = 2; i < 16; i++)
			{
				std::string team = "enemie" + std::to_string(i - 2);
				SpawnEnemie(0, &teams->at(team), nonPolarZone);
			}
		}

		UpdateConMap();
		UpdateGroundedMap();
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
				Vei2 putPos = PutCellInWorld(pos + Vei2(x, y),settings.GetWorldSizeInCells());
				if (chunks(Vei2(0, 0)).CellIsInWorld(putPos))
				{
					Vei2 curCellPos = PutCellInWorld(putPos,settings.GetWorldSizeInCells());
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
void WorldGenerator::GenerateLine(Vec2 p0, Vec2 p1, int type, int ontoType, int thickness, int surrBy) 
{
	Vec2 wSizeInCells = (Vec2)settings.GetWorldSizeInCells();
	//if (p0 != p1) {
		float m = 10000.0f;
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
		if (/*p1.x != p0.x && */std::abs(m) <= 1.0f)
		{
			if (p0.x > p1.x)
			{
				std::swap(p0, p1);
			}

			if (std::abs(p0.x - p1.x) >= wSizeInCells.x / 2)
			{
				Vec2 lastL = Vec2(wSizeInCells.x - 1, p1.y + (wSizeInCells.x - p1.x) * m);
				GenerateLine(p1, lastL, type, ontoType, 1, surrBy);
				Vec2 firstR = Vec2(0, p0.y - p0.x * m);
				GenerateLine(firstR, p0, type, ontoType, 1, surrBy);
				return;
			}

			const float b = p0.y - m * p0.x;

			for (int x = (int)p0.x; x <= (int)p1.x; x++)
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

			if (std::abs(p0.x - p1.x) >= wSizeInCells.x / 2)
			{
				Vec2 lastL = Vec2(wSizeInCells.x - 1, p1.y + (wSizeInCells.x - p1.x) * m);
				GenerateLine(p1, lastL, type, ontoType, 1, surrBy);
				Vec2 firstR = Vec2(0, p0.y - p0.x * m);
				GenerateLine(firstR, p0, type, ontoType, 1, surrBy);
				return;
			}

			const float w = (p1.x - p0.x) / (p1.y - p0.y);
			const float p = p0.x - w * p0.y;

			for (int y = (int)p0.y; y <= (int)p1.y; y++)
			{
				const float x = w * (float)y + p;

				const int xi = (int)x;

				GenerateCell(Vei2(xi, y), type, ontoType, surrBy);
			}
		}
	//}
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
void WorldGenerator::GenerateExplosion(Vei2 pos, int maxLineLength, int type, int ontoType, int nRolls, int surrBy)
{
	for (int i = 0; i < nRolls;)
	{
		Vec2 p1 = GigaMath::GetRandomPointOnUnitCircle<float>();
		Vei2 scaled = PutCellInWorld(pos + Vei2(p1 * (float)((float)maxLineLength * 1 / 2 + (float)(rng.Calc(maxLineLength * 50)) / 100)),settings.GetWorldSizeInCells());
		if (chunks(Vei2(0,0)).CellIsInWorld(scaled))
		{
			GenerateLine(Vec2(pos), Vec2(scaled), type, ontoType, 1, surrBy);
			i++;
		}
	}
}
bool WorldGenerator::GenerateCell(Vei2 pos, int type, int ontoType, int surrBy)
{
	Vei2 curCellPos = PutCellInWorld(pos, settings.GetWorldSizeInCells());
	if (chunks(Vei2(0, 0)).CellIsInWorld(curCellPos))
	{
		auto ccPos = Cell2ChunkPos(curCellPos);
		Matrix<int> aMat;
		if (surrBy != -1)
		{
			Matrix<int> aMat = chunks(ccPos.x).GetAroundmatrix(ccPos.y);
			assert(aMat.HasValue(-1) == false);//surrBy just available after calling UpdateConMap and UpdateGroundedMap!!!
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
					auto ccPos = Cell2ChunkPos(PutCellInWorld(x * Settings::chunkHasNCells + xInner - 1, y * Settings::chunkHasNCells + yInner - 1, settings.GetWorldSizeInCells()));

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
			auto ccPos = WorldGenerator::Cell2ChunkPos(PutCellInWorld(pos + Vei2(x - 1, y - 1),settings.GetWorldSizeInCells()));

			if (chunks(ccPos.x).GetCellTypeAt(ccPos.y) == type)
			{
				return true;
			}
		}
	}
	return false;
}

