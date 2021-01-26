#include "WorldDrawer.h"
void WorldDrawer::CalculateRects(World* w)
{
	/*
	this->w = w;

	Vei2 screenSize = Graphics::GetScreenVec2_<int>();
	Vei2 mos = Graphics::GetMidOfScreen();

	int xStart = 0;
	int xStop = 0;
	int yStart = 0;
	int yStop = 0;

	int indexX = 0;
	int indexY = 0;

	drawNCells = Vei2(xStop - xStart + 1, yStop - yStart + 1);
	chunkRects = Matrix<RectF>(drawNCells.x, drawNCells.y,RectF(Vec2(0,0),0,0));

	for (int y = yStart; y <= yStop; y++)
	{
		for (int x = xStart; x <= xStop; x++)
		{
			Vei2 curChunk = Chunk::PutChunkInWorld(w->mChunk + Vei2(x, y), w->s.worldHasNChunks);
			Vei2 curChunkPos = Vei2(x * w->s.chunkSize.x, -y * w->s.chunkSize.y) + Graphics::GetMidOfScreen() - Vei2(w->c.x, -w->c.y);
			RectF curChunkRect = RectF((Vec2)curChunkPos, w->s.chunkSize.x, w->s.chunkSize.y);
			chunkRects[indexX][indexY] = curChunkRect;
			chunkPos[indexX][indexY] = curChunk;
			indexX++;
		}
		indexY++;
	}
	*/
}
void WorldDrawer::Draw()
{
	
	//gfx.DrawSurface(RectI(Vei2(50, 50), 100, 100), resC->tC.fields[0].GetFrames()[0], SpriteEffect::Nothing(), 0);

	Vei2 mos = Graphics::GetMidOfScreen();
	auto renderRect = w->GetRenderRect();
	/*
	int xStart = renderRect.left;
	int xStop = renderRect.right;
	int yStart = renderRect.top;
	int yStop = renderRect.bottom;
	*/
	int xStart = 0;
	int xStop = 0;
	int yStart = 0;
	int yStop = 0;
	
#ifdef _DEBUG 
	xStart = -1;
	xStop = 1;
	yStart = -1;
	yStop = 1;
#endif
	int indexX = 0;
	int indexY = 0;

	for (int layer = 0; layer < 4; layer++)
	{
		for (int y = yStart; y <= yStop; y++)
		{
			for (int x = xStart; x <= xStop; x++)
			{
				switch (layer)
				{
				case 0:
					w->chunks(chunkPos[indexX][indexY]).DrawType(chunkRects[indexX][indexY], gfx);
					break;
					/*
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
					//gfx.DrawRect(GetChunkRect(mChunk), Colors::Red);
					break;
					*/
				}
				indexX++;
			}
			indexY++;
		}
	}
	
}