#pragma once
#include "Rect.h"
struct Materials
{
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

