#include "World.h"

World::World(WorldSettings wSettings, std::shared_ptr<TexturesCollection> tC) :tC(std::move(tC))
{
	LoadSettings(wSettings);
	fCell = wSize / 2;
	Generate(wSettings);
}

bool World::IsInWorld(Vei2 pos)
{
	return pos.x >= 0 && pos.y >= 0 && pos.y < wSize.y && pos.x < wSize.x;
}
int World::Vec2Number(Vei2 pos)
{
	return pos.y * wSize.x + pos.x;
}
Vei2 World::Number2Vec(int pos)
{
	return Vei2(pos / wSize.x, pos % wSize.x);
}
RectF World::GetCellRect(Vei2 cellP)
{
	Vei2 d = fCell - cellP;

	Vei2 mos = Graphics::GetMidOfScreen();
	mos.x += d.x * cSize.x;
	mos.y += d.y * cSize.y;
	return RectF(Vec2(mos), cSize.x, cSize.y);
}

void World::Draw(Graphics& gfx,Vec2& c)
{
	for (int y = -15; y < 15; y++)
	{
		for (int x = -15; x < 15; x++)
		{
			int cellType = cells.at(Vec2Number(Vei2(x, y) + fCell)).type;
			RectI curCellPos = (RectI)GetCellRect(Vei2(x, y) + fCell) + (Vei2)c;

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
	for (int i = 0; i < s.wSize.x*s.wSize.y; i++)
	{
		cells.emplace_back(Cell(0));
	}
	for (int i = 0; i < s.nIslands; i++)
	{
		GenerateCircle(Vei2(rng.Calc(s.wSize.x), rng.Calc(s.wSize.y)),3);
	}
}
void World::GenerateCircle(Vei2 where, int radius)
{
	for (int y = -radius; y < radius; y++)
	{
		for (int x = -radius; x < radius; x++)
		{
			if (sqrt(y * y + x * x) <= radius && IsInWorld(where+Vei2(x,y)))
			{
				cells.at(Vec2Number(Vei2(x, y) + where)).type = 1;
			}
		}
	}
}
