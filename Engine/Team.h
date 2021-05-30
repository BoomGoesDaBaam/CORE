#pragma once
#include "Rect.h"
#include <map>
struct Materials
{
	std::map<std::string, float> values;
	Materials()
	{
		//New Ressources need to be added in TranslateRessources

		//IT
		values["computer chips"] = 2500.f;//not added to showresorrces
		//Ressources
		values["wood"] = 2500.f;
		values["iron"] = 1000;
		values["sand"] = 200;
		values["stone"] = 2000;
		values["copper"] = 200;
		values["gold"] = 200;
		values["aluminum"] = 200;
		values["emerald"] = 200;
		values["sapphire"] = 200;
		values["robin"] = 200;
		values["diamond"] = 200;
		values["amber"] = 200;
		values["silicon"] = 200; //not added to showresorrces
		values["lithium"] = 200; //not added to showresorrces

		//Materials
		values["steel"] = 200;
		values["plastic"] = 200;
		values["concrete"] = 2000;
		values["glass"] = 200;
		values["ceramics"] = 200;
		values["snow"] = 100;
		values["bricks"] = 2000; 
		values["slate"] = 1011; 

		//Organics
		values["corals"] = 200;
		values["sticks"] = 205.f;
		values["leafes"] = 200;
		values["wool"] = 200;
		values["leather"] = 120.f;
		values["fur"] = 200;
		values["meat"] = 5;
		values["fish"] = 5;
		values["berrys"] = 5;
		values["apples"] = 200;
		values["cactus"] = 200;
		values["sapling"] = 200;		//not added to showresorrces
		values["feather"] = 200;		//not added to showresorrces
		values["maxUnits"] = 5;		//not added to showresorrces
		values["units"] = 5;		//not added to showresorrces
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
	void Set(std::map<std::string, float> set)
	{
		for (auto const& x : set)
		{
			values[x.first] = set[x.first];
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

