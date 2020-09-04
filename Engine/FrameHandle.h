#pragma once
#include "World.h"
#include <memory>
class FrameHandle
{
	class Frame
	{
	protected:
		sharedResC resC;
		bool visible = true;
		RectF pos;
		int type;
	public:
		Frame(RectF pos, int type, sharedResC resC)
			:
			pos(pos),
			type(type),
			resC(std::move(resC)){}

		virtual void Draw(Graphics& gfx) {
			gfx.DrawRect(pos, Colors::Red, SpriteEffect::Nothing());
		}
		virtual bool HandleMouseInput(Mouse::Event& e)
		{
			return pos.Contains((Vec2)e.GetPos());
		}
	};
	class TileFrame: public Frame
	{
		Matrix<int> matrix, outline;
		int tileSize;
		std::vector<RectI> offset;
	public:
		TileFrame(Vec2 pos, Matrix<int> matrix, int type, int tileSize, sharedResC resC)
			:
			Frame(RectF(Vec2(pos.x - tileSize,pos.y - tileSize), tileSize * (matrix.GetColums()+2),tileSize * (matrix.GetRows()+2)), type, resC),
			matrix(matrix),
			outline(matrix.GetMatPlusZeroOutline()),
			tileSize(tileSize),
			offset(resC->fsC.GetConOffset(Vei2(tileSize, tileSize)))
		{

		}
		void Draw(Graphics& gfx)override
		{
			RectI tileRect = RectI(pos.GetTopLeft<int>(), tileSize, tileSize) + Vei2(tileSize, tileSize);
			for (int y = 0; y < outline.GetRows(); y++)
			{
				for (int x = 0; x < outline.GetColums(); x++)
				{
					if (outline[x][y] == 1)
					{
						gfx.DrawSurface(tileRect + Vei2(tileSize * (x - 1), tileSize * (y - 1)), RectI(Vei2(0, 0), 50, 50), resC->tC.windows.at(0).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						//gfx.DrawSurface(configs.pos, RectI(Vei2(0, 0), 50, 50), configs.resC->tC.windows[0].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						//gfx.DrawSurface(RectI((Vei2)configs.pos, configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y), RectI(Vei2(0, 0), configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y),Graphics::GetScreenRect<int>(), configs.pics->tileFramePics[configs.style], SpriteEffect::Chroma(Colors::Magenta));
					}
					else
					{
						auto a = outline.GetAroundMatrix(Vei2(x, y));
						a.MirrowVertical();
						gfx.DrawConnections(1, pos.GetTopLeft<int>() + Vei2(tileSize * (x), tileSize * (y)), a, offset, resC->tC.windows[0].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
					}
				}
			}
		}
		bool HandleMouseInput(Mouse::Event& e)override
		{
			return pos.Contains((Vec2)e.GetPos());
		}
		bool HitFrame(Vec2 mP)
		{

		}
	};
	/*
	void Draw(Graphics& gfx)override
		{
			for (int y = 0; y < outline.GetRows(); y++)
			{
				for (int x = 0; x < outline.GetColums(); x++)
				{
					if (outline[x][y] == 1)
					{
						gfx.DrawSurface((RectI)configs.GetRect() + Vei2((int)configs.size * (x-1), (int)configs.size * (y-1)), RectI(Vei2(0, 0), 50, 50), configs.resC->tC.windows.at(0).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						//gfx.DrawSurface(configs.pos, RectI(Vei2(0, 0), 50, 50), configs.resC->tC.windows[0].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						//gfx.DrawSurface(RectI((Vei2)configs.pos, configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y), RectI(Vei2(0, 0), configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y),Graphics::GetScreenRect<int>(), configs.pics->tileFramePics[configs.style], SpriteEffect::Chroma(Colors::Magenta));
					}
					else
					{
						auto a = outline.GetAroundMatrix(Vei2(x, y));
						a.MirrowVertical();
						gfx.DrawConnections(1,(Vei2) configs.pos + Vei2((int)configs.size * (x-1), (int)configs.size * (y-1)), a, offset, configs.resC->tC.windows[0].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
					}
				}
			}

		}





	class Scrollbar// : public Component
	{
	public:
		Scrollbar(RectF posSB)//: Component(posSB)
		{}
	};
	
	class Textfield : public Frame
	{
		std::string text;
		int size;
		Font& font;
		Color c;
	public:
		Textfield(RectF pos, int type, std::string text, int size, Font& font, Color c)
			:Frame(pos,type),
			text(text), size(size), font(font) {}
		void Draw(Graphics& gfx)override {
			font.DrawText(text, pos.left, pos.top, size, c);
		}
	};
	/*
	class Element //: public Component
	{
		RectF pos;
	public:
		Element(RectF pos) //:Component(pos)
			:pos(pos)
		{}

	};
	*/
	/*
	class ExpandableWindow : public Component
	{
		ExpandableWindow(RectI posWind, RectI posExpButton)
	};
	class Button : public Frame
	{
		Color c;
	public:
		Button(RectF pos, int type, Color c) : Frame(pos,type), c(c)
		{

		}
		bool Hit(Vei2 mP, Color c)
		{
			return pos.Contains((Vec2)mP);
		}
		void Draw(Graphics& gfx)override { gfx.DrawRect(pos, c, SpriteEffect::Nothing()); }
	};

	class ScrollWindow : public Frame
	{
		//Scrollbar scrollBar;
		int nElem;
		std::string defValue;
	public:
		ScrollWindow(RectF posWind, RectF posSB, int type = 0, int nElem = 5, std::string defValue = "Text hier")
			: Frame(posWind,type),
			//scrollBar(posSB),
			nElem(nElem), defValue(defValue)
		{

		}
		void Draw(Graphics& gfx)override { gfx.DrawRect(pos, Colors::Red, SpriteEffect::Nothing()); }
	};
	*/

	std::vector<std::unique_ptr<Frame>> frames;
	sharedResC resC;
public:
	FrameHandle(sharedResC resC)
		:resC(std::move(resC))
	{

	}
	bool HandleMouseInput(Mouse::Event& e)
	{

		for (auto& frame : frames)
		{
			if (frame->HandleMouseInput(e))
			{
				return true;
			}
		}
		return false;
	}
	void Draw(Graphics& gfx)
	{
		for (int i = 0; i < frames.size(); i++)
		{
			frames[i]->Draw(gfx);
		}
	}
	void AddFrame(RectF posWind, int type, sharedResC resC)
	{
		frames.push_back(std::make_unique<Frame>(posWind, type, resC));
	}
	void AddTileFrame(Vec2 posWind, Matrix<int> matrix, int type, int tileSize, sharedResC resC)
	{
		frames.push_back(std::make_unique<TileFrame>(posWind, matrix, type, tileSize, resC));

	}
	void AddScrollWindow(RectF posWind, RectF posSB, int type = 0, int nElem = 5, std::string defValue = "Text hier")
	{
		//comps.push_back(std::make_unique<ScrollWindow>(posWind + configs.pos, posSB, type, nElem, defValue));
	}
	void AddText(RectF posWind, std::string text, int size, Font& font, Color c)
	{
		//comps.push_back(std::make_unique<Textfield>(posWind + configs.pos, text, size, font, c));
	}
	void AddButton(RectF posWind, Color c)
	{
		//comps.push_back(std::make_unique<Button>(posWind + configs.pos, c));
	}
};

