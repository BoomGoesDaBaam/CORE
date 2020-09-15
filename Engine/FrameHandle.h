#pragma once
#include "World.h"
#include <memory>
class Component
{
public:
	RectF pos;
	Color c;
	std::vector<int> activInStates;
	std::string text;
	Component(RectF pos, std::string text, Color c,std::vector<int> activInStates):pos(pos),c(c),text(text), activInStates(activInStates){}
	virtual void Draw(RectF parent, Graphics& gfx)
	{
		gfx.DrawRect(pos+Vec2(parent.left, parent.top), c, SpriteEffect::Nothing());
	}
};
class Text: public Component
{
public:
	Font f;
	int size;
	Text(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates);
	void Draw(RectF parent, Graphics& gfx) override
	{
		f.DrawText(text, pos.left + parent.left, pos.top + parent.top, size, c);
	}
};
class Button : public Component
{
public:
	Font f;
	int size;
	Button(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates);
	void Draw(RectF parent, Graphics& gfx) override
	{
		gfx.DrawRect(pos + parent.GetTopLeft<float>(), Colors::Blue, SpriteEffect::Rainbow());
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
		int type,curState=0, nStates=1;
		RectF pos;
		std::vector<std::unique_ptr<Component>> comps;
		Vec2 lastMouseP = Vec2(0,0);
		Vec2 posOfLastPress = Vec2(-1, -1);
	public:
		Frame(RectF pos, int type, int nStates, sharedResC resC);
		void Draw(Graphics& gfx) {
			if (mouseHovers)
			{
				switch (type)
				{
				case 0:
					switch (curState)
					{
					case 0:
						gfx.DrawSurface(RectI(pos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						gfx.DrawSurface((RectI)pos, resC->tC.windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.9f));
						break;
					case 1:
						gfx.DrawSurface(RectI(pos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						break;
					}
					break;
				}
			}
			else
			{
				switch (type)
				{
				case 0:
					switch (curState)
					{
					case 0:
						gfx.DrawSurface(RectI(pos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						gfx.DrawSurface((RectI)pos, resC->tC.windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.75f));
						break;
					case 1:
						gfx.DrawSurface(RectI(pos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						break;
					}
					break;
				}
			}
			DrawComps(gfx);
		}
		void Settext(std::string text, int index)
		{
			assert(index >= 0 && index < comps.size());
			comps[index]->text = text;
		}
		bool HandleMouseInput(Mouse::Event& e)
		{
			Vec2 mP = (Vec2)e.GetPos();
			bool hit = Hit(mP);

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
			if (e.GetType() == Mouse::Event::LPress && hit)
			{
				Grab(mP);
				posOfLastPress = mP;
			}
			if (nStates > 1 && hit && e.GetType() == Mouse::Event::LRelease && posOfLastPress == mP)
			{
				curState++;
				if (curState == nStates)
				{
					curState = 0;
				}
			}
			if (hit)
			{
				mouseHovers = true;
				return true;
			}
			else
			{
				mouseHovers = false;
			}
			return false;
		}
		bool Hit(Vec2 mP)
		{
			Vei2 mpRel = (Vei2)(mP - pos.GetTopLeft<float>());
			switch (type)
			{
			case 0:
				switch (curState)
				{
				case 0:
					return resC->tC.windowsFrame[0].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel) || resC->tC.windowsFrame[2].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel);
					break;
				case 1:

					return resC->tC.windowsFrame[1].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel);
				
					/*
					Matrix<int> chromaM = resC->tC.windowsFrame[1].GetCurSurface().GetChromaMatrix(Colors::Magenta);
					chromaM.MirrowVertical();
					float xRel = (float)mpRel.x / pos.GetWidth(); 
					float yRel = (float)mpRel.y / (pos.GetHeight() / 20);
					if (Vec2::IsPositivFactor(Vec2(xRel, yRel)) && chromaM(Vei2(xRel * surSize.x, yRel * surSize.y)) == 1)
					{
						return true;
					}
					*/
					break;
				}
				break;


			}
			return false;
		}
		void SetStates(int nStates);
		void DrawComps(Graphics& gfx);
		void AddText(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates = {});
		void AddButton(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates = {});
		void Grab(Vec2 mP);
		void Move(Vec2 mP);
		void Release();
	};
	

	std::vector<std::unique_ptr<Frame>> frames;
	sharedResC resC;
public:
	const Frame& operator[](std::size_t idx) const { return *frames[idx].get(); }
	Frame& operator[](std::size_t idx) { return *frames[idx].get();  }		

	FrameHandle(sharedResC resC);
	bool HandleMouseInput(Mouse::Event& e);
	void Draw(Graphics& gfx);
	void AddFrame(RectF posWind, int type, int nStates, sharedResC resC);
	void AddScrollWindow(RectF posWind, RectF posSB, int type = 0, int nElem = 5, std::string defValue = "Text hier");

	// ###### Update existing Frames ######
	void UpdateFieldinformation(World& curW)				
	{
		frames[0]->Settext(Settings::lang_Flora[Settings::lang] + ":" + Settings::GetTypeString(curW.GetfCellType()), 1);
	}



	static constexpr float percentForGrab = 0.05;			// 0 - 1
};



