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
	for (int type = 1; type < Settings::nDiffFieldTypes;type++)
	{
		conMap.push_back(Matrix<int>(wSize.x, wSize.y, 0));
		for (int y = 0; y < wSize.y; y++)
		{
			for (int x = 0; x < wSize.y; x++)
			{
				if (type != cells(Vei2(x, y)).type && IsSurroundedBy(Vei2(x, y), type))
				{
					conMap[(__int64)type-1][x][y] = 1;
				}
				else
				{
					conMap[(__int64)type-1][x][y] = 0;
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

	Vei2 mos = Graphics::GetMidOfScreen();

	mos.x += d.x * cSize.x;
	mos.y -= d.y * cSize.y;
	return RectF(Vec2(mos), cSize.x, cSize.y);
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
void World::Zoom(Vei2 delta)
{
	float deltaX = c.x / cSize.x;
	float deltaY = c.y / cSize.y;
	if (delta.x + cSize.x > 0 && delta.x + cSize.x <= 400)
	{
		cSize.x += delta.x;
		c.x = cSize.x * deltaX;
	}
	if (delta.y + cSize.y > 0 && delta.y + cSize.y <= 400)
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
		mCell.y = mos.y / cSize.y;
		c.y = 0;
	}
	if (mCell.y > wSize.y - 4)
	{
		mCell.y = wSize.y - 4;
		c.y = cSize.y-0.001f;
	}
}
void World::HandleMouseEvents(Mouse::Event& e, GrabHandle& gH)
{

	if (e.GetType() == Mouse::Event::LRelease && !gH.IsLooked())
	{
		fCell = GetCellHit((Vec2) e.GetPos());
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
			gritVisible = !gritVisible;
			break;
		case 0x1B:	//ERROR?
			fCell = Vei2(-1, -1);
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

	int xStart = -(mos.x / cSize.x) * 4;
	int xStop = 1 + (mos.x / cSize.x) * 4;
	int yStart = -(mos.y / cSize.y) * 4;
	int yStop = 2 + (mos.y / cSize.y) * 4;

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
				RectI curCellPos = (RectI)GetCellRect(Vei2(x, y) + mCell) + Vei2(-c.x, +c.y);
				if(gritVisible)
				for (int i = 0; i <= 25; i++)
				{
					if (i == 0 || i == 25)
					{
						gfx.DrawLine(Vec2(curCellPos.left, curCellPos.top + ((float)i / 25) * curCellPos.GetHeight()), Vec2(curCellPos.right, curCellPos.top + ((float)i / 25) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black),3);
						gfx.DrawLine(Vec2(curCellPos.left + ((float)i / 25) * curCellPos.GetWidth(), curCellPos.top), Vec2(curCellPos.left + ((float)i / 25) * curCellPos.GetWidth(), curCellPos.bottom), SpriteEffect::OneColor(Colors::Black), 3);
					}
					else 
					{
						gfx.DrawLine(Vec2(curCellPos.left, curCellPos.top + ((float)i / 25) * curCellPos.GetHeight()), Vec2(curCellPos.right, curCellPos.top + ((float)i / 25) * curCellPos.GetHeight()), SpriteEffect::OneColor(Colors::Black));
						gfx.DrawLine(Vec2(curCellPos.left + ((float)i / 25) * curCellPos.GetWidth(), curCellPos.top), Vec2(curCellPos.left + ((float)i / 25) * curCellPos.GetWidth(), curCellPos.bottom), SpriteEffect::OneColor(Colors::Black));
					}
				}
				switch (layer)
				{
				case 0:
					assert(cellType >= 0 && cellType < Settings::nDiffFieldTypes);
					//gfx.DrawSurface(curCellPos, tC->s_Fields.at(curCell.type), SpriteEffect::Chroma(Colors::Magenta));
					gfx.DrawSurface(curCellPos,RectI(Vei2(0,0),50,50), tC->Fields.at(cellType).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));

					for (int i = 0; i < Settings::nDiffFieldTypes - 1; i++)
					{
						if (conMap[i][curXY.x][curXY.y] == 1)
						{
							DrawConnections(1+i, Vei2(curCellPos.left,curCellPos.top), GetAroundMatrix(curXY), gfx);
						}
					}

					break;
				case 1:
					if (curXY == fCell)
					{
						gfx.DrawSurface(curCellPos.GetExpanded(cSize.x / 5), tC->Frames.at(0), SpriteEffect::Chroma(Colors::Magenta));
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
void World::DrawConnections(int lookFor,Vei2 topLeft, Matrix<int> aMat, Graphics& gfx)const
{
	assert(aMat.GetSize().x == 3 && aMat.GetSize().y == 3);

	if (aMat[1][1] < lookFor)
	{						

		if (aMat[1][2] == lookFor)			
		{
			if (aMat[0][1] == lookFor)	// 1
			{
				gfx.DrawSurface(resC->fsC.FieldCon[0] + topLeft, RectI(Vei2(51, 0), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else if (aMat[0][1] != lookFor)	// 13
			{
				gfx.DrawSurface(resC->fsC.FieldCon[0] + topLeft, RectI(Vei2(160, 0), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			if (aMat[2][1] == lookFor)	// 2
			{
				gfx.DrawSurface(resC->fsC.FieldCon[1] + topLeft, RectI(Vei2(76, 0), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else if (aMat[2][1] != lookFor)	// 14
			{
				gfx.DrawSurface(resC->fsC.FieldCon[1] + topLeft, RectI(Vei2(185, 0), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
		}
		else if (aMat[1][2] != lookFor)
		{	
			if (aMat[0][1] == lookFor)	//9
			{
				gfx.DrawSurface(resC->fsC.FieldCon[0] + topLeft, RectI(Vei2(109, 0), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else if (aMat[0][1] < lookFor && aMat[0][2] == lookFor)	// 5
			{
				gfx.DrawSurface(resC->fsC.FieldCon[4] + topLeft, RectI(Vei2(102, 0), 6, 6), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			if (aMat[2][1] == lookFor)
			{
				gfx.DrawSurface(resC->fsC.FieldCon[1] + topLeft, RectI(Vei2(134, 0), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else if (aMat[2][1] < lookFor && aMat[2][2] == lookFor)	// 6
			{
				gfx.DrawSurface(resC->fsC.FieldCon[5] + topLeft, RectI(Vei2(102, 7), 6, 6), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
		}
		if (aMat[1][0] == lookFor)					
		{
			if (aMat[0][1] == lookFor)	// 3
			{
				gfx.DrawSurface(resC->fsC.FieldCon[2] + topLeft, RectI(Vei2(51, 25), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else if (aMat[0][1] != lookFor)	// 15
			{
				gfx.DrawSurface(resC->fsC.FieldCon[2] + topLeft, RectI(Vei2(160, 25), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			if (aMat[2][1] == lookFor)	// 4
			{
				gfx.DrawSurface(resC->fsC.FieldCon[3] + topLeft, RectI(Vei2(76, 25), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else if (aMat[2][1] != lookFor)	// 16
			{
				gfx.DrawSurface(resC->fsC.FieldCon[3] + topLeft, RectI(Vei2(185, 25), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
		}
		else if (aMat[1][0] != lookFor)
		{
			if (aMat[0][1] < lookFor && aMat[0][0] == lookFor)	// 7
			{
				gfx.DrawSurface(resC->fsC.FieldCon[6] + topLeft, RectI(Vei2(102, 14), 6, 6), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else if (aMat[0][1] == lookFor)	// 11
			{
				gfx.DrawSurface(resC->fsC.FieldCon[2] + topLeft, RectI(Vei2(109, 25), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			if (aMat[2][1] < lookFor && aMat[2][0] == lookFor)	// 8
			{
				gfx.DrawSurface(resC->fsC.FieldCon[7] + topLeft, RectI(Vei2(102, 21), 6, 6), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else if (aMat[2][1] == lookFor)	// 12
			{
				gfx.DrawSurface(resC->fsC.FieldCon[3] + topLeft, RectI(Vei2(134, 25), 25, 25), tC->Fields.at(lookFor).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
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
	
	for (int i = 0; i < s.nIslands; i++)				//See spawnen
	{
		GenerateExplosion(Vei2(rng.Calc(s.wSize.x), rng.Calc(s.wSize.y)), 10, 1);
		//GenerateCircle(Vei2(rng.Calc(s.wSize.x), rng.Calc(s.wSize.y)), 7, 1);
	}
	for (int i = 0; i < wSize.y; i++)					//Nord & Suedpol
	{
		GenerateCircle(Vei2(i,0), rng.Calc(5), 2);
		//GenerateCircle(Vei2(i,wSize.y-1), 1,2);
	}
	//GenerateLine(Vei2(0, 250), Vei2(10, 253), 2);
	GenerateExplosion(Vei2(0, 250), 20,7,100);
	GenerateExplosion(Vei2(0, 250), 20, 2, 70);
	GenerateExplosion(Vei2(0, 250), 20, 3, 50);
	GenerateExplosion(Vei2(0, 250), 20, 5, 30);
	UpdateConMap();
}
void World::GenerateCircle(Vei2 pos, int radius,int type ,float density)
{
	assert(density > 0 && density <= 1.0f);

	for (int y = -radius; y < radius; y++)
	{
		for (int x = -radius; x < radius; x++)
		{
			if (sqrt(y * y + x * x) <= radius)
			{
				Vei2 putPos = PutInWorldX(pos + Vei2(x, y));
				if (IsInWorld(putPos))
				{
					//bool put = true;
					if (density != 0.0f)
					{
						//float dist = sqrt(y * y + x * x);
						//float normalD = rng.GetNormalDist();
						//if (rng.GetNormalDist() < density)
							//put = false;
					}
					cells(putPos).type = type;
				}
			}
		}
	}
}
void World::GenerateLine(Vec2 p0, Vec2 p1, int type, int thickness, float density)
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
					GenerateLine(Vec2(p1.x, p1.y + i), Vec2(p0.x, p0.y + i), type);
				}
				else
				{
					GenerateLine(Vec2(p1.x + i, p1.y), Vec2(p0.x + i, p0.y), type);
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
				cells(PutInWorldX(Vei2(x, yi))).type = type;
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

				cells(PutInWorldX(Vei2(xi, y))).type = type;
			}
		}
	}
}
void World::GenerateExplosion(Vei2 pos, int maxLineLength, int type, int nRolls)	
{
	for (int i = 0; i < nRolls; i++)
	{
		float rad = (float)rng.Calc(360) * 0.0174533f;
		Vec2 p1 = (Vec2) GigaMath::RotPointToOrigin<float>(1.0f,0.0f, rad);
		Vei2 scaled = pos + Vei2(p1 * (maxLineLength*1/2+ rng.Calc(maxLineLength*1/2)));
		GenerateLine(Vec2(pos), Vec2(scaled), type);
	}
}