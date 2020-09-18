#pragma once
#include "World.h"
#include <memory>
class Component
{
public:
	//Component* parentC;
	RectF GetPos() { return pos + parentPos.GetTopLeft<float>(); }
	RectF pos;
	RectF& parentPos;
	Color c = Colors::Black;
	std::vector<int> activInStates;
	std::string text = "no title";
	Component(RectF& parentPos, RectF pos) :pos(pos), parentPos(parentPos){}
	virtual void Draw(Graphics& gfx)
	{
		gfx.DrawRect(pos+Vec2(parentPos.left, parentPos.top), c, SpriteEffect::Nothing());
	}
	virtual bool HandleMouseInput(Mouse::Event& e)
	{
		return pos.Contains((Vec2)e.GetPos());
	}
};
class Text: public Component
{
public:
	Font f;
	int size;
	Text(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates, RectF& parentPos);
	void Draw(Graphics& gfx) override
	{
		f.DrawText(text, pos.left + parentPos.left, pos.top + parentPos.top, size, c);
	}
};
class Button : public Component
{
public:
	Font f;
	int size;
	Button(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates, RectF& parentPos);
	void Draw(Graphics& gfx) override
	{
		gfx.DrawRect(pos + parentPos.GetTopLeft<float>(), Colors::Blue, SpriteEffect::Rainbow());
		f.DrawText(text, pos.left + parentPos.left, pos.top + parentPos.top, size, c);
	}
};


class FrameHandle
{
	class Frame: public Component
	{
	protected:
		sharedResC resC;
		bool visible = true;
		bool grabbed = false;
		bool mouseHovers = false;
		int type,curState=0, nStates=1;								//'curState' = 0 is minimized in every type
		std::vector<std::unique_ptr<Component>> comps;
		Vec2 lastMouseP = Vec2(0,0);
		Vec2 posOfLastPress = Vec2(-1, -1);
	public:
		Frame(RectF pos, int type, RectF& parentPos, sharedResC resC);
		Frame(const Frame& rhs)
			:Component(rhs.parentPos,rhs.pos),
			resC(std::move(rhs.resC)),
			type(rhs.type),
			nStates(rhs.nStates)
		{
			this->activInStates = rhs.activInStates;
		}
		
		void DrawComps(Graphics& gfx);
		virtual void Draw(Graphics& gfx)override {
			assert(type == 0 || type == -1);
			if (mouseHovers)
			{
				switch (type)
				{
				case 0:
					switch (curState)
					{
					case 0:
						gfx.DrawSurface(RectI(pos.GetTopLeft<int>() + parentPos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						break;
					case 1:
						gfx.DrawSurface(RectI(pos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20) + parentPos.GetTopLeft<int>(), resC->tC.windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						gfx.DrawSurface((RectI)pos + parentPos.GetTopLeft<int>(), resC->tC.windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.9f));
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
						gfx.DrawSurface(RectI(pos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20) + parentPos.GetTopLeft<int>(), resC->tC.windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						break;
					case 1:
						gfx.DrawSurface(RectI(pos.GetTopLeft<int>() + parentPos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						gfx.DrawSurface((RectI)pos + parentPos.GetTopLeft<int>(), resC->tC.windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.75f));
						break;
					}
					break;
				}
			}
			DrawComps(gfx);
		}
		
		bool HandleMouseInput(Mouse::Event& e)override
		{
			for (const auto& comp : comps)
			{
				if (comp->HandleMouseInput(e))
				{
					return true;
				}
			}
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
				NextState();
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
		bool Hit(Vec2 mP);
		
		void SetText(std::string text, int index);
		bool SetState(int state) { bool changed = curState == state; curState = state; return changed; }
		void NextState()
		{
			curState++;
			if (curState == nStates)
			{
				curState = 0;
			}
		}
		Component* GetComp(int index) { return comps[index].get(); }
		int GetNumberOfComps() { return comps.size(); }
		int GetCurState() { return curState; }
		int GetExtendedHeight();
		bool IsExtended();
		
		void AddText(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates = {});
		void AddButton(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates = {});
		void AddSubFrame(RectF pos, int type, sharedResC resC);
		
		void Grab(Vec2 mP);
		void Move(Vec2 mP);
		void Release();
	};


	class MultiFrame: public Frame	
	{
		std::vector<std::unique_ptr<Frame>> frames;
	public:
		MultiFrame(RectF& parentPos, RectF pos, sharedResC resC) :Frame(pos, -1,parentPos, resC) {}
		Frame* AddFrame(Frame frame);
		virtual void Draw(Graphics& gfx)override 
		{
			for (int i = 0; i < frames.size(); i++)
			{
				frames[i]->Draw(gfx);
			}
		}
		bool HandleMouseInput(Mouse::Event& e)override
		{
			Frame::HandleMouseInput(e);
			std::vector<bool> extended;
			for (int i = 0; i < frames.size(); i++)
			{
				extended.push_back(frames[i]->GetCurState());
			}
			int changed = -1;
			int yMove = 0;
			for (int n = 0; n < frames.size(); n++)
			{
				if (frames[n]->HandleMouseInput(e))
				{
					for (int i = 0; i < frames.size(); i++)
					{
						frames[i]->pos.top += yMove;
						frames[i]->pos.bottom += yMove;
						if (extended[i] != frames[i]->GetCurState())
						{
							changed = i;
							if (extended[i])
							{
								yMove -= frames[i]->GetExtendedHeight();
							}
							if (!extended[i])
							{
								yMove += frames[i]->GetExtendedHeight();
							}
						}
					}
				}
			}
			yMove = 0;
			if (changed != -1)
			{
				for (int i = 0; i < frames.size(); i++)
				{
					frames[i]->pos.top += yMove;
					frames[i]->pos.bottom += yMove;
					if (frames[i]->IsExtended() && i != changed)
					{
						frames[i]->SetState(0);
						yMove -= frames[i]->GetExtendedHeight();
					}
				}
			}
					/*
					if (changed != -1)
					{
						yMove = 0;
						for (int i = 0; i < frames.size(); i++)
						{
							frames[i]->pos.top += yMove;
							frames[i]->pos.bottom += yMove;
							if (i != changed)
							{
								if (frames[i]->SetState(1) && i < changed)
								{
									yMove -= frames[i]->GetExtendedHeight();
								}
								else if (frames[i]->SetState(1) && i > changed)
								{
									yMove -= frames[i]->GetExtendedHeight();
								}
							}
						}
					}
					*/
			return changed == -1;
		}
	};

	std::vector<std::unique_ptr<Frame>> windows;
	sharedResC resC;
	RectF overallParent = Graphics::GetScreenRect<float>();
public:
	const Frame& operator[](std::size_t idx) const { return *windows[idx].get(); }
	Frame& operator[](std::size_t idx) { return *windows[idx].get();  }

	FrameHandle(sharedResC resC);
	bool HandleMouseInput(Mouse::Event& e);
	void Draw(Graphics& gfx);
	void AddFrame(RectF pos, int type, RectF& parentPos, sharedResC resC);
	MultiFrame* AddMultiFrame(RectF pos, int type, int nStates, RectF& parentPos, sharedResC resC);
	// ###### Init Frames #####
	void InitFrames()
	{
		std::vector<int> a = { 0,1 };

		MultiFrame* m = AddMultiFrame(RectF(Vec2(540, 110), 140, 280), 0, 1, overallParent, resC);
		Frame* f1 = m->AddFrame(Frame(RectF(Vec2(0, 0), 140, 280), 0, m->pos, resC));
		Frame* f2 = m->AddFrame(Frame(RectF(Vec2(0, 12), 140, 280), 0, m->pos, resC));
		Frame* f3 = m->AddFrame(Frame(RectF(Vec2(0, 24), 140, 280), 0, m->pos, resC));
		f1->AddText(Settings::lang_Feldinformationen[Settings::lang], RectF(Vec2(35, 3), 50, 50), 7, resC->tC.fonts[0], Colors::Black);
		f1->AddText(Settings::lang_Flora[Settings::lang] + ":", RectF(Vec2(7, 19), 50, 50), 7, resC->tC.fonts[0], Colors::Black, a);
		f1->AddText(Settings::lang_Flora[Settings::lang] + ":", RectF(Vec2(70, 19), 50, 50), 7, resC->tC.fonts[0], Colors::Black, a);


		/*
		AddFrame(RectF(Vec2(540, 110), 140, 280), 0, 2, overallParent, resC);
		windows[0]->NextState();

		windows[0]->AddSubFrame(RectF(Vec2(0, 12), 140, 280), 0,2,b, resC);
		Frame* mt = static_cast<Frame*>(windows[0]->GetComp(0));
		mt->NextState();

		windows[0]->AddText(Settings::lang_Feldinformationen[Settings::lang], RectF(Vec2(35, 3), 50, 50), 7, resC->tC.fonts[0], Colors::Black);
		windows[0]->AddText(Settings::lang_Flora[Settings::lang] + ":", RectF(Vec2(7, 19), 50, 50), 7, resC->tC.fonts[0], Colors::Black, a);
		windows[0]->AddText(Settings::lang_Flora[Settings::lang] + ":", RectF(Vec2(70, 19), 50, 50), 7, resC->tC.fonts[0], Colors::Black, a);
		*/


	}
	// ### Update components of existing Frames ###

	// ###### Update existing Frames ######
	void UpdateFieldinformation(World& curW)				
	{
		windows[0]->SetText(Settings::GetTypeString(curW.GetfCellType()), 2);
	}


	static constexpr float percentForGrab = 0.05;			// 0 - 1
};



