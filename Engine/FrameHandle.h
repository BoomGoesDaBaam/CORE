#pragma once
#include "World.h"
#include <memory>
class Component
{
protected:
	RectF pos;
	Color c;
public:
	Component(RectF pos, Color c):pos(pos),c(c){}
	virtual void Draw(RectF parent, Graphics& gfx)
	{
		gfx.DrawRect(pos+Vec2(parent.left, parent.top), c, SpriteEffect::Nothing());
	}
};
class Text: public Component
{
	std::string text;
	Font f;
	int size;
public:
	Text(std::string text, RectF pos, int size, Font f, Color c);
	void Draw(RectF parent, Graphics& gfx) override
	{
		f.DrawText(text, pos.left + parent.left, pos.top + parent.top, size, c);
	}
};

class FrameHandle
{
	class Frame
	{
	protected:
		sharedResC resC;
		bool visible = true;
		bool grabbed = false;
		bool mouseHovers = false;
		int type;
		RectF pos;
		std::vector<std::unique_ptr<Component>> comps;
		Vec2 lastMouseP = Vec2(0,0);
	public:
		Frame(RectF pos, int type, sharedResC resC);

		virtual void Draw(Graphics& gfx) {
			if (mouseHovers)
			{
				gfx.DrawRect(pos, Colors::Blue, SpriteEffect::Nothing());
			}
			else
			{
				gfx.DrawRect(pos, Colors::Red, SpriteEffect::Nothing());
			}
			DrawComps(gfx);
		}
		void DrawComps(Graphics& gfx);
		virtual bool HandleMouseInput(Mouse::Event& e)
		{
			Vec2 mP = (Vec2)e.GetPos();
			if (grabbed)
			{
				if (e.GetType() == Mouse::Event::LRelease)
				{
					Release();
				}
				else
				{
					Move(mP);
				}
			}
			if (e.GetType() == Mouse::Event::LPress)
			{
				Grab(mP);
			}
			if (pos.Contains((Vec2)e.GetPos()))
			{
				mouseHovers = true;
			}
			else
			{
				mouseHovers = false;
			}
			return pos.Contains((Vec2)e.GetPos());
		}
		void AddText(std::string text, RectF pos, int size, Font f, Color c);
		void AddButton(std::string text, RectF pos, int size, Font f, Color c);
		void Grab(Vec2 mP);
		void Move(Vec2 mP);
		void Release();
	};
	class TileFrame: public Frame
	{
		Matrix<int> matrix, outline;
		int cellSize;
		std::vector<RectI> offset;
	public:
		TileFrame(Vec2 pos, Matrix<int> matrix, int type, int tileSize, sharedResC resC);
		void Draw(Graphics& gfx)override
		{
			RectI tileRect = RectI(pos.GetTopLeft<int>(), cellSize, cellSize) + Vei2(cellSize, cellSize);
			for (int y = 0; y < outline.GetRows(); y++)
			{
				for (int x = 0; x < outline.GetColums(); x++)
				{
					if (outline[x][y] == 1)
					{
						gfx.DrawSurface(tileRect + Vei2(cellSize * (x - 1), cellSize * (y - 1)), RectI(Vei2(0, 0), 50, 50), resC->tC.windows.at(0).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						//gfx.DrawSurface(configs.pos, RectI(Vei2(0, 0), 50, 50), configs.resC->tC.windows[0].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						//gfx.DrawSurface(RectI((Vei2)configs.pos, configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y), RectI(Vei2(0, 0), configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y),Graphics::GetScreenRect<int>(), configs.pics->tileFramePics[configs.style], SpriteEffect::Chroma(Colors::Magenta));
					}
					else
					{
						auto a = outline.GetAroundMatrix(Vei2(x, y));
						a.MirrowVertical();
						gfx.DrawConnections(1, pos.GetTopLeft<int>() + Vei2(cellSize * (x), cellSize * (y)), a, offset, resC->tC.windows[0].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
					}
				}
			}
			DrawComps(gfx);
		}
		bool HandleMouseInput(Mouse::Event& e)override
		{
			Vec2 mP = (Vec2)e.GetPos();
			Vec2 delta = mP - pos.GetTopLeft<float>();
			Vec2 onCellPos = Vec2((int)delta.x % cellSize, (int)delta.y % cellSize);
			Vei2 hitCell = (Vei2)delta / cellSize;

			if (e.GetType() == Mouse::Event::LPress)
			{
				if (pos.Contains((Vec2)e.GetPos()))
				{
					if (outline(hitCell) == 1)
					{
						Grab(mP);
						return true;
					}
					if (hitCell.x == 0 || hitCell.x == outline.GetColums() - 1 || hitCell.y == 0 || hitCell.y == outline.GetRows() - 1)
					{
						Matrix<int> chromaM = SubAnimation::PutOnTopOfEachOther(resC->fsC.GetConnectionAnimationVec(1, hitCell, false, outline.GetAroundMatrix(hitCell)), Vei2(50, 50), 1, 0);
						chromaM.MirrowVertical();
						if (chromaM[onCellPos.x][onCellPos.y] == 1)
						{
							Grab(mP);
							return true;
						}

					}
				}
			}
			else if(grabbed)
			{
				if (e.GetType() == Mouse::Event::LRelease)
				{
					Release();
				}
				else
				{
					Move(mP);
				}
				return true;
			}

			return false;
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
	const Frame& operator[](std::size_t idx) const { return *frames[idx].get(); }
	Frame& operator[](std::size_t idx) { return *frames[idx].get();  }

	FrameHandle(sharedResC resC);
	bool HandleMouseInput(Mouse::Event& e);
	void Draw(Graphics& gfx);
	void AddFrame(RectF posWind, int type, sharedResC resC);
	void AddTileFrame(Vec2 posWind, Matrix<int> matrix, int type, int tileSize, sharedResC resC);
	void AddScrollWindow(RectF posWind, RectF posSB, int type = 0, int nElem = 5, std::string defValue = "Text hier");
	void AddText(RectF posWind, std::string text, int size, Font& font, Color c);
	void AddButton(RectF posWind, Color c);
	static constexpr float percentForGrab = 0.2;			// 0 - 1
};

