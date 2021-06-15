#pragma once
#include "Rect.h"
#include <map>
#include <vector>
#include "Settings.h"
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
	Type type = Slot::Type::Empty;
public:
	Slot() { type = Slot::Type::Empty; }
	Slot(int id) :itemId(id)
	{
		durability = Settings::itemStats[id].durability;
		type = Empty;
		if (id == 0)
		{
			type = Holdable;
		}
		if (id == 1)
		{
			type = Holdable;
		}
		if (id == 2)
		{
			type = Armor;
		}
		if (id == 3)
		{
			type = Bonus;
		}
		if (id == 4)
		{
			type = Bonus;
		}
		if (id == 5)
		{
			type = Armor;
		}
		if (id == 6)
		{
			type = Simple;
		}
		if (id == 7)
		{
			type = Simple;
		}
		if (id == 8)
		{
			type = Simple;
		}
		if (id == 9)
		{
			type = Armor;
		}
		if (id == 10)
		{
			type = Holdable;
		}
		if (id == 11)
		{
			type = Holdable;
		}
		if (id == 12)
		{
			type = Holdable;
		}
	}
	int GetId()
	{
		return itemId;
	}
	bool IsBroken()
	{
		return durability == 0;
	}
	bool Used()
	{
		if (durability > 0)
		{
			durability--;
			return true;
		}
		return false;
	}
	Slot::Type GetType()
	{
		return type;
	}
};
class Inventory
{
	std::vector<std::unique_ptr<Slot>> slots = {};
	std::vector<Slot::Type> slotsType;
	int type = -1;
public:
	Inventory() = delete;
	Inventory(int type)			//type == 0 => Unit Inventory		type == 1 => Box Inventory		type == 2 => Storage Inventory
		:
		type(type)
	{
		if (type == 0)
		{
			for (int i = 0; i < 8; i++)
			{
				slotsType.push_back(Slot::Type::Simple);
				slots.push_back(nullptr);
			}
			slotsType[0] = Slot::Type::Holdable;
			slotsType[1] = Slot::Type::Bonus;
			slotsType[2] = Slot::Type::Armor;
			slotsType[3] = Slot::Type::Bonus;
		}
		if (type == 1)
		{
			for (int i = 0; i < 9; i++)
			{
				slotsType.push_back(Slot::Type::Simple);
				slots.push_back(nullptr);
			}
		}
		if (type == 2)
		{
			for (int i = 0; i < 25; i++)
			{
				slotsType.push_back(Slot::Type::Simple);
				slots.push_back(nullptr);
			}
		}
	}
	Inventory(Inventory& other)
	{
		*this = other;
	}
	//Obstacle(Obstacle&& obst) {}
	Inventory& operator=(const Inventory& other)
	{
		slotsType = other.slotsType;
		for (int i = 0; i < other.slots.size(); i++)
		{
			if (other.slots[i].get() != nullptr)
				slots.push_back(std::make_unique<Slot>(*other.slots[i].get()));
			else
				slots.push_back(nullptr);
		}
		type = other.type;
		return *this;
	}
	bool SetItem(std::unique_ptr<Slot>&& item, int flatID)
	{
		assert(flatID >= 0 && flatID < slots.size());
		if (ItemFitsForSlotFlat(&item, flatID))
		{
			slots[flatID] = std::move(item);
			return true;
		}
		return false;
	}
	bool ItemFitsForSlotFlat(std::unique_ptr<Slot>* item, int flatID)
	{
		assert(flatID >= 0 && flatID < slots.size());
		return (slotsType[flatID] == item->get()->GetType() || slotsType[flatID] == Slot::Type::Simple) && slots[flatID].get() == nullptr;
	}
	bool WouldFitWhenEmptyFlat(std::unique_ptr<Slot>* item, int flatID)
	{
		assert(item->get() != nullptr);
		return item->get()->GetType() == slotsType[flatID] || slotsType[flatID] == Slot::Type::Simple;
	}
	std::unique_ptr<Slot>* GetItem(int flatID)
	{
		assert(flatID >= 0 && flatID < slots.size());
		return &slots[flatID];
		return nullptr;
	}
	void ItemUsed(int itemID)
	{
		for (int i = 0; i < slots.size(); i++)
		{
			if (slots[i].get() != nullptr && slots[i].get()->GetId() == itemID && !slots[i].get()->IsBroken() && slotsType[i] != Slot::Type::Simple)
			{
				slots[i].get()->Used();
				return;
			}
		}
	}
	bool HasItemNotBroken(int itemID)
	{
		for (int i = 0; i < slots.size(); i++)
		{
			if (slots[i].get() != nullptr && slots[i].get()->GetId() == itemID && !slots[i].get()->IsBroken() && slotsType[i] != Slot::Type::Simple)
			{
				return true;
			}
		}
		return false;
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

