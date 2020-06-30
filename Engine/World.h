#include <vector>
#include <memory>
#include "Rect.h"
#include "Graphics.h"
#pragma once
class World
{
	class Cell
	{
	public:
		Cell(int type):type(type)
		{

		}
		int type = 0;
	};
	Vei2 wSize;
	Vec2 cSize = { 50,50 };
	Vei2 fCell;
public:
	World(Vei2 wSize):wSize(wSize)
	{
		fCell = wSize / 2;
		for (int y = 0; y < wSize.y; y++)
		{
			for (int x = 0; x < wSize.y; x++)
			{
				cells.push_back(Cell(x%2));
			}
		}
	}
	
	RectF GetCellRect(Vei2 cellP)
	{
		Vei2 d = fCell - cellP;

		return RectF(Vec2(fCell.x + cSize.x * d.x, fCell.y + cSize.y * d.y), cSize.x, cSize.y);
	}
	void Draw(Graphics& gfx)
	{
		gfx.DrawCircle(50, 50, 50, Colors::Yellow);
	}

	std::vector<Cell> cells;


	Vei2 GetwSize() { return wSize; }
	Vec2 GetcSize() { return cSize; }
	Vei2 GetfCell() { return fCell; }
};

