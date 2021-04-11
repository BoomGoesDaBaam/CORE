#pragma once
#include "Rect.h"
#include <map>
struct Materials
{
	std::map<std::string, float> values;
	Materials()
	{
		//Ressources
		values["wood"] = 2500.f;
		values["iron"] = 0;
		values["sand"] = 0;
		values["stone"] = 2000;
		values["copper"] = 0;
		values["gold"] = 0;
		values["aluminum"] = 0;
		values["emerald"] = 0;
		values["sapphire"] = 0;
		values["robin"] = 0;
		values["diamond"] = 0;
		values["amber"] = 0;

		//Materials
		values["steel"] = 0;
		values["plastic"] = 0;
		values["concrete"] = 2000;
		values["glass"] = 0;
		values["ceramics"] = 0;
		values["snow"] = 5005;
		values["bricks"] = 2000; 
		values["slate"] = 1011; 

		//Organics
		values["corals"] = 0;
		values["sticks"] = 205.f;
		values["leafes"] = 0;
		values["wool"] = 0;
		values["leather"] = 120.f;
		values["fur"] = 0;
		values["meat"] = 5;
		values["fish"] = 5;
		values["berrys"] = 5;
		values["apples"] = 0;
		values["cactus"] = 0;
	}
	bool Has(std::map<std::string, float> lookFor)
	{
		for (auto const& x : lookFor)
		{
			if (values[x.first] < lookFor[x.first])
			{
				return false;
			}
			/*
			std::cout << x.first  // string (key)
				<< ':'
				<< x.second // string's value 
				<< std::endl;
				*/
		}
		return true;
	}
	void Remove(std::map<std::string, float> remove)
	{
		for (auto const& x : remove)
		{
			assert(values[x.first] > remove[x.first]);
			values[x.first] -= remove[x.first];
		}
	}
	void Add(std::map<std::string, float> add)
	{
		for (auto const& x : add)
		{
			values[x.first] += add[x.first];
		}
	}
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

