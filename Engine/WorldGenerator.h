#pragma once
#include "World.h"
class WorldGenerator
{
	World* world;
public:
	WorldGenerator(World* world);
	void Generate();
};

