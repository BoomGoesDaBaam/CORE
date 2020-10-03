#pragma once
#include "Rect.h"
struct Materials
{
	//Resources
	int wood = 10;
	int iron = 20;
	int sand = 30;
	int stone = 20;
	int copper = 20;
	int gold = 20;
	int aluminum = 30;
	int emerald = 20;
	int sapphire = 20;
	int robin = 30;
	int diamond = 20;
	int amber = 30;

	//Materials
	int steel = 20;
	int plastic = 20;
	int concrete = 30;
	int glass = 30;
	int ceramics = 20;

	//Organics
	int corals = 20; 
	int sticks = 30;
	int leaves = 20;
	int wool = 20;
	int leather = 30;
	int fur = 20;
	int meat = 20;
	int fish = 30;
	int berrys = 20;
	int apples = 20;
	int cactus = 30;
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
};

