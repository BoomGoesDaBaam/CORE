#include "Chunk.h"
void Chunk::PlaceLadderableTiles(int type)
{
	for (int y = 0; y < cells.GetColums() * Settings::CellSplitUpIn; y++)
	{
		for (int x = 0; x < cells.GetRows() * Settings::CellSplitUpIn; x++)
		{
			if (groundedMap[x][y] == 2)
			{
				if (GetGrounedMapAt(Vei2(x, y + 2)) == 0 || GetGrounedMapAt(Vei2(x, y + 2)) == 1)
				{
					groundedMap[x][y] = type;
				}
				if (GetGrounedMapAt(Vei2(x, y - 2)) == 0 || GetGrounedMapAt(Vei2(x, y - 2)) == 1)
				{
					groundedMap[x][y] = type;
				}
				if (GetGrounedMapAt(Vei2(x + 2, y)) == 0 || GetGrounedMapAt(Vei2(x + 2, y)) == 1)
				{
					groundedMap[x][y] = type;
				}
				if (GetGrounedMapAt(Vei2(x - 2, y)) == 0 || GetGrounedMapAt(Vei2(x - 2, y)) == 1)
				{
					groundedMap[x][y] = type;
				}
			}
		}
	}
}

void Chunk::ChangeGroundedVal(int from, int to)
{
	for (int y = 0; y < cells.GetColums() * Settings::CellSplitUpIn; y++)
	{
		for (int x = 0; x < cells.GetRows() * Settings::CellSplitUpIn; x++)
		{
			if (groundedMap[x][y] == from)
				groundedMap[x][y] = to;
		}
	}
}
void Chunk::PlaceTilesForMaskedField(Vei2 pos, int value, int valOfMixed, int valueOfZero, int type)
{
	SubAnimation sa = SubAnimation(resC->tC.maskedFields[Settings::translateIntoMaskedType(type)], RectI(Vei2(0, 0), 50, 50), RectI(Vei2(0, 0), 50, 50));

	Matrix<int> chromaM1 = sa.chromaM;
	chromaM1.Sort(value, valueOfZero);
	chromaM1.HalfSize(chromaM1, valOfMixed);
	chromaM1.MirrowVertical();
	//
	auto a = aMats(pos);
	std::vector<SubAnimation> sAVec;

	for (int i = 0; i < Settings::ArrSize(Settings::groundedTypesARE); i++)
	{
		int curType = Settings::groundedTypesARE[i];
		if (a.HasValue(curType))
		{
			if (conMap[curType][pos.x][pos.y] == 1)
			{
				std::vector<SubAnimation> newVecs;
				if (Settings::anyMaskedType(curType))
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, pos, true, a);
				}
				else
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, pos, false, a);
				}
				for (int c = 0; c < newVecs.size(); c++)
				{
					sAVec.push_back(newVecs[c]);
				}
			}
		}
	}
	Matrix<int> chromaM2 = SubAnimation::PutOnTopOfEachOther(sAVec, Vei2(50, 50), 1, 0);
	chromaM2.Sort(value, valueOfZero);
	chromaM2.HalfSize(chromaM2, valOfMixed);
	chromaM2.MirrowVertical();
	//
//	chromaM1.CombineWith(chromaM2);			//HIER
	SetTilesAT(pos, chromaM1);

}
void Chunk::PlaceConnectionsIntoCelltiles(Vei2 pos, int value, int valOfMixed, int valueOfZero, const int* types)
{
	auto a = aMats(pos);
	std::vector<SubAnimation> sAVec;

	for (int i = 0; i < Settings::ArrSize(types); i++)
	{
		int curType = types[i];
		if (a.HasValue(curType))
		{
			if (conMap[curType][pos.x][pos.y] == 1)
			{
				std::vector<SubAnimation> newVecs;
				if (Settings::anyMaskedType(curType))
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, pos, true, a);
				}
				else
				{
					newVecs = resC->fsC.GetConnectionAnimationVec(curType, pos, false, a);
				}
				for (int c = 0; c < newVecs.size(); c++)
				{
					sAVec.push_back(newVecs[c]);
				}
			}
		}
	}
	Matrix<int> chromaM = SubAnimation::PutOnTopOfEachOther(sAVec, Vei2(50, 50), 1, 0);
	chromaM.Sort(value, valueOfZero);
	chromaM.HalfSize(chromaM, valOfMixed);
	chromaM.MirrowVertical();
	SetTilesAT(pos, chromaM);
}
void Chunk::SetTilesAT(Vei2 pos, int value)
{
	for (int yInner = 0; yInner < Settings::CellSplitUpIn; yInner++)
	{
		for (int xInner = 0; xInner < Settings::CellSplitUpIn; xInner++)
		{
			groundedMap[(__int64)pos.x * Settings::CellSplitUpIn + xInner][(__int64)pos.y * Settings::CellSplitUpIn + yInner] = value;
		}
	}
}
void Chunk::SetTilesAT(Vei2 pos, Matrix<int> matrix)
{
	assert(matrix.GetColums() == Settings::CellSplitUpIn);
	assert(matrix.GetRows() == Settings::CellSplitUpIn);


	for (int yInCell = 0; yInCell < Settings::CellSplitUpIn; yInCell++)
	{
		for (int xInCell = 0; xInCell < Settings::CellSplitUpIn; xInCell++)
		{
			int curX = (__int64)pos.x * Settings::CellSplitUpIn + xInCell;
			int curY = (__int64)pos.y * Settings::CellSplitUpIn + yInCell;

			if (groundedMap[curX][curY] != 0)
			{
				if (matrix[xInCell][yInCell] != -1 && (groundedMap[curX][curY] == -1 || groundedMap[curX][curY] == 1))
				{
					groundedMap[curX][curY] = matrix[xInCell][yInCell];
				}
			}
		}
	}
}