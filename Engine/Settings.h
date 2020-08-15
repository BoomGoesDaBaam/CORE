#pragma once
namespace Settings
{

	static constexpr int nDiffFieldTypes = 14;
	static constexpr int typeLayer[] = { 6,0,4,3,1,5,7,8,2,12,9,10,11,13};	//Orden in wich the connections are drawn
	static constexpr int hillTypesARE[] = { 12,10,11 };
	static constexpr int typesToBuildOnARE[] = { 1,2,3,4,5,7,9,13 };	
	static constexpr int CellSplitUpIn = 25;								//every cell has n*n supcells

	
	
	
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
	static bool anyOfBuildOnTypes(int Type)
	{
		for (int i = 0; i < sizeof(typesToBuildOnARE) / sizeof(typesToBuildOnARE[0]); i++)
		{
			if (Type == typesToBuildOnARE[i])
			{
				return true;
			}
		}
		return false;
	}
};