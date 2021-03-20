#pragma once
#include "Rect.h"
struct Materials
{
	//Resources
	float wood = 5.f;
	float iron = 0;
	float sand = 0;
	float stone = 0;
	float copper = 0;
	float gold = 0;
	float aluminum = 0;
	float emerald = 0;
	float sapphire = 0;
	float robin = 0;
	float diamond = 0;
	float amber = 0;

	//Materials
	float steel = 0;
	float plastic = 0;
	float concrete = 0;
	float glass = 0;
	float ceramics = 0;

	//Organics
	float corals = 0; 
	float sticks = 2.5f;
	float leaves = 0;
	float wool = 0;
	float leather = 5;
	float fur = 0;
	float meat = 5;
	float fish = 5;
	float berrys = 5;
	float apples = 0;
	float cactus = 0;
};

class Team
{
	std::string teamname = "kein Name";
	Materials m;
public:

	Team(std::string teamname)
	{
		this->teamname = teamname;
	}
	Materials& GetMaterials() { return m; }
	std::string GetTeamName() { return teamname; }
};

