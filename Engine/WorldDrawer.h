#pragma once
#include "Rect.h"
#include "World.h"
#include "ResourceCollection.h"
#include "Vec3.h"
class WorldDrawer
{
	Vei2 drawNCells = Vei2(0,0);
	Matrix<RectF> chunkRects;
	Matrix<Vei2> chunkPos;
	World* w = nullptr;
	Graphics& gfx;
	sharedResC resC;

	int xStart = 0;
	int xStop = 0;
	int yStart = 0;
	int yStop = 0;
public:
	WorldDrawer(Graphics& gfx, sharedResC resC);
	void CalculateRects(World* w);
	void Draw();
};

