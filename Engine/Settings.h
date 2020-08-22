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
	static constexpr int nDiffFieldTypes = 14;										//Array Nullterminator == -1
	static constexpr int typeLayer[] = { 6,0,8,4,3,9,1,5,7,2,13,12,10,11,-1};		//Orden in wich the connections are drawn
	static constexpr int hillTypesARE[] = { 11,10,12,-1 };
	static constexpr int liquidsTypesARE[] = { 0,6,8,-1 };
	static constexpr int groundedTypesARE[] = { 13,1,2,3,4,5,7,9,-1 };				//Types where you can place normal Buildings	
	static constexpr int CellSplitUpIn = 25;									//every cell has n*n supcells (ACCTUALLY HARDCODED!!! DONT EVEN TRY TO CHANGE SOMETHING !!! REALLY)
																				
	
	
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

	static int ArrSize(int* start)		// max size is 999
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
};