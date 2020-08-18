#pragma once
namespace Settings
{
	/*		##Types##
			0 = water				5 = snow				10 = mountains (high)			
			1 = dirt				6 = coral reef			11 = canjon	   (high)
			2 = ice					7 = Stone				12 = lavahills (high)
			3 = desert				8 = lava				13 = candyland
			4 = nutritious dirt		9 = savanne		
	
	*/
	static constexpr int nDiffFieldTypes = 14;
	static constexpr int typeLayer[] = { 6,0,4,3,9,1,5,7,8,2,13,12,10,11};		//Orden in wich the connections are drawn
	static constexpr int hillTypesARE[] = { 12,10,11 };
	static constexpr int typesGroundedARE[] = { 13,1,2,3,4,5,7,9 };	//Types where you can place normal Buildings	
	static constexpr int CellSplitUpIn = 25;									//every cell has n*n supcells (ACCTUALLY HARDCODED!!!)

	
	
	
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
		for (int i = 0; i < sizeof(typesGroundedARE) / sizeof(typesGroundedARE[0]); i++)
		{
			if (Type == typesGroundedARE[i])
			{
				return true;
			}
		}
		return false;
	}
};