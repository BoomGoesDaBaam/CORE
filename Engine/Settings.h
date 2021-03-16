#pragma once
#include "Rect.h"
#include <fstream>
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
			### Masked Array ###
			0 = swamp

			### Windows ###
			1 = Fieldinformaion

			### Frame Types ###		
			0 = frame with bar to open

			### Multiframe Types ###
			0 = empty

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

	static const int obstacleStates[] = { 1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	static const Vei2 obstacleSizes[] = { Vei2(2,2), Vei2(1,1), Vei2(1,1), Vei2(4,4), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(2,2), Vei2(4,4), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1), Vei2(1,1) };
	static const int obstacleBaseHP[] = { 20,20,20, 100, 20, 10, 5, 100, 20, 100, 50, 10,10,10,25,10,5,10,5,1,25 };
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

	const std::string lang_tent[] = { "tent", "Zelt" };
	const std::string lang_bonfire[] = { "bonfire", "Lagerfeuer" };
	const std::string lang_townhall[] = { "townhall", "Rathaus" };

	//Obstacles

	const std::string lang_tree[] = { "tree", "Baum" };
	const std::string lang_fir[] = { "fir", "Tanne" };
	const std::string lang_box[] = { "box", "Box" };
	const std::string lang_unit[] = { "unit", "Einheit" };
	const std::string lang_worker[] = { "worker", "Arbeiter" };

	const std::string lang_unitName[] = { "Unit name", "Name der Einheit" };
	const std::string lang_hp[] = { "Health points", "Lebenspunkte" };

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

	static constexpr float percentForGrab[] = { 0.05f, 0.05f };
	static bool anyOfUnit(int type)
	{
		if (type >= 10 && type <=20)
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
		default:
			return "Settings::GetObstacleString needs an update";
			break;
		}
		return lang_unknown[lang];
	}
};