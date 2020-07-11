#include "World.h"

World::World(WorldSettings wSettings, std::shared_ptr<TexturesCollection> tC, Vec2& camera) :tC(std::move(tC)), c(camera),cells(wSettings.wSize,wSettings.defType)
{
	LoadSettings(wSettings);
	mCell = wSize / 2;
	fCell = wSize / 2;
	mCell.x = 0;
	Generate(wSettings);
}
bool World::IsInWorld(Vei2& v)const
{
	return v.x >= 0 && v.x < wSize.x && v.y >= 0 && v.y < wSize.y;
}
Vei2 World::PutInWorldX(Vei2& v)const
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
	Vec2 mos = (Vec2) Graphics::GetMidOfScreen() + c;
	Vei2 deltaPixel = (Vei2) mos - (Vei2) mP;
	Vei2 deltaCells = { 0,0 };

	deltaCells.y = (deltaPixel.y / cSize.y);
	if (deltaPixel.y < 0)
		deltaCells.y--;
	deltaCells.x = -(deltaPixel.x / cSize.x);
	if (deltaPixel.x < 0)
		deltaCells.x++;

	return PutInWorldX(deltaCells + mCell);
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
void World::HandleMouseEvents(Mouse::Event& e)
{
	if (e.GetType() == Mouse::Event::LRelease)
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
	ApplyCameraChanges(Vec2(0, 0));
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
	
	for (int y = -3-(mos.y / cSize.y) * 4; y <= 3 + (mos.y / cSize.y) * 4; y++)
	{
		for (int x = -3-(mos.x / cSize.x)*4; x <= 3 + (mos.x / cSize.x) * 4; x++)
		{
			Vei2 curXY = mCell + Vei2(x,y);
			const Cell& curCell = cells(PutInWorldX(curXY));

			int cellType = curCell.type;

			RectI curCellPos = (RectI)GetCellRect(Vei2(x, y) + mCell) + Vei2(-c.x,+c.y);


			assert(cellType >= 0 && cellType < tC->s_Fields.size());
			gfx.DrawSurface(curCellPos, tC->s_Fields.at(curCell.type), SpriteEffect::Chroma(Colors::Magenta));
		}
	}
}

void World::LoadSettings(WorldSettings& s)
{
	wSize = s.wSize;
	cSize = s.cSize;
}

void World::Generate(WorldSettings& s)
{
	int j = 0;
	cells.Init(s.wSize,s.defType);						//Welt erstellen & default value setzen

	for (int i = 0; i < s.nIslands; i++)				//Alles mit Landschaft vollmachen
	{
		GenerateCircle(Vei2(rng.Calc(s.wSize.x), rng.Calc(s.wSize.y)), 7, 1);
	}
	for (int i = 0; i < wSize.y; i++)					//Nord & Suedpol
	{
		GenerateCircle(Vei2(i,0), rng.Calc(5), 2);
		//GenerateCircle(Vei2(i,wSize.y-1), 1,2);
	}
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
