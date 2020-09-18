#include "World.h"

World::World(WorldSettings wSettings, std::shared_ptr<ResourceCollection> resC, Vec2& camera) 
	:
	resC(std::move(resC)),
	c(camera),
	cells(wSettings.wSize,wSettings.defType),
	tC(&this->resC->tC),
	fsC(&this->resC->fsC)
{
	Init(wSettings);
	Generate(wSettings);
}
//conMap(Matrix<int>(wSize.x, wSize.y, 0))
bool World::IsInWorld(Vei2& v)const
{
	return v.x >= 0 && v.x < wSize.x && v.y >= 0 && v.y < wSize.y;
}
bool World::IsInWorldY(int y)const
{
	return y >= 0 && y < wSize.y;
}
Vei2 World::PutInWorldX(Vei2 v)const
{

	if (v.x < 0)
	{
		v.x = -v.x;
		v.x %= wSize.x;
		v.x = wSize.x - v.x;
	}
	if (v.x >= wSize.x)
	{
		v.x = v.x % wSize.x;
	}

	if (v.y < 0)
	{
		v.y = 0;
	}
	if (v.y >= wSize.y)
	{
		v.y = wSize.y - 1;
	}
	return v;
}
Matrix<int> World::GetAroundMatrix(Vei2 cell) const
{
	Matrix<int> m = Matrix<int>(3, 3, 0);

	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			Vei2 curCell = PutInWorldX(Vei2(cell.x+x-1,cell.y+y-1));

			if (!IsInWorldY(y))
			{
				m[x][y] = -1;
			}
			else
			{
				m[x][y] = cells(curCell).type;
			}
		}
	}
	return m;
}
void World::UpdateConMap()
{
	conMap.clear();
	for (int type = 0; type < Settings::nDiffFieldTypes;type++)
	{
		conMap.push_back(Matrix<int>(wSize.x, wSize.y, 0));
		for (int y = 0; y < wSize.y; y++)
		{
			for (int x = 0; x < wSize.x; x++)
			{
				if (type != cells(Vei2(x, y)).type && IsSurroundedBy(Vei2(x, y), type))
				{
					conMap[type][x][y] = 1;
				}
				else
				{
					conMap[type][x][y] = 0;
				}
			}
		}
	}
}
void World::UpdateGroundedMap(Vei2 pos, Vei2 size)
{
	assert(pos.y >= 0 && pos.y < wSize.y);

	using namespace Settings;
	if (size == Vei2(-1, -1) || size == wSize)
	{
		size = wSize;
		groundedMap = Matrix<int>(wSize.x * CellSplitUpIn, wSize.y * CellSplitUpIn, -1);
	}

	for (int y = pos.y; y < pos.y+size.y; y++)
	{
		for (int x = pos.x; x < pos.x+size.x; x++)
		{
			Vei2 pos = PutInWorldX(Vei2(x, y));
			assert(IsInWorld(pos));
			SetTilesAT(pos, -1);

			int curType = cells[pos.x][pos.y].type;
			if (Settings::anyGroundedTypes(curType))				//grounded
			{
				SetTilesAT(pos, 1);
				PlaceConnectionsIntoCelltiles(pos, 2, 2, -1, hillTypesARE);
			}
			else if(Settings::anyOfHillTypes(curType))	//hills
			{
				SetTilesAT(pos, 2);
			}
			else if (Settings::anyLiquidType(curType))	//liqids
			{
				PlaceConnectionsIntoCelltiles(pos, 2,2,-1, hillTypesARE);

				PlaceConnectionsIntoCelltiles(pos, 1,-1,-1, groundedTypesARE);
				PlaceConnectionsIntoCelltiles(pos, 1, -1, -1, maskTypesARE);
			}
			else if (Settings::anyMaskedType(curType))	//swamp
			{
				PlaceTilesForMaskedField(pos,1, -1, -1, 14);

				PlaceConnectionsIntoCelltiles(pos, 2, 2, -1, hillTypesARE);
				PlaceConnectionsIntoCelltiles(pos, 1, -1, -1, groundedTypesARE);
			}
		}
	}
	ChangeGroundedVal(-1, 0);
	PlaceLadderableTiles(3);
}

void World::PlaceLadderableTiles(int type)
{
	for (int y = 0; y < wSize.y * Settings::CellSplitUpIn; y++)
	{
		for (int x = 0; x < wSize.x * Settings::CellSplitUpIn; x++)
		{
			if (groundedMap[x][y] == 2)
			{
				if (y > 1 && (groundedMap[x][(__int64)y + 2] == 0 || groundedMap[x][(__int64)y + 2] == 1))
				{
					groundedMap[x][y] = type;
				}
				if (y < wSize.y * Settings::CellSplitUpIn - 2 && (groundedMap[x][(__int64)y - 2] == 0 || groundedMap[x][(__int64)y - 2] == 1))
				{
					groundedMap[x][y] = type;
				}
				if (groundedMap(PutTileIntoWorld((__int64)x + 2,y)) == 0 || groundedMap(PutTileIntoWorld((__int64)x + 2, y)) == 1)
				{
					groundedMap[x][y] = type;
				}
				if (groundedMap(PutTileIntoWorld((__int64)x - 2, y)) == 0 || groundedMap(PutTileIntoWorld((__int64)x - 2, y)) == 1)
				{
					groundedMap[x][y] = type;
				}
			}
		}
	}
}
Vei2 World::PutTileIntoWorld(Vei2 pos)
{
	return PutTileIntoWorld(pos.x, pos.y);
}
Vei2 World::PutTileIntoWorld(int x, int y)
{
	if (y < 0)
	{
		y = 0;
	}
	if (y >= wSize.y * Settings::CellSplitUpIn)
	{
		y = wSize.y * Settings::CellSplitUpIn - 1;
	}
	while (x < 0)
	{
		x += wSize.x * Settings::CellSplitUpIn;
	}
	while (x >= wSize.x * Settings::CellSplitUpIn)
	{
		x -= wSize.x * Settings::CellSplitUpIn;
	}
	return Vei2(x, y);
}
void World::ChangeGroundedVal(int from, int to)
{
	for (int y = 0; y < wSize.y * Settings::CellSplitUpIn; y++)
	{
		for (int x = 0; x < wSize.x * Settings::CellSplitUpIn; x++)
		{
			if (groundedMap[x][y] == from)
				groundedMap[x][y] = to;
		}
	}
}
void World::PlaceTilesForMaskedField(Vei2 pos, int value, int valOfMixed, int valueOfZero, int type)
{
	SubAnimation sa = SubAnimation(resC->tC.maskedFields[Settings::translateIntoMaskedType(type)],RectI(Vei2(0,0),50,50), RectI(Vei2(0, 0), 50, 50));

	Matrix<int> chromaM = sa.chromaM;
	chromaM.Sort(value, valueOfZero);
	chromaM.HalfSize(chromaM, valOfMixed);
	chromaM.MirrowVertical();
	SetTilesAT(pos, chromaM);

}
void World::PlaceConnectionsIntoCelltiles(Vei2 pos, int value, int valOfMixed, int valueOfZero, const int* types)
{
	auto a = GetAroundMatrix(pos);
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
std::vector<SubAnimation> World::GetConnectionsOfTypes(Vei2 pos, int* types)
{
	std::vector<SubAnimation> cons;
	for (int i = 0; i < Settings::ArrSize(types); i++)
	{
		std::vector<SubAnimation> newC = resC->fsC.GetConnectionAnimationVec(types[i], pos, false, GetAroundMatrix(pos));
		for (int n = 0; n < newC.size(); n++)
		{
			cons.push_back(newC[n]);
		}
	}
	return cons;
}
void World::SetTilesAT(Vei2 pos, int value)
{
	for (int yInner = 0; yInner < Settings::CellSplitUpIn; yInner++)
	{
		for (int xInner = 0; xInner < Settings::CellSplitUpIn; xInner++)
		{
			groundedMap[(__int64)pos.x * Settings::CellSplitUpIn + xInner][(__int64)pos.y * Settings::CellSplitUpIn + yInner] = value;
		}
	}
}
void World::SetTilesAT(Vei2 pos, Matrix<int> matrix)
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
bool World::IsSurroundedBy(Vei2 pos, int type)
{
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			if (cells(PutInWorldX(pos+Vei2(x-1,y-1))).type == type)
			{
				return true;
			}
		}
	}
	return false;
}
RectF World::GetCellRect(Vei2 cellP)const
{
	Vei2 d = cellP - mCell;

	if (d.x > wSize.x / 2)
	{
		d.x -= wSize.x;
	}
	if (d.x < -wSize.x / 2)
	{
		d.x += wSize.x;
	}

	if (d.y > wSize.y / 2)
	{
		d.y -= wSize.y;
	}
	if (d.y < -wSize.y / 2)
	{
		d.y += wSize.y;
	}

	Vei2 mos = Graphics::GetMidOfScreen();

	mos.x += d.x * cSize.x;
	mos.y -= d.y * cSize.y;
	return RectF(Vec2(mos), (float)cSize.x, (float)cSize.y) + Vec2(-(int)c.x, +(int)c.y);
}
RectF World::GetTileRect(Vei2 tileP)const
{
	using namespace Settings;

	Vei2 cellHit = Vei2(tileP.x / CellSplitUpIn, tileP.y / CellSplitUpIn);
	RectF cellPos = GetCellRect(cellHit);
	Vec2 tileSize = GetTileSize();

	cellPos.left += (tileP.x % CellSplitUpIn) * tileSize.x;
	cellPos.right = cellPos.left + tileSize.x;
	cellPos.bottom -= (tileP.y % CellSplitUpIn) * tileSize.y;
	cellPos.top = cellPos.bottom - tileSize.y;

	return cellPos;
}
Vei2 World::GetCellHit(Vec2 mP)const
{
	Vec2 mos = (Vec2) Graphics::GetMidOfScreen();
	mos.x -= c.x;
	mos.y += c.y;

	Vei2 deltaPixel = (Vei2) mos - (Vei2) mP;
	Vei2 deltaCells = { 0,0 };

	deltaCells.y = (deltaPixel.y / cSize.y);
	if (deltaPixel.y < 0)
		deltaCells.y--;
	deltaCells.x = -(deltaPixel.x / cSize.x) - 1;
	if (deltaPixel.x < 0)
		deltaCells.x++;

	return PutInWorldX(deltaCells + mCell);
}
Vei2 World::GetTileHit(Vec2 mP)const
{
	Vec2 mos = (Vec2)Graphics::GetMidOfScreen();

	Vei2 cell = GetCellHit(mP) ;
	RectF cellRect = GetCellRect(cell);
	Vei2 delta = Vei2(mP.x - cellRect.left,cellRect.bottom - mP.y);
	//assert(delta.x > 0 && delta.y > 0);
	Vec2 tileSize = GetTileSize();
	Vei2 hitTile = Vei2(cell.x * Settings::CellSplitUpIn + delta.x / tileSize.x, (cell.y - 1) * Settings::CellSplitUpIn + delta.y / tileSize.y);

	return hitTile;
}
void World::Zoom(Vei2 delta)
{
	float deltaX = c.x / cSize.x;
	float deltaY = c.y / cSize.y;
	if (delta.x + cSize.x > 0 && delta.x + cSize.x <= 600)
	{
		cSize.x += delta.x;
		c.x = cSize.x * deltaX;
	}
	if (delta.y + cSize.y > 0 && delta.y + cSize.y <= 600)
	{
		cSize.y += delta.y;
		c.y += delta.y / 2;
		c.y = cSize.y * deltaY;
	}
	fsC->Update(cSize);
}

void World::ApplyCameraChanges(Vec2 cDelta)
{
	c.x += -cDelta.x;
	c.y += cDelta.y;
		
	if (c.x < 0)						//move Camera
	{
		mCell.x--;
		c.x += GetcSize().x;
	}
	if (c.x > cSize.x)
	{
		mCell.x++;
		c.x -= GetcSize().x;
	}
	if (c.y < 0)
	{
		mCell.y--;
		c.y += GetcSize().y;
	}
	if (c.y > cSize.y)
	{
		mCell.y++;
		c.y -= GetcSize().y;
	}
	
	if (mCell.x < 0)
	{
		mCell.x += wSize.x;
	}
	if (mCell.x > wSize.x - 1)
	{
		mCell.x -= wSize.x;
	}
	Vec2 mos = (Vec2) Graphics::GetMidOfScreen();		//Move mCell
	if (mCell.y < mos.y / cSize.y)
	{
		mCell.y =(int) mos.y / cSize.y;
		c.y = 0;
	}
	if (mCell.y > wSize.y - (mos.y / cSize.y) - 1)
	{
		mCell.y = (int)(wSize.y - (mos.y / cSize.y) - 1);
		c.y = cSize.y-0.001f;
	}


}
void World::HandleMouseEvents(Mouse::Event& e, GrabHandle& gH)
{
	Vec2 mP = (Vec2)e.GetPos();
	if (e.GetType() == Mouse::Event::LRelease && !gH.IsLocked())
	{
		fCell = GetCellHit(mP);
		fTile = GetTileHit(mP);
		/*
		cells(fCell).type = 14;
		UpdateConMap();
		UpdateGroundedMap(Vei2(fCell.x-1,fCell.y-1),Vei2(3,3));
		*/
	}
	if (e.GetType() == Mouse::Event::WheelDown)
	{
		Zoom(Vei2(-25, -25));
	}
	if (e.GetType() == Mouse::Event::WheelUp)
	{
		Zoom(Vei2(25, 25));
	}
	Vec2 cDelta = gH.MoveCamera(e);
	ApplyCameraChanges(cDelta);
}
void World::HandleKeyboardEvents(Keyboard::Event& e)
{
	if (e.IsRelease())
	{
		switch (e.GetCode())
		{
		case 'G':
			grit = !grit;
			break;
		case 'B':
			buildMode = !buildMode;
			grit = !grit;
			break;
		case 0x1B:	//ERROR?
			fCell = Vei2(0, 0);
			break;
		}
	}
}
void World::Draw(Graphics& gfx) const
{
	/*			3x3
	for (int y = 0; y <= 2; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
	*/
	Vei2 mos = Graphics::GetMidOfScreen();

	int xStart = -(mos.x / cSize.x) *2 - 1;
	int xStop = 1 + (mos.x / cSize.x) * 2;
	int yStart = -(mos.y / cSize.y) * 2;
	int yStop = 2 + (mos.y / cSize.y) * 2;

	#ifdef _DEBUG 
	xStart = -1;
	xStop = 1;
	yStart = 0;
	yStop = 2;
	#endif

	for (int layer = 0; layer <= 2; layer++)
	{
		for (int y = yStart; y <= yStop; y++)
		{
			for (int x = xStart; x <= xStop; x++)
			{
				Vei2 curXY = PutInWorldX(mCell + Vei2(x, y) + Vei2(0, -1));
				const Cell& curCell = cells(curXY);
				int cellType = curCell.type;
				RectI curCellPos = (RectI)GetCellRect(Vei2(x, y) + mCell);

				switch (layer)
				{
				case 0:
					assert(cellType >= 0 && cellType < Settings::nDiffFieldTypes);
					gfx.DrawSurface(curCellPos, RectI(Vei2(0, 0), 50, 50), tC->Fields.at(cellType).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
					
					for (int i = 0; i < Settings::nDiffFieldTypes; i++)
					{
						int order = Settings::typeLayer[i];
						if (conMap[order][curXY.x][curXY.y] == 1)
						{
							gfx.DrawConnections(order, Vei2(curCellPos.left, curCellPos.top), GetAroundMatrix(curXY), fsC->FieldCon, tC->Fields[order].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						}
					}
					break;
				case 1:
					if (grit)
					{
						using namespace Settings;
						SpriteEffect::Transparent e = SpriteEffect::Transparent(0.6f);

						for (int xOnCell = 0; xOnCell < CellSplitUpIn; xOnCell++)
						{
							for (int yOnCell = 0; yOnCell < CellSplitUpIn; yOnCell++)
							{
								Vei2 v = (curXY * CellSplitUpIn + Vei2(xOnCell, yOnCell));
								assert(v.x < 5000 && v.x >= 0);
								assert(v.y < 5000 && v.y >= 0);

								
									float xPos = curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth();
									float yPos = curCellPos.bottom - ((float)(yOnCell+1) / CellSplitUpIn) * curCellPos.GetHeight();
									RectF curP = RectF(Vec2(xPos, yPos), (float)std::ceil((double)curCellPos.GetWidth() / CellSplitUpIn), (float)std::ceil((double)curCellPos.GetHeight() / CellSplitUpIn));
									if (Graphics::GetScreenRect<float>().IsOverlappingWith(curP))
									{
										curP.PutInto(Graphics::GetScreenRect<float>());
										if (groundedMap(v) == 0)
										{
											gfx.DrawRect(curP, Colors::Red, e);
										}
										else if (groundedMap(v) == 1)
										{
											//gfx.DrawRect(curP, Colors::Green, e);
										}
										else if (groundedMap(v) == -1)
										{
											gfx.DrawRect(curP, Colors::Magenta, e);
										}
										else if (groundedMap(v) == 2)
										{
											gfx.DrawRect(curP, Colors::Blue, e);
										}
										else if (groundedMap(v) == 3)
										{
											gfx.DrawRect(curP, Colors::Cyan, SpriteEffect::Transparent(0.5f));
										}
										if (v == fTile)
										{
											gfx.DrawRect(curP, Colors::Black, SpriteEffect::Transparent(0.5f));
										}
									}
								
							}
						}

						for (int xOnCell = 0; xOnCell < Settings::CellSplitUpIn + 1; xOnCell++)
						{
							if (xOnCell == 0 || xOnCell == CellSplitUpIn)
							{
								gfx.DrawLine(Vec2((float)curCellPos.left, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), Vec2((float)curCellPos.right, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black), 3);
								gfx.DrawLine(Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.top), Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.bottom), SpriteEffect::OneColor(Colors::Black), 3);
							}
							else
							{
								gfx.DrawLine(Vec2((float)curCellPos.left, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), Vec2((float)curCellPos.right, (float)curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black));
								gfx.DrawLine(Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.top), Vec2((float)curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), (float)curCellPos.bottom), SpriteEffect::OneColor(Colors::Black));
							}
							/*
							if (xOnCell == 0 || xOnCell == CellSplitUpIn)
							{
								if (grit == 1)
								{
									gfx.DrawLine(Vec2(curCellPos.left, curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), Vec2(curCellPos.right, curCellPos.top + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black), 3);
									gfx.DrawLine(Vec2(curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), curCellPos.top), Vec2(curCellPos.left + ((float)xOnCell / CellSplitUpIn) * curCellPos.GetWidth(), curCellPos.bottom), SpriteEffect::OneColor(Colors::Black), 3);
								}
								else if (grit == 2)
								{
									gfx.DrawLine(Vec2(curCellPos.left, curCellPos.top + ((float)xOnCell / CellSplitUpIn * 2) * curCellPos.GetHeight()), Vec2(curCellPos.right, curCellPos.top + ((float)xOnCell / CellSplitUpIn * 2) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black), 3);
									gfx.DrawLine(Vec2(curCellPos.left + ((float)xOnCell / CellSplitUpIn*2) * curCellPos.GetWidth(), curCellPos.top), Vec2(curCellPos.left + ((float)xOnCell / CellSplitUpIn * 2) * curCellPos.GetWidth(), curCellPos.bottom), SpriteEffect::OneColor(Colors::Black), 3);
								}
							}
							else
							{
								if (grit == 1)
								{
									gfx.DrawLine(Vec2(curCellPos.left, curCellPos.top + ((float)xOnCell / CellSplitUpIn ) * curCellPos.GetHeight()), Vec2(curCellPos.right, curCellPos.top + ((float)xOnCell / CellSplitUpIn ) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black));
									gfx.DrawLine(Vec2(curCellPos.left + ((float)xOnCell / CellSplitUpIn ) * curCellPos.GetWidth(), curCellPos.top), Vec2(curCellPos.left + ((float)xOnCell / CellSplitUpIn ) * curCellPos.GetWidth(), curCellPos.bottom), SpriteEffect::OneColor(Colors::Black));
								}
								else if (grit == 2)
								{
									gfx.DrawLine(Vec2(curCellPos.left, curCellPos.top + ((float)xOnCell / CellSplitUpIn * 2) * curCellPos.GetHeight()), Vec2(curCellPos.right, curCellPos.top + ((float)xOnCell / CellSplitUpIn * 2) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black));
									gfx.DrawLine(Vec2(curCellPos.left + ((float)xOnCell / CellSplitUpIn * 2) * curCellPos.GetWidth(), curCellPos.top), Vec2(curCellPos.left + ((float)xOnCell / CellSplitUpIn * 2) * curCellPos.GetWidth(), curCellPos.bottom), SpriteEffect::OneColor(Colors::Black));
								}
							}
							*/
						}
					}
					if (buildMode)
					{
						//gfx.DrawRect(GetTileRect(fTile), Colors::Black, SpriteEffect::Rainbow());
					}
					break;
				case 2:
					if (curXY == fCell)
					{
						gfx.DrawSurface(curCellPos.GetExpanded(cSize.x / 5), tC->Frames.at(0).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
					}
					break;
				}
				
			}
		}
	}
}
bool World::NeedsConnections(Vei2 curXY)const
{
	if (cells.InWorldAT(curXY).type == 1)
	{
		return false;
	}
	return true;
}
bool World::FIDF(int first, int second)const
{
	if (first != second)
	{
		for (int i = 0; i < Settings::nDiffFieldTypes; i++)
		{
			if (Settings::typeLayer[i] == first || Settings::typeLayer[i] == second)
			{
				return Settings::typeLayer[i] == first;
			}
		}
	}
	return false;
}
void World::CutHills(int replaceTo)
{
	using namespace Settings;
	for (int y = 0; y < wSize.y; y++)
	{
		for (int x = 0; x < wSize.x; x++)
		{
			int arrSize = sizeof(hillTypesARE) / sizeof(hillTypesARE[0]);
			bool test = false;
			for (int i = 0; i < arrSize; i++)
			{
				if (cells[x][y].type == hillTypesARE[i])
				{
					test = true;
				}
			}
			if (test)
			{
				for (int i = 0; i < arrSize; i++)
				{
					auto aMat = GetAroundMatrix(Vei2(x, y));
					if (hillTypesARE[i] == aMat[1][1])
					{
						continue;
					}
					Vei2 problem = aMat.GetPosOfValue(hillTypesARE[i])[0];
					if (problem != Vei2(-1,-1))
					{
						Vei2 where = problem + Vei2(x, y) + Vei2(-1,-1);
						cells(where).type = replaceTo;
						i=0;
					}
				}
			}
		}
	}
}

void World::Init(WorldSettings& s)
{
	wSize = s.wSize;
	cSize = s.cSize;
	mCell = wSize / 2;
	fCell = wSize / 2;
	mCell.x = 0;
	for (int type = 1; type < tC->Fields.size(); type++)		//Create connectionsmaps
	{
		conMap.push_back(Matrix<int>(wSize.x, wSize.y, 0));
	}
	Zoom(Vei2(0,0));
}

void World::Generate(WorldSettings& s)
{
	cells.Init(s.wSize,s.defType);						//Welt erstellen & default value setzen
	
	
	switch (s.defBlueprint)
	{
	case 0:
		int arcticSize = (wSize.y / 40);
		int subArcticSize = (wSize.y / 7.5f);

		for (int i = 0; i < wSize.y; i++)					//north & south ice
		{
			GenerateExplosion(Vei2(i, 0), rng.GetNormalDist() * arcticSize, 2);
			GenerateExplosion(Vei2(i, wSize.y - 1), rng.GetNormalDist() * arcticSize, 2);
		}
		for (int i = 0; i < (wSize.x * wSize.y) / 50; i++)	//plants
		{
			GenerateExplosion(Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20)), rng.GetNormalDist() * 3 + 5, 1);
		}
		for (int i = 0; i < wSize.x * 5; i++)	//snow
		{
			GenerateExplosion(Vei2(rng.Calc(wSize.x), rng.GetNormalDist() * subArcticSize), rng.GetNormalDist() * 3, 5, -1);
			GenerateExplosion(Vei2(rng.Calc(wSize.x), wSize.y - rng.GetNormalDist() * subArcticSize), rng.GetNormalDist() * 3, 5, -1);
		}
		for (int i = 0; i < (wSize.x * wSize.y) / 200; i++)	//coral reef
		{
			GenerateExplosion(Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20)), rng.GetNormalDist() * 3 + 3,6,0);
		}
		for (int i = 0; i < (wSize.x * wSize.y) / 6400; i++)	//candyland
		{
			GenerateExplosion(Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20)), 3, 13, 1);
		}
		for (int i = 0; i < (wSize.x * wSize.y) / 1600; i++)	//desert
		{
			Vei2 pos = Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20));

			GenerateCircle(pos, 7, 3, 1);
			GenerateExplosion(pos, rng.GetNormalDist() * 6 + 10, 3,1);
		}
		for (int i = 0; i < (wSize.x * wSize.y) / 3200; i++)	//rocks
		{
			GenerateExplosion(Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20)), rng.GetNormalDist() * 6 + 5, 10, -1);
		}
		for (int i = 0; i < (wSize.x * wSize.y) / 3200; i++)	//canjon + savanne
		{
			Vei2 pos = Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20));
			GenerateExplosion(pos, 6 + rng.GetNormalDist() * 6, 9, 1);
			GenerateExplosion(pos, 4, 11,9);
		}
		for (int i = 0; i < (wSize.x * wSize.y) / 6400; i++)	//magma
		{
			Vei2 pos = Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20));
			GenerateExplosion(pos,6, 12, -1);
			GenerateExplosion(pos, 4,8,12);
		}
		for (int i = 0; i < (wSize.x * wSize.y) / 10; i++)	//nutritious plants
		{
			GenerateExplosion(Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20)), rng.GetNormalDist() * 3, 4, 1,20,0);
		}
		for (int i = 0; i < (wSize.x * wSize.y) / 10; i++)	//nutritious plants
		{
			GenerateExplosion(Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20)), rng.GetNormalDist() * 3, 14, 1, 20, 0);
		}
		for (int i = 0; i < (wSize.x * wSize.y); i++)	//nutritious plants
		{
			GenerateExplosion(Vei2(rng.Calc(wSize.y), 10 + rng.Calc(wSize.y - 20)), rng.GetNormalDist() * 3, 14, 0, 20, 14);
		}
		break;
	}

	//CutHills(1);		//NICHT ZUENDE!!!!

	UpdateConMap();
	UpdateGroundedMap();
}
void World::GenerateCircle(Vei2 pos, int radius,int type, int ontoType, int surrBy)
{
	for (int y = -radius; y < radius; y++)
	{
		for (int x = -radius; x < radius; x++)
		{
			if (sqrt(y * y + x * x) <= radius)
			{
				Vei2 putPos = PutInWorldX(pos + Vei2(x, y));
				if (IsInWorld(putPos))
				{
					Cell& curCell = cells(PutInWorldX(putPos));
					auto aMat = GetAroundMatrix(putPos);
					
					if ((curCell.type == ontoType || -1 == ontoType) && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
					{
						curCell.type = type;
					}
				}
			}
		}
	}
}
void World::GenerateLine(Vec2 p0, Vec2 p1, int type, int ontoType, int thickness, int surrBy) //bnot
{
	if (p0 != p1) {
		float m = 0.0f;
		if (p1.x != p0.x)
		{
			m = ((float)p1.y - p0.y) / ((float)p1.x - p0.x);
		}
		if (thickness > 1)
		{
			for (int i = -thickness / 2; i < thickness / 2; i++)
			{
				if (std::abs(m) <= 1.0f)
				{
					GenerateLine(Vec2(p1.x, p1.y + i), Vec2(p0.x, p0.y + i), type, ontoType,1, surrBy);
				}
				else
				{
					GenerateLine(Vec2(p1.x + i, p1.y), Vec2(p0.x + i, p0.y), type, ontoType,1, surrBy);
				}
			}
		}
		//	######
		if (p1.x != p0.x && std::abs(m) <= 1.0f)
		{
			if (p0.x > p1.x)
			{
				std::swap(p0, p1);
			}

			const float b = p0.y - m * p0.x;

			for (int x = (int)p0.x; x < (int)p1.x; x++)
			{
				const float y = m * (float)x + b;

				const int yi = (int)y;
				Cell& curCell = cells(PutInWorldX(Vei2(x, yi)));
				auto aMat = GetAroundMatrix(Vei2(x, yi));

				if ((curCell.type == ontoType || -1 == ontoType) && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
				{
					curCell.type = type;
				}
			}
		}
		else
		{
			if (p0.y > p1.y)
			{
				std::swap(p0, p1);
			}

			const float w = (p1.x - p0.x) / (p1.y - p0.y);
			const float p = p0.x - w * p0.y;

			for (int y = (int)p0.y; y < (int)p1.y; y++)
			{
				const float x = w * (float)y + p;

				const int xi = (int)x;
				Cell& curCell = cells(PutInWorldX(Vei2(xi, y)));
				
				auto aMat = GetAroundMatrix(Vei2(xi, y));

				if ((curCell.type == ontoType || -1 == ontoType) && (aMat.HasValue(surrBy) || surrBy == -1) && aMat[1][1] != surrBy)
				{
					curCell.type = type;
				}
			}
		}
	}
}
void World::GenerateExplosion(Vei2 pos, int maxLineLength, int type,int ontoType, int nRolls, int surrBy)//not
{
	for (int i = 0; i < nRolls; i++)
	{
		float rad = (float)rng.Calc(360) * 0.0174533f;
		Vec2 p1 = (Vec2) GigaMath::RotPointToOrigin<float>(1.0f,0.0f, rad);
		Vei2 scaled = pos + Vei2(p1 * (maxLineLength*1/2+ rng.Calc(maxLineLength*1/2)));
		GenerateLine(Vec2(pos), Vec2(scaled), type, ontoType,1, surrBy);
	}
}