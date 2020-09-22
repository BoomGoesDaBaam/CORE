#pragma once
#include "World.h"
#include <memory>


class ButtonFunctions;


class Component
{
public:
	Component* parentC;
	RectF GetPos() {
		if (parentC == nullptr)
		{
			return pos;
		}
		return pos + parentC->GetPos().GetTopLeft<float>(); }
	RectF pos;
	Color c = Colors::Black;
	std::vector<int> activInStates;
	std::string text = "no title";
	bool mouseHovers = false;
	bool visible = true;

	Component(RectF pos, Component* parentC) :pos(pos), parentC(parentC){}
	virtual void Draw(Graphics& gfx)
	{
		gfx.DrawRect(GetPos(), c, SpriteEffect::Nothing());
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		if (interact && GetPos().Contains((Vec2)e.GetPos()))
		{
			mouseHovers = true;
			return true;
		}
		mouseHovers = false;
		return false;
	}
};
class Text: public Component
{
public:
	Font f;
	int size;
	Text(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates, Component* parentC);
	void Draw(Graphics& gfx) override
	{
		f.DrawText(text, GetPos().left, GetPos().top, size, c);
	}
};
class Button : public Component
{
	Animation& a;
	Animation& aHover;
public:
	Button(RectF pos, Animation& a, Animation& aHover, std::vector<int> activInStates, Component* parentC); 
	void Draw(Graphics& gfx) override
	{
		auto k = []() {};
		if (mouseHovers)
		{
			gfx.DrawSurface((RectI)GetPos(), aHover.GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.6f));
		}
		else
		{
			gfx.DrawSurface((RectI)GetPos(), a.GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta));
		}
	}
};


class FrameHandle
{
public:
	class Frame: public Component
	{
	protected:
		sharedResC resC;
		bool grabbed = false;
		int type,curState=0, nStates=1;								//'curState' = 0 is minimized in every type
		std::vector<std::unique_ptr<Component>> comps;
		Vec2 lastMouseP = Vec2(0,0);
		Vec2 posOfLastPress = Vec2(-1, -1);
	public:
		Frame(RectF pos, int type, sharedResC resC, Component* parentC);
		
		
		virtual void DrawComps(Graphics& gfx)
		{
			for (int i = 0; i < comps.size(); i++)
			{
				if (comps[i]->activInStates[curState] == 1 && comps[i]->visible) {
					comps[i]->Draw(gfx);
				}
			}
		}
		virtual void Draw(Graphics& gfx)override {
			if (visible)
			{
				assert(type == 0 || type == -1);
				RectF cPos = GetPos();
				if (mouseHovers)
				{
					switch (type)
					{
					case 0:
						switch (curState)
						{
						case 0:
							gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
							break;
						case 1:
							gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
							gfx.DrawSurface((RectI)cPos, resC->tC.windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.9f));
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
							gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
							break;
						case 1:
							gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), pos.GetWidth(), pos.GetHeight() / 20), resC->tC.windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
							gfx.DrawSurface((RectI)cPos, resC->tC.windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.75f));
							break;
						}
						break;
					}
				}
				DrawComps(gfx);
			}
		}
		
		bool HandleMouseInput(Mouse::Event& e, bool interact)override
		{
			if (visible)
			{
				bool hitComp = false;
				for (int i = 0; i < comps.size(); i++)
				{
					hitComp = comps[i]->activInStates[curState] == 1 && comps[i]->visible && comps[i]->HandleMouseInput(e, interact && !hitComp) || hitComp;
				}

				if (hitComp && interact)
				{
					return true;
				}
				if (hitComp)
				{
					return false;
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
					if (type != -1)
					{
						return true;
					}
				}
				else
				{
					mouseHovers = false;
				}
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
		Component* GetComp(int index) {
			assert(index >= 0 && index < comps.size());
			return comps[index].get(); }
		int GetNumberOfComps() { return comps.size(); }
		int GetCurState() { return curState; }
		int GetExtendedHeight();
		bool IsExtended();
		std::vector<int> GetActivInVector(int val, int count);

		virtual void AddText(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates = {})
		{
			if (activInStates.size() == 0)
			{
				activInStates = GetActivInVector(1, nStates);
			}
			comps.push_back(std::make_unique<Text>(text, pos, size, f, c, activInStates, this));
		}
		virtual void AddButton(RectF pos, Animation& a, Animation& aHover, std::vector<int> activInStates = {})
		{
			if (activInStates.size() == 0)
			{
				activInStates = GetActivInVector(1, nStates);
			}
			comps.push_back(std::make_unique<Button>(pos, a, aHover, activInStates, this));
		}

		
		void Grab(Vec2 mP);
		void Move(Vec2 mP);
		void Release();
	};

	template <typename T>
	struct fail : std::false_type
	{
	};

	class PageFrame : public Frame
	{
		int nPages = 1;
		int curPage = 0;
		std::vector<int> compBelongsToPage;
	public:
		PageFrame(RectF pos, int type, sharedResC resC, Component* parentC, int nPages);

		template<typename T = bool>
		void AddText(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates = {})
		{
			static_assert (fail<T>::value, "Do not use!");
		}
		template<typename T = bool>
		void AddButton(RectF pos, Animation& a, Animation& aHover, std::vector<int> activInStates = {})
		{
			static_assert (fail<T>::value, "Do not use!");
		}

		void AddText(std::string text, RectF pos, int size, Font f, Color c, int onPage, std::vector<int> activInStates = {})
		{
			Frame::AddText(text, pos, size, f, c, activInStates);
			compBelongsToPage.push_back(onPage);
		}
		void AddButton(RectF pos, Animation& a, Animation& aHover, int onPage, std::vector<int> activInStates = {})
		{
			Frame::AddButton(pos, a, aHover, activInStates);
			compBelongsToPage.push_back(onPage);
		}
		void DrawComps(Graphics& gfx) override
		{
			for (int i = 0; i < comps.size(); i++)
			{
				if (comps[i]->activInStates[curState] == 1 && compBelongsToPage[i] == curPage && comps[i]->visible) {
					comps[i]->Draw(gfx);
				}
			}
		}
	
		bool HandleMouseInput(Mouse::Event& e, bool interact)override
		{
			if (visible)
			{
				bool hit = Frame::HandleMouseInput(e, interact);
				if (curPage == 0)
				{
					comps[0]->visible = false;
				}
				else
				{
					comps[0]->visible = true;
				}

				if (curPage == nPages - 1)
				{
					comps[1]->visible = false;
				}
				else if (curPage > 0)
				{
					comps[1]->visible = true;
				}
				return hit;
			}
			return false;
		}
	};

	class MultiFrame: public Frame	
	{
		std::vector<std::unique_ptr<Frame>> frames;
	public:
		MultiFrame(RectF pos, sharedResC resC, Component* parentC) :Frame(pos, -1, resC, parentC) {}
		
		Frame* AddFrame(RectF pos, int type, sharedResC resC, Component* parentC);
		PageFrame* AddPageFrame(RectF pos, int type, sharedResC resC, Component* parentC, int nPages);
		
		void Draw(Graphics& gfx)override 
		{
			for (int i = 0; i < frames.size(); i++)
			{
				frames[i]->Draw(gfx);
			}
		}
		bool HandleMouseInput(Mouse::Event& e, bool interact)override
		{
			bool hit = Frame::HandleMouseInput(e, interact);
			std::vector<bool> extended;
			for (int i = 0; i < frames.size(); i++)
			{
				extended.push_back(frames[i]->GetCurState());
			}
			int changed = -1;
			int yMove = 0;
			for (int n = 0; n < frames.size(); n++)
			{
				bool hitFrame = frames[n]->HandleMouseInput(e, interact);
				hit = hit || hitFrame;
				if (hitFrame)
				{
					for (int i = 0; i < frames.size(); i++)
					{
						frames[i]->pos.top += yMove;
						frames[i]->pos.bottom += yMove;
						if (extended[i] != frames[i]->GetCurState())
						{
							changed = i;
							n = frames.size();
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
			if (hit)
			{
				return true;
			}
			return changed != -1;
		}
		Frame* GetFrame(int index)
		{
			assert(index >= 0 && index < frames.size());
			return frames[index].get();
		}
	};

private:
	std::vector<std::unique_ptr<Frame>> windows;
	sharedResC resC;
	RectF overallParent = Graphics::GetScreenRect<float>();
public:
	const Frame& operator[](std::size_t idx) const { return *windows[idx].get(); }
	Frame& operator[](std::size_t idx) { return *windows[idx].get();  }

	FrameHandle(sharedResC resC);
	bool HandleMouseInput(Mouse::Event& e);
	void Draw(Graphics& gfx);
	
	void AddFrame(RectF pos, int type, sharedResC resC);
	MultiFrame* AddMultiFrame(RectF pos, int type, int nStates, sharedResC resC);
	
	// ###### Init Frames #####
	void InitFrames()
	{
		std::vector<int> a = { 0,1 };

		MultiFrame* m = AddMultiFrame(RectF(Vec2(540, 110), 140, 280), 0, 1, resC);
		
		Frame* f1 = m->AddFrame(RectF(Vec2(0, 0), 140, 280), 0, resC, m);
		PageFrame* p2 = m->AddPageFrame(RectF(Vec2(0, 12), 140, 280), 0, resC, m, 2);
		Frame* f3 = m->AddFrame(RectF(Vec2(0, 24), 140, 280), 0, resC, m);
		
		f1->AddText(Settings::lang_fieldInformation[Settings::lang], RectF(Vec2(35, 3), 50, 50), 7, resC->tC.fonts[0], Colors::Black);
		f1->AddText(Settings::lang_noInformations[Settings::lang], RectF(Vec2(7, 19), 50, 50), 7, resC->tC.fonts[0], Colors::Black, a);
		
		p2->AddText(Settings::lang_buildmenu[Settings::lang], RectF(Vec2(35, 3), 50, 10), 7, resC->tC.fonts[0], Colors::Black, 0);
		p2->AddText(Settings::lang_buildings[Settings::lang], RectF(Vec2(42, 19), 50, 10), 7, resC->tC.fonts[0], Colors::Black, 0, a);
		
		

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
		MultiFrame* m = static_cast<MultiFrame*>(windows[0].get());				
		Frame* f1 = static_cast<Frame*>(m->GetFrame(0));			
		f1->SetText(Settings::GetTypeString(curW.GetfCellType()), 1);
	}


	static constexpr float percentForGrab = 0.05;			
};



class ButFunc
{
public:
	class B1
	{
	public:
		void operator()(FrameHandle::PageFrame* c) {

		}
	};

};