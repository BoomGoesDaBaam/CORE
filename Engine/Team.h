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

class Slot
{
public:
	enum Type
	{
		Holdable,
		Armor,
		Bonus,
		Simple,
		Empty
	};
private:
	int itemId;
	int durability;	//-1 == Item has no durability	0 == Item has no durability left
	Type type;
public:
	Slot() { type = Slot::Type::Empty; }
	Slot(int id, int durability = -1) :itemId(itemId), durability(durability)
	{
		type = Empty;
		if (id == 0)
		{
			type == Holdable;
		}
		if (id == 1)
		{
			type == Holdable;
		}
		if (id == 2)
		{
			type == Holdable;
		}
		if (id == 3)
		{
			type == Bonus;
		}
		if (id == 4)
		{
			type == Bonus;
		}
		if (id == 5)
		{
			type == Bonus;
		}
		if (id == 6)
		{
			type == Simple;
		}
		if (id == 7)
		{
			type == Simple;
		}
		if (id == 8)
		{
			type == Simple;
		}
		if (id == 9)
		{
			type == Armor;
		}
		if (id == 10)
		{
			type == Holdable;
		}
	}
	int GetId()
	{
		return itemId;
	}
	int Used()
	{
		if (durability > 0)
		{
			durability--;
		}
	}
	bool IsBroken()
	{
		return durability == 0;
	}
	Slot::Type GetType()
	{
		return type;
	}
};
class Inventory
{
	Slot hand1, hand2;
	Slot armor, bonus;
	Slot item1, item2, item3, item4;
public:
	Inventory(Slot hand1 = Slot(), Slot hand2 = Slot(), Slot armor = Slot(), Slot bonus = Slot(), Slot item1 = Slot(), Slot item2 = Slot(), Slot item3 = Slot(), Slot item4 = Slot())
		:hand1(hand1), hand2(hand2), armor(armor), bonus(bonus), item1(item1), item2(item2), item3(item3), item4(item4)
	{}
	//SET
	bool SetHand1(Slot hand1)
	{
		//if(hand1.GetType == Slot::Type::Holdable)
		this->hand1 = hand1;
	}
	bool SetHand2(Slot hand2)
	{
		this->hand2 = hand2;
	}
	bool SetArmor(Slot armor)
	{
		this->armor = armor;
	}
	bool SetBonus(Slot bonus)
	{
		this->bonus = bonus;
	}
	bool SetItem1(Slot item1)
	{
		this->item1 = item1;
	}
	bool SetItem2(Slot item2)
	{
		this->item2 = item2;
	}
	bool SetItem3(Slot item3)
	{
		this->item3 = item3;
	}
	bool SetItem4(Slot item3)
	{
		this->item3 = item3;
	}
	//GET
	const Slot* GetHand1()
	{
		return &hand1;
	}
	const Slot* GetHand2()
	{
		return &hand2;
	}
	const Slot* GetArmor()
	{
		return &armor;
	}
	const Slot* GetBonus()
	{
		return &bonus;
	}
	const Slot* GetItem1()
	{
		return &item1;
	}
	const Slot* GetItem2()
	{
		return &item2;
	}
	const Slot* GetItem3()
	{
		return &item3;
	}
	const Slot* GetItem4()
	{
		return &item3;
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

