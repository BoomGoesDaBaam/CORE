#pragma once
#include "Rect.h"
#include <fstream>
#include <map>
namespace Settings
{
	/*		### Types ###
			0 = water				5 = snow				10 = mountains (high)
			1 = dirt				6 = coral reef			11 = canjon	   (high)
			2 = ice					7 = Stone				12 = lavahills (high)
			3 = desert				8 = lava				13 = candyland
			4 = nutritious dirt		9 = savanne				14 = swamp

			### Obstacles ###		mo = multiobstacle
			0 = tent								
			1 = tree		(mo #1)					
			2 = bonfire						
			3 = townhall			
			4= other tree			
			5 = cactus
			6 = box
			7 = stone
			8 = fir
			9 = big stone
			10 = worker
			11 = deer
			12 = cow
			13 = horse
			14 = billy goat
			15 = sheep
			16 = chicken
			17 = pig
			18 = penguin
			19 = mouse
			20 = snake
			21 = iglu
			22 = wood house
			23 = stone house
			24 = brick house
			25 = skyscraper
			26 = villa

			27 = lumberjack hut
			28 = nature house
			29 = finishing house
			30 = wrought
			31 = windmill
			32 = hunting house
			33 = mine
			34 = sawmill
			35 = oil drill
			36 = shipyard
			37 = factory
			38 = ship port
			39 = power plant
			40 = solar panel
			41 = wind power plant
			42 = airport
			43 = construction site 1x1
			44 = construction site 2x2
			45 = construction site 3x3
			46 = construction site 4x4
			47 = construction site 5x5
			48 = power storage
			49 = CAR FACTORY
			50 = storage

			### Masked Array ###
			0 = swamp

			### Windows ###
			0 = Fieldinformaion
			1 = Unit Info
			2 = Next Turn 
			3 = Buildselection button
			4 = Build selection screen
			5 = Load Gamefield

			### Frame Types ###		
			0 = frame with bar to open
			1 = frame with custom pixel offset
			
			### Object Display ###
			0 = build menu like

			### Error returns ###
			-1 = something happened
			-2 = Matrix out of bounds
	*/
	//	'constexpr' = already generated in compiletime
	//	'const'		= generated in runtime

	static constexpr int nDiffFieldTypes = 15;										//Array Nullterminator == -1
	static constexpr int nDiffObstackes = 11;
	static constexpr int nDiffWindows = 1;
	static constexpr int typeLayer[] = { 6,0,8,14,4,3,9,1,5,7,2,13,12,10,11,-1 };		//Orden in wich the connections are drawn
	static constexpr int hillTypesARE[] = { 11,10,12,-1 };
	static constexpr int liquidsTypesARE[] = { 0,6,8,-1 };
	static constexpr int maskTypesARE[] = { 14,-1 };							    //Types like swamp that need to copy watertexture behind
	static constexpr int groundedTypesARE[] = { 13,1,2,3,4,5,7,9,-1 };				//Types where you can place normal Buildings	
	static constexpr int CellSplitUpIn = 25;										//every cell has n*n supcells (ACCTUALLY HARDCODED!!! DONT EVEN TRY TO CHANGE SOMETHING !!! REALLY)
	static constexpr int chunkHasNCells = 5;
	static constexpr int chunkHasNTiles = chunkHasNCells * CellSplitUpIn;

	static const int obstacleStates[] = { 1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
										1,1,1,1,1,1,
										1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};//[27-50]
	static const Vei2 obstacleSizes[] = { Vei2(2,2), Vei2(1,1), Vei2(1,1), Vei2(4,4), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(2,2), Vei2(4,4), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1),
										Vei2(3,3),Vei2(3,3), Vei2(4,4), Vei2(4,4), Vei2(4,4), Vei2(5,5),
										Vei2(4,4), Vei2(3,3), Vei2(4,4), Vei2(3,3), Vei2(5,5), Vei2(3,3), Vei2(5,5), Vei2(3,3), Vei2(5,5), Vei2(5,5),//[27-36]
										Vei2(4,4), Vei2(5,5), Vei2(4,4), Vei2(3,3), Vei2(3,3), Vei2(5,5), Vei2(1,1), Vei2(2,2), Vei2(3,3), Vei2(4,4),//[37-46]
										Vei2(5,5), Vei2(2,2), Vei2(5,5), Vei2(4,4) };//[47-50]

	static const int obstacleBaseHP[] = { 20,20,20, 100, 20, 10, 5, 100, 20, 100, 50, 10,10,10,25,10,5,10,5,1,25, 
										10,30,200,150,100,80,
										80,80,80,80,80,80,80,80,80,80,//[27-36]
										80,80,80,80,80,80,80,80,80,80,//[37-46]
										80,80,80,80 };//[47-50]
	static const int obstacleMovesPerTurn[] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,10,10,10,10,10,10,5,10,10,30,15 
												-1,-1,-1,-1,-1,-1,
												-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 ,//[27-36]
												-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 //[37-46]
												- 1,-1,-1,-1};//[47-50]
	static const std::map<std::string, float> neededRes[] = { {{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"wood",10},{"sticks",5}},		{{"concrete",300},{"wood",200}},		{{"leather",50},{"sticks",30}},
															{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},
															{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},
															{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},		{{"leather",50},{"sticks",30}},
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},
															{{"concrete",500}},		{{"slate",100},{"bricks",200},{"concrete",200}},
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[27-31]
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[32-36]
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[37-41]
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[42-46]   
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[47-51]   

	};
	static const std::map<std::string, float> lootForDestroying[] = { {{"leather",25},{"sticks",15}},		{{"wood",200},{"sticks",50},{"leafes",2},{"apples",5}},		{{"wood",5},{"sticks",15}},		{{"concrete",150},{"wood",100}},		{{"wood",200},{"sticks",50},{"leafes",2},{"apples",5}},
															{{"cactus",3}},		{{"iron",3},{"gold",0.1}},		{{"stone",20},{"iron",0.5}},		{{"wood",150},{"sticks",30},{"leafes",2}},		{{"stone",40},{"iron",2},{"copper",1}},
															{{"",50},{"",30}},		{{"leather",25},{"meat",20}},		{{"leather",25},{"meat",25}},		{{"leather",25},{"meat",10}},		{{"fur",25},{"meat",10}},
															{{"wool",3},{"meat",10}},		{{"",50},{"meat",3}},		{{"meat",30},{"",30}},		{{"meat",20},{"",30}},		{{"meat",1},{"",30}},
															{{"meat",1},{"",30}},		{{"snow",25},{"wood",10}},		{{"wood",350}},		{{"stone",200},{"wood",100}},		{{"bricks",100},{"wood",50},{"stone",50}},
															{{"concrete",250}},		{{"slate",50},{"bricks",100},{"concrete",100}},
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[27-31]
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[32-36]
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[37-41]
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[42-46]   
															{{"leather",50},{"sticks",30}},		{{"snow",50},{"wood",20}},		{{"wood",700}},		{{"stone",400},{"wood",200}},		{{"bricks",200},{"wood",100},{"stone",100}},//[47-51]  
	};
	static const int obstacleAttackRange[] = { 0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0 };
	static const int obstacleAttackDmg[] = { 0,0,0,0,0,0,0,0,0,0,100,20,0,15,30,00,0,20,0,40,0,0,0,0,0,0 };
	static const std::vector<Vei2> multiObstacleSize[] = { {Vei2(5,5)}, {Vei2(5,5)} };					//sizes of hidden frames
	static const Vei2 multiObstaclePos[] = { Vei2(-2,-2), Vei2(-2,-2) };									//relativ pos of hidden frames in tiles

	//Graphic options
	//static bool displayObstacles = true;

	//Game options
	static bool obstaclesOn = true;
	static bool spawnObstacles = true;

	static bool framesOn = true;


	//
	static int lang = 0;															//choosen language (0 = english, 1 = german)

	const std::string lang_fieldInformation[] = { "Field information", "Feldinformationen" };
	const std::string lang_buildmenu[] = { "Build menue", "Baumenue" };
	const std::string lang_unitInfo[] = { "Unit Information", "Einheitsinformationen" };
	const std::string lang_buildings[] = { "Buildings", "Gebeaude" };
	const std::string lang_housing[] = { "Housing", "Unterkuenfte" };
	const std::string lang_productions[] = { "Productions", "Produktionen" };
	const std::string lang_agility[] = { "Agility", "Beweglichkeit" };
	const std::string lang_decoration[] = { "Decoration", "Dekoration" };

	const std::string lang_constructionMaterials[] = { "Construction materials", "Baumaterialien" };
	const std::string lang_resources[] = { "Resources", "Rohstoffe" };
	const std::string lang_materials[] = { "Materials", "Werkstoffe" };
	const std::string lang_organic[] = { "Organic", "Oragnisches" };

	const std::string lang_wood[] = { "wood", "Holz" };
	const std::string lang_iron[] = { "iron", "Eisen" };
	const std::string lang_sand[] = { "sand", "Sand" };
	const std::string lang_stone[] = { "stone", "Stein" };
	const std::string lang_copper[] = { "copper", "Kupfer" };
	const std::string lang_gold[] = { "gold", "Gold" };
	const std::string lang_aluminum[] = { "aluminum", "Aluminium" };
	const std::string lang_emerald[] = { "emerald", "Smaragd" };
	const std::string lang_sapphire[] = { "sapphire", "Saphir" };
	const std::string lang_robin[] = { "robin", "Robin" };
	const std::string lang_diamond[] = { "diamond", "Diamant" };
	const std::string lang_amber[] = { "amber", "Bernstein" };

	const std::string lang_steel[] = { "steel", "Stahl" };
	const std::string lang_plastic[] = { "plastic", "Kunststoff" };
	const std::string lang_concrete[] = { "concrete", "Beton" };
	const std::string lang_glass[] = { "glass", "Glas" };
	const std::string lang_ceramics[] = { "ceramics", "Keramik" };
	const std::string lang_bricks[] = { "bricks", "Ziegeln" };
	const std::string lang_slate[] = { "slate", "schiefer" };

	const std::string lang_corals[] = { "corals", "Korallen" };
	const std::string lang_sticks[] = { "sticks", "Stoecke" };
	const std::string lang_leaves[] = { "leaves", "Blaetter" };
	const std::string lang_wool[] = { "wool", "Wolle" };
	const std::string lang_leather[] = { "leather", "Leder" };
	const std::string lang_fur[] = { "fur", "Fell" };
	const std::string lang_meat[] = { "meat", "Fleisch" };
	const std::string lang_fish[] = { "fish", "Fisch" };
	const std::string lang_berrys[] = { "berrys", "Beeren" };
	const std::string lang_apples[] = { "apples", "Aepfel" };
	const std::string lang_cactus[] = { "cactus", "Kaktus" };


	const std::string lang_water[] = { "water", "Wasser" };
	const std::string lang_plans[] = {"plans", "Ebene"};
	const std::string lang_ice[] = { "ice", "Eis" };
	const std::string lang_desert[] = { "Desert", "Wueste" };
	const std::string lang_nutritiousPlans[] = { "nutritious plans", "Naehrreiche Ebene" };
	const std::string lang_snow[] = { "snow", "Schnee" };
	const std::string lang_coralreef[] = { "Coral reef", "Korallenriff" };
	const std::string lang_stonelands[] = { "stony", "Steinig" };
	const std::string lang_lava[] = { "lava", "Lava" };
	const std::string lang_savannah[] = { "savannah", "Savanne" };
	const std::string lang_hills[] = { "hills", "Berge" };
	const std::string lang_canyon[] = { "canyon", "Canyon" };
	const std::string lang_lavahills[] = { "lavahills", "Lavaberge" };
	const std::string lang_candyland[] = { "candyland", "Suessigkeitenland" };
	const std::string lang_swamp[] = { "swamp", "Sumpf" };

	const std::string lang_unknown[] = { "unbekannt", "unknown" };
	const std::string lang_noInformation[] = { "no information", "Keine Informationen vorhanden" };
	const std::string lang_flora[] = { "Flora", "Flora" };			// Goettin
	const std::string lang_Obstacle[] = { "Obstacle", "Hindernis" };			// Goettin
	const std::string lang_kilogram[] = { "kg", "kg" };		
	const std::string lang_stepsLeft[] = { "steps left", "Schritte ueber" };


	//Obstacles

	const std::string lang_tree[] = { "tree", "Baum" };
	const std::string lang_fir[] = { "fir", "Tanne" };
	const std::string lang_box[] = { "box", "Box" };
	const std::string lang_unit[] = { "unit", "Einheit" };
	const std::string lang_worker[] = { "worker", "Arbeiter" };

	const std::string lang_unitName[] = { "Unit name", "Name der Einheit" };
	const std::string lang_hp[] = { "Health points", "Lebenspunkte" };
	const std::string lang_team[] = { "Team", "Team" };

	//Animals
	const std::string lang_deer[] = { "deer", "Hirsch" };
	const std::string lang_cow[] = { "cow", "Kuh" };
	const std::string lang_horse[] = { "horse", "Pferd" };
	const std::string lang_billygoat[] = { "billy goat", "Geissbock" };
	const std::string lang_sheep[] = { "sheep", "Schaf" };
	const std::string lang_chicken[] = { "chicken", "Huhn" };
	const std::string lang_pig[] = { "pig", "Schwein" };
	const std::string lang_penguin[] = { "penguin", "Pinguin" };
	const std::string lang_mouse[] = { "mouse", "Maus" };
	const std::string lang_snake[] = { "snake", "Schlange" };

	//Building
	const std::string lang_tent[] = { "tent", "Zelt" };
	const std::string lang_bonfire[] = { "bonfire", "Lagerfeuer" };
	const std::string lang_townhall[] = { "townhall", "Rathaus" };
	const std::string lang_igloo[] = { "igloo", "iglu" };
	const std::string lang_woodenHouse[] = { "wooden house", "Holzhaus" };
	const std::string lang_stoneHouse[] = { "stone house", "Steinhaus" };
	const std::string lang_brickhouse[] = { "brickhouse", "Ziegelhaus" };
	const std::string lang_skyscraper[] = { "skyscraper", "Hochhaus" };
	const std::string lang_villa[] = { "villa", "Villa" };

	//Productions
	const std::string lang_lumberjackHut[] = { "lumberjack hut", "Holzfaellerhuette" };
	const std::string lang_natureHouse[] = { "nature house", "Naturhaus" };
	const std::string lang_finishingHouse[] = { "finishing house", "Fischerei" };
	const std::string lang_wrought[] = { "wrought", "Schmiede" };
	const std::string lang_windmill[] = { "windmill", "Windmuehle" };
	const std::string lang_huntingHouse[] = { "hunting house", "Jaegerei" };
	const std::string lang_mine[] = { "mine", "Mine" };
	const std::string lang_sawmill[] = { "sawmill", "Saegewerk" };
	const std::string lang_oilDrill[] = { "oil drill", "Oelbohrer" };
	const std::string lang_shipyard[] = { "shipyard", "Schiftwerft" };
	const std::string lang_factory[] = { "factory", "Fabrik" };
	const std::string lang_shipPort[] = { "ship port", "Hafen" };
	const std::string lang_powerPlant[] = { "power plant", "Kraftwerk" };
	const std::string lang_solarPanel[] = { "solar panel", "Sonnenkraftwerk" };
	const std::string lang_windPowerPlant[] = { "wind power plant", "Windkraftwerk" };
	const std::string lang_airport[] = { "airport", "Flughafen" };
	const std::string lang_constructionSite[] = { "construction site", "Baustelle" };
	const std::string lang_powerStorage[] = { "power storage", "Stromspeicher" };
	const std::string lang_carFactory[] = { "car factory", "Autofabrik" };
	const std::string lang_storage[] = { "storage", "Lagerhalle" };


	//


	static constexpr float percentForGrab[] = { 0.05f, 0.05f };
	static bool anyOfUnit(int type)
	{
		if (type >= 10 && type <=20)
		{
			return true;
		}
		return false;
	}
	static bool anyOfChangeSizeWhenNear(int type)		//trees that have a diffrent look when unit is nearby
	{
		if (type == 1 || type == 4)
		{
			return true;
		}
		return false;
	}
	static bool anyOfAnimals(int type)		
	{
		if (type >= 11 && type <= 20)
		{
			return true;
		}
		return false;
	}
	static bool anyOfPlants(int type)
	{
		if (type == 1 || type == 4 || type == 5 || type == 8)
		{
			return true;
		}
		return false;
	}
	static bool anyOfHillTypes(int Type)
	{
		for (int i = 0; i < sizeof(hillTypesARE) / sizeof(hillTypesARE[0]); i++)
		{
			if (Type == hillTypesARE[i])
			{
				return true;
			}
		}
		return false;
	}
	static bool anyGroundedTypes(int Type)
	{
		for (int i = 0; i < sizeof(groundedTypesARE) / sizeof(groundedTypesARE[0]); i++)
		{
			if (Type == groundedTypesARE[i])
			{
				return true;
			}
		}
		return false;
	}
	static bool anyLiquidType(int Type)
	{
		for (int i = 0; i < sizeof(liquidsTypesARE) / sizeof(liquidsTypesARE[0]); i++)
		{
			if (Type == liquidsTypesARE[i])
			{
				return true;
			}
		}
		return false;
	}
	static bool anyMaskedType(int Type)
	{
		for (int i = 0; i < sizeof(maskTypesARE) / sizeof(maskTypesARE[0]); i++)
		{
			if (Type == maskTypesARE[i])
			{
				return true;
			}
		}
		return false;
	}
	static int translateIntoMaskedType(int Type)				//translates type into index for resC->tC.maskedFields
	{
		assert(anyMaskedType(Type));
		switch (Type)
		{
		case 14:
			return 0;
		}
		return -1;
	}
	static int Obstacle2MultiObstacle(int Type)				//translates type into index for resC->tC.maskedFields
	{
		switch (Type)
		{
		case 1:
			return 0;
		case 4:
			return 1;
		}
		return -1;
	}
	static int ArrSize(int* start)		// max size is 999	arrays need "-1" terminator
	{
		int n = 0;
		for (int i = 0; start[i] != -1 && i <= 1000; i++)
		{
			n++;
		}
		assert(n < 1000);
		return n;
	}
	static int ArrSize(const int* start)
	{
		int n = 0;
		for (int i = 0; start[i] != -1 && i <= 1000; i++)
		{
			n++;
		}
		assert(n < 1000);
		return n;
	}
	static std::vector<int> CombineTwoArrays(const int* first, const int* second)
	{
		std::vector<int> v;
		int fSize = ArrSize(first) + ArrSize(second);
		int sSize = ArrSize(first) + ArrSize(second);
		for (int i = 0; i < fSize; i++)
		{
			v.push_back(first[i]);
		}
		for (int i = 0; i < sSize; i++)
		{
			v.push_back(second[i]);
		}
		return v;
	}
	
	static std::string GetTypeString(int type)
	{
		switch (type)
		{
		case -1:
			return lang_noInformation[lang];
			break;
		case 0:
			return lang_water[lang];
			break;
		case 1:
			return lang_plans[lang];
			break;
		case 2:
			return lang_ice[lang];
			break;
		case 3:
			return lang_desert[lang];
			break;
		case 4:
			return lang_nutritiousPlans[lang];
			break;
		case 5:
			return lang_snow[lang];
			break;
		case 6:
			return lang_coralreef[lang];
			break;
		case 7:
			return lang_stonelands[lang];
			break;
		case 8:
			return lang_lava[lang];
			break;
		case 9:
			return lang_savannah[lang];
			break;
		case 10:
			return lang_hills[lang];
			break;
		case 11:
			return lang_canyon[lang];
			break;
		case 12:
			return lang_lavahills[lang];
			break;
		case 13:
			return lang_candyland[lang];
			break;
		case 14:
			return lang_swamp[lang];
			break;
		case 15:
			return "Settings::GetTypeString needs an update";
			break;
		}
		return lang_unknown[lang];
	}
	static std::string GetObstacleString(int type)
	{
		switch (type)
		{
		case -1:
			return lang_noInformation[lang];
			break;
		case 0:
			return lang_tent[lang];
			break;
		case 1:
			return lang_tree[lang];
			break;
		case 2:
			return lang_bonfire[lang];
			break;
		case 3:
			return lang_townhall[lang];
			break;
		case 4:
			return lang_tree[lang];
			break;
		case 5:
			return lang_cactus[lang];
			break;
		case 6:
			return lang_box[lang];
			break;
		case 7:
			return lang_stone[lang];
			break;
		case 8:
			return lang_fir[lang];
			break;
		case 9:
			return lang_stone[lang];
			break;
		case 10:
			return lang_worker[lang];
			break;
		case 11:
			return lang_deer[lang];
			break;
		case 12:
			return lang_cow[lang];
			break;
		case 13:
			return lang_horse[lang];
			break;
		case 14:
			return lang_billygoat[lang];
			break;
		case 15:
			return lang_sheep[lang];
			break;
		case 16:
			return lang_chicken[lang];
			break;
		case 17:
			return lang_pig[lang];
			break;
		case 18:
			return lang_penguin[lang];
			break;
		case 19:
			return lang_mouse[lang];
			break;
		case 20:
			return lang_snake[lang];
			break;
		case 21:
			return lang_igloo[lang];
			break;
		case 22:
			return lang_woodenHouse[lang];
			break;
		case 23:
			return lang_stoneHouse[lang];
			break;
		case 24:
			return lang_brickhouse[lang];
			break;
		case 25:
			return lang_skyscraper[lang];
			break;
		case 26:
			return lang_villa[lang];
			break;
		case 27:
			return lang_lumberjackHut[lang];
			break;
		case 28:
			return lang_natureHouse[lang];
			break;
		case 29:
			return lang_finishingHouse[lang];
			break;
		case 30:
			return lang_wrought[lang];
			break;
		case 31:
			return lang_windmill[lang];
			break;
		case 32:
			return lang_huntingHouse[lang];
			break;
		case 33:
			return lang_mine[lang];
			break;
		case 34:
			return lang_sawmill[lang];
			break;
		case 35:
			return lang_oilDrill[lang];
			break;
		case 36:
			return lang_shipyard[lang];
			break;
		case 37:
			return lang_factory[lang];
			break;
		case 38:
			return lang_shipPort[lang];
			break;
		case 39:
			return lang_powerPlant[lang];
			break;
		case 40:
			return lang_solarPanel[lang];
			break;
		case 41:
			return lang_windPowerPlant[lang];
			break;
		case 42:
			return lang_airport[lang];
			break;
		case 43:
			return lang_constructionSite[lang]+" 1x1";
			break;
		case 44:
			return lang_constructionSite[lang] + " 2x2";
			break;
		case 45:
			return lang_constructionSite[lang] + " 3x3";
			break;
		case 46:
			return lang_constructionSite[lang] + " 4x4";
			break;
		case 47:
			return lang_constructionSite[lang] + " 5x5";
			break;
		case 48:
			return lang_powerStorage[lang];
			break;
		case 49:
			return lang_carFactory[lang];
			break;
		case 50:
			return lang_storage[lang];
			break;
		default:
			return "Settings::GetObstacleString needs an update";
			break;
		}
		return lang_unknown[lang];
	}
};