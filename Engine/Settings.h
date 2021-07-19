#pragma once
#include "Rect.h"
#include <fstream>
#include <map>
#include <algorithm>
namespace Settings
{
	/*
	to do list:
		-animals consume food every turn finish
	
	*/
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
			4 = other tree			
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

			### Items
			0 = iron axe
			1 = iron sword
			2 = iron shield
			3 = range chain
			4 = heal ring
			5 = heal Head
			6 = tulip
			7 = rose 
			8 = lily
			9 = iron armor
			10 = iron pickaxe
			11 = bow
			12 = sniper

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
	static constexpr int nDiffObstacles = 51;
	static constexpr int nDiffWindows = 1;
	static constexpr int typeLayer[] = { 0,6,8,14,4,3,9,1,5,7,2,13,12,10,11,-1 };		//Orden in wich the connections are drawn
	static constexpr int hillTypesARE[] = { 11,10,12,-1 };
	static constexpr int liquidsTypesARE[] = { 0,6,8,-1 };
	static constexpr int maskTypesARE[] = { 14,-1 };							    //Types like swamp that need to copy watertexture behind
	static constexpr int groundedTypesARE[] = { 13,1,2,3,4,5,7,9,-1 };				//Types where you can place normal Buildings	
	static constexpr int CellSplitUpIn = 25;										//every cell has n*n supcells (ACCTUALLY HARDCODED!!! DONT EVEN TRY TO CHANGE SOMETHING !!! REALLY)
	static constexpr int chunkHasNCells = 5;
	static constexpr int chunkHasNTiles = chunkHasNCells * CellSplitUpIn;
	//Type Traits
	static constexpr int obstacleTrait_education[] = { 3,-1 };
	static constexpr int obstacleTrait_heal[] = { 2,3,-1 };
	static constexpr int obstacleTrait_attack[] = { 3,10,11,12,13,14,15,16,17,18,19,20,27,-1 };
	//Categorisation
	static const std::vector<int> anyOfPlantsVec = { 1,4,5,8};
	static const std::vector<int> anyOfAnimalsVec = { 11,12,13,14,15,16,17,18,19,20 };
	static const std::vector<int> anyUnitsVec = { 10 };
	static const std::vector<int> anyStorageVec = { 6, 30, 50 };
	static const std::vector<int> anyAxeBonusVec = { 0,1,2,4,5,6,8,27,28,29,30,31,32,33,34, };
	static const std::vector<int> anyPickaxeBonusVec = { 0,2,3,5,6,7,8,9,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50 };
	static const std::vector<int> anyOfBuildingVec = {0,2,3,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,40};
	static const std::vector<int> anySawmillBoostedObstacles = { 27,30,33,35,36,37,42,43,44,45,46,47,49 };
	static const std::vector<int> anyWindmillBoostedObstacles = {  };
	//Graphic options
	//static bool displayObstacles = true;
	static int probToGrow = 50;
	//Game options
	static bool obstaclesOn = true;
	static bool spawnObstacles = true;
	static bool testMode = false;
	static bool zoomingEnabled = false;

	static bool framesOn = true;

	static int forestDensity = 8;
	static int animalDensity = 4;
	static float animalFoodCostPerTurn = 0.1f;


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
	const std::string lang_back[] = { "back", "Zurueck" };
	const std::string lang_build[] = { "build", "bauen" };
	const std::string lang_forge[] = { "forge", "Schmieden" };
	const std::string lang_attack[] = { "attack", "Angreifen" };

	const std::string lang_constructionMaterials[] = { "Construction materials", "Baumaterialien" };
	const std::string lang_resources[] = { "Resources", "Rohstoffe" };
	const std::string lang_materials[] = { "Materials", "Werkstoffe" };
	const std::string lang_organic[] = { "Organic", "Oragnisches" };

	const std::string lang_computerChip[] = { "computer chip", "computer chip" };
	
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
	const std::string lang_silicon[] = { "silicon", "Silizium" };
	const std::string lang_lithium[] = { "lithium", "Lithium" };

	const std::string lang_steel[] = { "steel", "Stahl" };
	const std::string lang_plastic[] = { "plastic", "Kunststoff" };
	const std::string lang_concrete[] = { "concrete", "Beton" };
	const std::string lang_glass[] = { "glass", "Glas" };
	const std::string lang_ceramics[] = { "ceramics", "Keramik" };
	const std::string lang_bricks[] = { "bricks", "Ziegeln" };
	const std::string lang_slate[] = { "slate", "schiefer" };
	const std::string lang_gunpowder[] = { "gunpowder", "Schiesspulver" };
	const std::string lang_silk[] = { "silk", "Seide" };

	const std::string lang_leafes[] = { "leafes", "Blaetter" };
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
	const std::string lang_feather[] = { "feather", "Feder" };
	const std::string lang_sapling[] = { "sapling", "Setzling" };

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
	const std::string lang_educate[] = { "educate", "Ausbilden" };
	const std::string lang_heal[] = { "heal", "Heilen" };
	const std::string lang_recharge[] = { "recharge", "nachladen" };
	const std::string lang_automatic[] = { "automatic chop", "automatisch faellen" };

	//Obstacles

	const std::string lang_tree[] = { "tree", "Baum" };
	const std::string lang_fir[] = { "fir", "Tanne" };
	const std::string lang_box[] = { "box", "Box" };
	const std::string lang_unit[] = { "unit", "Einheit" };
	const std::string lang_worker[] = { "worker", "Arbeiter" };
	const std::string lang_units[] = { "units", "Einheiten" };
	const std::string lang_maxUnits[] = { "maxUnits", "MaxUnits" };

	const std::string lang_unitName[] = { "Unit name", "Name der Einheit" };
	const std::string lang_hp[] = { "Health points", "Lebenspunkte" };
	const std::string lang_team[] = { "Team", "Team" };
	const std::string lang_attacksLeft[] = { "attacks left", "Verb. angriffe" };
	const std::string lang_chopsLeft[] = { "chops left", "schlaege über" };
	const std::string lang_chop[] = { "chops", "zerhacken" };
	const std::string lang_craftedItem[] = { "crafted items", "Erstellte Items" };
	const std::string lang_reparation[] = { "reparation", "Reperatur" };
	
	//Items
	const std::string lang_ironAxe[] = { "iron axe", "Eisenaxt" };
	const std::string lang_ironSword[] = { "iron sword", "Eisenschwert" };
	const std::string lang_ironShield[] = { "iron shield", "Eisenschild" };
	const std::string lang_chainOfEase[] = { "chain of ease", "Kette der Leichtigkeit" };
	const std::string lang_RingOfRejuvenation[] = { "ring of rejuvenation", "Ring der verjuengung" };
	const std::string lang_naturalWonder[] = { "natural wonder", "Naturwunder" };
	const std::string lang_tulip[] = { "tulip", "Tulpe" };
	const std::string lang_rose[] = { "rose ", "Rose" };
	const std::string lang_lily[] = { "lily", "Lilie" };
	const std::string lang_ironArmor[] = { "iron armor", "Eisenruestung" };
	const std::string lang_ironPickaxe[] = { "iron pickaxe", "Eisenspitzhacke" };
	const std::string lang_bow[] = { "bow", "Bogen" };
	const std::string lang_sniper[] = { "sniper", "Scharfschuetzengewehr" };

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

	//Statustexte
	const std::string lang_status[] = { "status", "Status" };
	const std::string lang_ressourcesAreMissing[] = { "ressources are missing", "Ressourcen fehlen" };
	const std::string lang_finishedIn[] = { "finished in", "fertig in" };
	const std::string lang_rounds[] = { "rounds", "Runden" };
	const std::string lang_nothingSelected[] = { "nothing Selected", "Nichts ausgewaehlt" };

	//Beschreibungen
	const std::string lang_TownhallInfo[] = { "This is the main building of your town. Use it to heal your surrounding units, train units or defent your town from enemies. Choose one every turn!", "Lagerhalle" };
	const std::string lang_LumberjackHutInfo[] = { "Use this building to chop wood.", "Lagerhalle" };

	//
	static std::string TranslateRessource(std::string ressource, int lang)
	{
		if (ressource == "computer")
		{
			return lang_computerChip[lang];
		}
		else if (ressource == "wood")
		{
			return lang_wood[lang];
		}
		else if (ressource == "iron")
		{
			return lang_iron[lang];
		}
		else if (ressource == "sand")
		{
			return lang_sand[lang];
		}
		else if (ressource == "stone")
		{
			return lang_stone[lang];
		}
		else if (ressource == "copper")
		{
			return lang_copper[lang];
		}
		else if (ressource == "gold")
		{
			return lang_gold[lang];
		}
		else if (ressource == "aluminum")
		{
			return lang_aluminum[lang];
		}
		else if (ressource == "emerald")
		{
			return lang_emerald[lang];
		}
		else if (ressource == "sapphire")
		{
			return lang_sapphire[lang];
		}
		else if (ressource == "robin")
		{
			return lang_robin[lang];
		}
		else if (ressource == "diamond")
		{
			return lang_diamond[lang];
		}
		else if (ressource == "amber")
		{
			return lang_amber[lang];
		}
		else if (ressource == "silicon")
		{
			return lang_silicon[lang];
		}
		else if (ressource == "lithium")
		{
			return lang_lithium[lang];
		}
		else if (ressource == "steel")
		{
			return lang_steel[lang];
		}
		else if (ressource == "plastic")
		{
			return lang_plastic[lang];
		}
		else if (ressource == "concrete")
		{
			return lang_concrete[lang];
		}
		else if (ressource == "glass")
		{
			return lang_glass[lang];
		}
		else if (ressource == "ceramics")
		{
			return lang_ceramics[lang];
		}
		else if (ressource == "snow")
		{
			return lang_snow[lang];
		}
		else if (ressource == "bricks")
		{
			return lang_bricks[lang];
		}
		else if (ressource == "slate")
		{
			return lang_slate[lang];
		}
		else if (ressource == "corals")
		{
			return lang_corals[lang];
		}
		else if (ressource == "sticks")
		{
			return lang_sticks[lang];
		}
		else if (ressource == "leafes")
		{
			return lang_leafes[lang];
		}
		else if (ressource == "wool")
		{
			return lang_wool[lang];
		}
		else if (ressource == "leather")
		{
		return lang_leather[lang];
		}
		else if (ressource == "fur")
		{
		return lang_fur[lang];
		}
		else if (ressource == "meat")
		{
		return lang_meat[lang];
		}
		else if (ressource == "fish")
		{
		return lang_fish[lang];
		}
		else if (ressource == "berrys")
		{
		return lang_berrys[lang];
		}
		else if (ressource == "apples")
		{
		return lang_apples[lang];
		}
		else if (ressource == "cactus")
		{
		return lang_cactus[lang];
		}
		else if (ressource == "sapling")
		{
		return lang_sapling[lang];
		}
		else if (ressource == "feather")
		{
		return lang_feather[lang];
		}
		else if (ressource == "silk")
		{
		return lang_silk[lang];
		}
		else if (ressource == "gunpowder")
		{
		return lang_gunpowder[lang];
		}
		else
		{
		return "ERROR:"+ressource + " not supported in Method TranslateRessources!";
		}
	}

	static constexpr float percentForGrab[] = { 0.05f, 0.05f };
	static bool anyOfCreature(int type)
	{
		return std::any_of(std::begin(anyOfAnimalsVec), std::end(anyOfAnimalsVec), [&](const int val) {return type == val; })
			|| std::any_of(std::begin(anyUnitsVec), std::end(anyUnitsVec), [&](const int val) {return type == val; });
	}
	static bool anyOfStorage(int type)
	{
		return std::any_of(std::begin(anyStorageVec), std::end(anyStorageVec), [&](const int val) {return type == val; });
	}
	static bool anyOfAxeBonus(int type)
	{
		return std::any_of(std::begin(anyAxeBonusVec), std::end(anyAxeBonusVec), [&](const int val) {return type == val; });
	}
	static bool anyOfPickaxeBonus(int type)
	{
		return std::any_of(std::begin(anyPickaxeBonusVec), std::end(anyPickaxeBonusVec), [&](const int val) {return type == val; });
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
		return std::any_of(std::begin(anyOfAnimalsVec), std::end(anyOfAnimalsVec), [&](const int val) {return type == val; });
	}
	static bool anyOfPlants(int type)
	{
		return std::any_of(std::begin(anyOfPlantsVec), std::end(anyOfPlantsVec), [&](const int val) {return type == val; });
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
			return "Settings::GetObstacleString needs an update for Type: " + type;
			break;
		}
		return lang_unknown[lang];
	}
	static std::string GetItemString(int type)
	{
		switch (type)
		{
		case -1:
			return lang_noInformation[lang];
			break;
		case 0:
			return lang_ironAxe[lang];
			break;
		case 1:
			return lang_ironSword[lang];
			break;
		case 2:
			return lang_ironShield[lang];
			break;
		case 3:
			return lang_chainOfEase[lang];
			break;
		case 4:
			return lang_RingOfRejuvenation[lang];
			break;
		case 5:
			return lang_naturalWonder[lang];
			break;
		case 6:
			return lang_tulip[lang];
			break;
		case 7:
			return lang_rose[lang];
			break;
		case 8:
			return lang_lily[lang];
			break;
		case 9:
			return lang_ironArmor[lang];
			break;
		case 10:
			return lang_ironPickaxe[lang];
			break;
		case 11:
			return lang_bow[lang];
			break;
		case 12:
			return lang_sniper[lang];
			break;
		default:
			return "Settings::GetItemString needs an update for Type: "+type;
			break;
		}
	}
	class OPB//ObstacleProductivityBoost
	{
		float prodDelta=0.0f;
		std::vector<int> boostProdOfType = {};
		int prodBoostRange = 0;
		bool boostsProd = false;
	public:
		OPB() = default;
		OPB(float prodDelta, std::vector<int> boostProdOfType, int prodBoostRange)
			:
			prodDelta(prodDelta),
			boostProdOfType(boostProdOfType),
			prodBoostRange(prodBoostRange),
			boostsProd(true)
		{}
		bool BoostsProd()const { return boostsProd; }
		float GetProdDelta()const { return prodDelta; }
		const std::vector<int>& GetBoostedTypes()const { return boostProdOfType; }
		int GetProdBoostrange()const { return prodBoostRange; }

	};

	struct ObstacleStats
	{
		ObstacleStats(int nStates, std::vector<Vei2> size, int baseHp, int movesPerTurn,
			int attackRange, int attackDmg, int attacksPerTurn, int constructionTime,
			std::map<std::string, float> neededResToBuild,
			std::map<std::string, float> lootForDestroying,
			std::vector<Vei2> obstacleStartPos = { Vei2(0,0) },
			int healNumber = 0,
			int healRange = 0,
			float dmgAgainstPlants = 1.0f,
			float dmgAgainstWater = 1.0f,
			float dmgAgainstLand = 1.0f,
			float dmgAgainstAir = 1.0f,
			float dmgAgainstArmored = 1.0f,
			OPB opb = OPB())
			:
			nStates(nStates),
			size(size),
			movesPerTurn(movesPerTurn),
			attackRange(attackRange),
			attackDmg(attackDmg),
			attacksPerTurn(attacksPerTurn),
			constructionTime(constructionTime),
			neededResToBuild(neededResToBuild),
			lootForDestroying(lootForDestroying),
			obstacleStartPos(obstacleStartPos),
			healNumber(healNumber),
			healRange(healRange),
			dmgAgainstPlants(dmgAgainstPlants),
			dmgAgainstWater(dmgAgainstWater),
			dmgAgainstLand(dmgAgainstLand),
			dmgAgainstAir(dmgAgainstAir),
			dmgAgainstArmored(dmgAgainstArmored),
			opb(opb)
		{}

		int nStates = 1;
		int baseHp = 50;
		int movesPerTurn = 10;
		int attackRange = 1;
		int attackDmg = 10;
		int attacksPerTurn = 10;
		int constructionTime = 10;
		std::vector<Vei2> size = {};
		std::vector<Vei2> obstacleStartPos = { Vei2(0,0) };
		std::map<std::string, float> neededResToBuild = { {"leather",50.f},{"sticks",30.f} };
		std::map<std::string, float> lootForDestroying = { {"leather",25.f},{"sticks",15.f} };
		int healNumber = 10;
		int healRange = 10;
		float dmgAgainstPlants;
		float dmgAgainstWater;
		float dmgAgainstLand;
		float dmgAgainstAir;
		float dmgAgainstArmored;
		OPB opb;
	};
	static const ObstacleStats obstacleStats[] =
	{ 
		ObstacleStats(				//#0
		1,{Vei2(2,2)},100,-1,0,0,0,1,
		{{"leather",50.f},{"sticks",30.f}},
		{{"leather",25.f},{"sticks",15.f}},
		{Vei2(0,0)},
		0,0,1.0f,1.0f,1.0f,1.0f,1.0f,OPB()),
		ObstacleStats(				//#1
		2,{Vei2(1,1),Vei2(5,5)},100,-1,0,0,0,1,
		{{"sapling",1.f}},
		{{"wood",200.f},{"sticks",50.f},{"leafes",2.f},{"apples",5.f}},
		{Vei2(0,0),Vei2(-2,-2) }),
		ObstacleStats(				//#2
		1,{Vei2(1,1)},100,-1,0,0,0,1,
		{{"wood",10.f},{"sticks",30.f}},
		{{"wood",5.f},{"sticks",15.f}},{Vei2(0,0)},
			10,5),
		ObstacleStats(				//#3
		1,{Vei2(4,4)},100,-1,10,10,3,3,
		{{"stone",60.f},{"wood",100.f}},
		{{"stone",30.f},{"wood",15.f}},{Vei2(0,0)},
			20,10),
		ObstacleStats(				//#4
		2,{Vei2(1,1),Vei2(5,5)},100,-1,0,0,0,1,
		{{"sapling",1.f}},
		{{"wood",200.f},{"sticks",50.f},{"leafes",2.f},{"apples",5.f}},
		{Vei2(0,0),Vei2(-2,-2) }),
		ObstacleStats(				//#5
		1,{Vei2(1,1)},100,-1,0,0,0,1,
		{{"cactus",6.f}},
		{{"cactus",3.f}}),
		ObstacleStats(				//#6
		1,{Vei2(1,1)},100,-1,0,0,0,1,
		{},
		{{"iron",3.f},{"gold",0.1f}}),
		ObstacleStats(				//#7
		1,{Vei2(1,1)},100,-1,0,0,0,1,
		{{"stone",60.f}},
		{{"stone",20.f},{"iron",0.5f}}),
		ObstacleStats(				//#8
		1,{Vei2(2,2)},100,-1,0,0,0,1,
		{{"sapling",1.f}},
		{{"wood",150.f},{"sticks",30.f},{"leafes",2.f}}),
		ObstacleStats(				//#9
		1,{Vei2(4,4)},100,-1,0,0,0,3,
		{{"stone",80.f}},
		{{"stone",40.f},{"iron",2.f},{"copper",1.f}}),
		ObstacleStats(				//#10
		1,{Vei2(1,1)},100,10,1,10,1,0,
		{},
		{}),
		ObstacleStats(				//#11
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"leather",25.f},{"meat",20.f}}),
		ObstacleStats(				//#12
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"leather",25.f},{"meat",20.f}}),
		ObstacleStats(				//#13
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"leather",25.f},{"meat",20.f}}),
		ObstacleStats(				//#14
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"fur",25.f},{"meat",20.f}}),
		ObstacleStats(				//#15
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"wool",3.f},{"meat",10.f}}),
		ObstacleStats(				//#16
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"feather",1.f},{"meat",20.f}}),
		ObstacleStats(				//#17
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"meat",20.f}}),
		ObstacleStats(				//#18
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"feather",1.f},{"meat",20.f}}),
		ObstacleStats(				//#19
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"meat",20.f}}),
		ObstacleStats(				//#20
		1,{Vei2(1,1)},100,5,1,10,1,0,
		{},
		{{"meat",20.f}}),
		ObstacleStats(				//#21
		1,{Vei2(3,3)},100,-1,0,0,0,3,
		{{"snow",50.f},{"wood",20.f}},
		{{"snow",25.f},{"wood",10.f}}),
		ObstacleStats(				//#22
		1,{Vei2(3,3)},100,-1,0,0,0,4,
		{{"wood",700.f}},
		{{"wood",350.f}}),
		ObstacleStats(				//#23
		1,{Vei2(4,4)},100,-1,0,0,0,4,
		{{"stone",400.f},{"wood",200.f}},
		{{"stone",200.f},{"wood",100.f}}),
		ObstacleStats(				//#24
		1,{Vei2(4,4)},100,-1,0,0, 0,4,
		{{"bricks",200.f},{"wood",100.f},{"stone",100.f}},
		{{"bricks",100.f},{"wood",50.f},{"stone",50.f} }),
		ObstacleStats(				//#25
		1,{Vei2(4,4)},100,-1,0,0, 0,6,
		{{"concrete",500.f} },
		{{"concrete",250.f} }),
		ObstacleStats(				//#26
		1,{Vei2(5,5)},100,-1,0,0, 0,6,
		{{"slate",100.f},{"bricks",200.f},{"concrete",200.f} },
		{{"slate",50.f},{"bricks",100.f},{"concrete",100.f} }),
		ObstacleStats(				//#27
		1,{Vei2(4,4)},100,-1,15,5,3,3,
		{{"wood",100.f}},
		{{"wood",50.f}},
			{ Vei2(0,0) },
		0,0,8.f),
		ObstacleStats(				//#28
		1,{Vei2(3,3)},100,-1,0,0, 0,3,
		{{"sapling",50.f},{"wood",50.f},{"stone",50.f} },
		{{"sapling",25.f},{"wood",25.f},{"stone",25.f} }),
		ObstacleStats(				//#29
		1,{Vei2(4,4)},100,-1,0,0, 0,3,
		{{"wood",50.f},{"stone",30.f}},
		{ {"wood",25.f},{"stone",25.f} }),
		ObstacleStats(				//#30
		1,{Vei2(3,3)},100,-1,0,0, 0,3,
		{{"iron",10.f},{"stone",100.f},{"sticks",30.f}},
		{ {"iron",5.f},{"stone",50.f},{"sticks",15.f} }),
		ObstacleStats(				//#31
		1,{Vei2(5,5)},100,-1,0,0,0,6,
		{ {"iron",5.f},{"wood",200.f},{"stone",130.f}},
		{ {"iron",2.5f},{"wood",100.f},{"stone",65.f} },
		{ Vei2(0,0) }, 0, 0, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, OPB(1.0f, anyWindmillBoostedObstacles,20)),
		ObstacleStats(				//#32
		1,{Vei2(3,3)},100,-1,0,0, 0,3,
		{{"wood",100.f},{"feather",2.f}},
		{{"wood",50.f},{"feather",1.f}}),
		ObstacleStats(				//#33
		1,{Vei2(5,5)},100,-1,0,0, 0,6,
		{{"stone",100.f},{"wood",200.f},{"concrete",80.f} },
		{{"stone",50.f},{"wood",100.f},{"concrete",40.f} }),
		ObstacleStats(				//#34
		1,{Vei2(3,3)},100,-1,0,0, 0,3,
		{{"steel",50.f},{"wood",150.f}},
		{{"steel",25.f},{"wood",75.f}},
		{ Vei2(0,0) }, 0, 0, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, OPB(1.0f, anySawmillBoostedObstacles, 20)),
		ObstacleStats(				//#35
		1,{Vei2(5,5)},100,-1,0,0, 0,6,
		{{"steel",50.f},{"stone",150.f}},
		{{"steel",25.f},{"stone",75.f}}),
		ObstacleStats(				//#36
		1,{Vei2(5,5)},100,-1,0,0, 0,3,
		{{"steel",50.f},{"iron",30.f},{"stone",100.f} },
		{{"steel",25.f},{"iron",15.f},{"stone",50.f} }),
		ObstacleStats(				//#37
		1,{Vei2(4,4)},100,-1,0,0, 0,6,
		{{"steel",100.f},{"stone",100.f}},
		{{"steel",50.f},{"stone",75.f}}),
		ObstacleStats(				//#38
		1,{Vei2(5,5)},100,-1,0,0, 0,3,
		{{"steel",50.f},{"iron",30.f},{"stone",100.f} },
		{{"steel",25.f},{"iron",15.f},{"stone",50.f} }),
		ObstacleStats(				//#39
		1,{Vei2(4,4)},100,-1,0,0, 0,6,
		{{"steel",50.f},{"iron",30.f},{"stone",150.f} },
		{{"steel",25.f},{"iron",15.f},{"stone",75.f} }),
		ObstacleStats(				//#40
		1,{Vei2(3,3)},100,-1,0,0, 0,3,
		{{"silicon",50.f},{"plastic",30.f}, {"copper",3.f}},
		{{"silicon",25.f},{"plastic",15.f}, {"copper",1.5f} }),
		ObstacleStats(				//#41
		1,{Vei2(3,3)},100,-1,0,0, 0,3,
		{{"iron",50.f},{"plastic",30.f}, {"copper",3.f} },
		{{"iron",25.f},{"plastic",15.f}, {"copper",3.f} }),
		ObstacleStats(				//#42
		1,{Vei2(5,5)},100,-1,0,0, 0,6,
		{{"concrete",250.f},{"computer chips",30.f}},
		{{"concrete",125.f},{"computer chips",15.f}}),
		ObstacleStats(				//#43
		1,{Vei2(1,1)},100,-1,0,0, 0,0,
		{},
		{}),
		ObstacleStats(				//#44
		1,{Vei2(2,2)},100,-1,0,0, 0,0,
		{},
		{}),
		ObstacleStats(				//#45
		1,{Vei2(3,3)},100,-1,0,0, 0,0,
		{},
		{}),
		ObstacleStats(				//#46
		1,{Vei2(4,4)},100,-1,0,0, 0,0,
		{},
		{}),
		ObstacleStats(				//#47
		1,{Vei2(5,5)},100,-1,0,0, 0,0,
		{},
		{}),
		ObstacleStats(				//#48
		1,{Vei2(2,2)},100,-1,0,0, 0,3,
		{{"lithium",50.f},{"copper",30.f},{"concrete",30.f} },
		{{"lithium",25.f},{"copper",15.f},{"concrete",15.f} }),
		ObstacleStats(				//#49
		1,{Vei2(5,5)},100,-1,0,0, 0,6,
		{{"concrete",250.f},{"computer chips",30.f} },
		{{"concrete",125.f},{"computer chips",15.f} }),
		ObstacleStats(				//#50
		1,{Vei2(4,4)},100,-1,0,0, 0,3,
		{{"stone",200.f},{"wood",200.f}},
		{{"stone",100.f},{"wood",100.f}})
	};
	struct ItemStats
	{
		ItemStats(int durability, std::map<std::string, float> neededResToCraft, int turns2Craft, std::map<std::string, float> neededResToRepair, int durabilityHealPerRepair)
			:
			durability(durability),
			neededResToCraft(neededResToCraft),
			turns2Craft(turns2Craft),
			neededResToRepair(neededResToRepair),
			durabilityHealPerRepair(durabilityHealPerRepair)
		{}

		int durability;
		std::map<std::string, float> neededResToCraft = { {"leather",50.f},{"sticks",30.f} };
		int turns2Craft;
		std::map<std::string, float> neededResToRepair = { {"leather",50.f},{"sticks",30.f} };
		int durabilityHealPerRepair = 5;
	};
	static const ItemStats itemStats[] = {
		ItemStats(10,//#0
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(10,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(10,
			{ {"iron",2.f} },5,
			{ {"iron",0.5f} },
			5),
		ItemStats(10,
			{ {"iron",1.f} },5,
			{ {"iron",0.5f} },
			5),
		ItemStats(10,
			{ {"gold",1.f},{"emerald",1.f} },5,
			{ {"gold",0.25f} },
			5),
		ItemStats(-1,// #5
			{ {"sticks",1.f},{"sapling",1.f} },5,
			{ {"sticks",0.5f},{"sapling",0.25f} },
			5),
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(10,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(10,
			{ {"iron",3.f} },5,
			{ {"iron",1.f} },
			5),
		ItemStats(10,// #10
			{ {"iron",1.f} },5,
			{ {"iron",0.25f} },
			5),
		ItemStats(10,
			{ {"wood",1.f},{"silk",1.f} },5,
			{ {"wood",0.5f},{"silk",0.25f} },
			5),
		ItemStats(10,
			{ {"gunpowder",1.f},{"iron",2.f} },5,
			{ {"gunpowder",0.5f},{"iron",1.f} },
			5),
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),// #15
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5),
		ItemStats(-1,
			{ {"wood",1.f},{"iron",1.f} },5,
			{ {"wood",0.5f},{"iron",0.25f} },
			5)//#20
	};
};