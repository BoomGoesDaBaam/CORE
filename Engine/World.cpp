#include "World.h"

World::World(WorldSettings wSettings, std::shared_ptr<TexturesCollection> tC, Vec2& camera) :tC(std::move(tC)), c(camera)
{
	LoadSettings(wSettings);
	mCell = wSize / 2;
	fCell = wSize / 2;
	mCell.x = 0;
	Generate(wSettings);
}

bool World::IsInWorld(Vei2 v)const
{
	return v.x >= 0 && v.y >= 0 && v.y < wSize.y && v.x < wSize.x;
}
int World::Vec2Number(Vei2 v)const
{
	int x = (wSize.x + v.x) % wSize.x;
	int y = (wSize.y + v.y) % wSize.y;
	return y * wSize.x + x;
}
Vei2 World::Number2Vec(int pos)const
{
	return Vei2(pos / wSize.x, pos % wSize.x);
}
Vei2 World::PutInWorld(Vei2 v)const
{
	
	assert(v.y > 0 && v.y < wSize.y);
	
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

	return PutInWorld(deltaCells + mCell);
}
void World::ApplyCameraChanges(Vec2 cDelta)
{
	c.x += -cDelta.x;
	c.y += cDelta.y;

	if (c.x < 0)
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
	Vec2 mos = (Vec2) Graphics::GetMidOfScreen();
	if (mCell.y < mos.y / cSize.y)
	{
		mCell.y = 4;
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
}
void World::Draw(Graphics& gfx)const
{
	for (int y = -6; y < 7; y++)
	{
		for (int x = -5; x < 7; x++)
		{
			//Vei2 curCellI=
			int cellType = cells.at(Vec2Number(Vei2(x, y) + mCell)).type;
			RectI curCellPos = (RectI)GetCellRect(Vei2(x, y) + mCell) + Vei2(-c.x,+c.y);


			assert(cellType >= 0 && cellType < tC->s_Fields.size());
			gfx.DrawSurface(curCellPos, tC->s_Fields.at(cellType), SpriteEffect::Chroma(Colors::Magenta));
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
	cells.clear();
	for (int i = 0; i < s.wSize.x*s.wSize.y; i++)		//Alles mit Wasser voll machen
	{
		cells.emplace_back(Cell(0));
	}
	for (int i = 0; i < s.nIslands; i++)				//Alles mit Landschaft vollmachen
	{
		GenerateCircle(Vei2(rng.Calc(s.wSize.x), rng.Calc(s.wSize.y)), 3, 1);
	}
	for (int i = 0; i < wSize.y; i++)					//Nord & Suedpol
	{
		GenerateCircle(Vei2(i,0), 1, 2);
		//GenerateCircle(Vei2(i,wSize.y-1), 1,2);
	}
}
void World::GenerateCircle(Vei2 where, int radius,int type ,float density)
{
	assert(density > 0 && density <= 1.0f);

	for (int y = -radius; y < radius; y++)
	{
		for (int x = -radius; x < radius; x++)
		{
			if (sqrt(y * y + x * x) <= radius && IsInWorld(where+Vei2(x,y)))
			{
				//bool put = true;
				if (density != 0.0f)
				{
					//float dist = sqrt(y * y + x * x);
					//float normalD = rng.GetNormalDist();
					//if (rng.GetNormalDist() < density)
						//put = false;
				}

				//if(put)
				cells.at(Vec2Number(Vei2(x, y) + where)).type = type;
			}
		}
	}
}
