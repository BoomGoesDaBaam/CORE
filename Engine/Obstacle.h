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
	int steps = 0;
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
	void Draw(RectF tileRect, Vei2 tilePos, Graphics& gfx)const			//	'tileRect' = Rect of tile where (Vei2(0, -1) && Vei2(-1, 0) != index) == true
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
