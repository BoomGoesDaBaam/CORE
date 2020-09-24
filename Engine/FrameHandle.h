#pragma once
#include "World.h"
#include <memory>


class ButtonFunctions;
class PageFrame;

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
		gfx.DrawFilledRect(GetPos(), c, SpriteEffect::Nothing());
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
	int size, textLoc;			//		'0' = centered			 '1' = left
	Text(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc);
	void Draw(Graphics& gfx) override
	{
		if (textLoc == 0)
		{
			RectF drawPos = parentC->GetPos() + pos.GetCenter();
			f.DrawTextCentered(text, drawPos.GetTopLeft<int>(), size, c);
		}
		else if (textLoc == 1)
		{
			RectF drawPos = parentC->GetPos() + pos.GetTopLeft<float>();
			f.DrawText(text, drawPos.left, drawPos.top, size, c);
		}
		//gfx.DrawRect(GetPos(), Colors::Red);
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)override
	{
		if (interact && GetPos().Contains((Vec2)e.GetPos()))
		{
			mouseHovers = true;
			return false;
		}
		mouseHovers = false;
		return false;
	}
};
class Button : public Component
{
	Animation& a;
	Animation& aHover;
public:
	bool(*bFunc)(PageFrame* pF) = nullptr;
	PageFrame* ppF = nullptr;
	//Frame* pF = nullptr;

	Button(RectF pos, Animation& a, Animation& aHover, std::vector<int> activInStates, Component* parentC); 
	void Draw(Graphics& gfx) override
	{
		if (mouseHovers)
		{
			gfx.DrawSurface((RectI)GetPos(), aHover.GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.6f));
		}
		else
		{
			gfx.DrawSurface((RectI)GetPos(), a.GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta,0.6));
		}
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		Component::HandleMouseInput(e, interact);
		if (GetPos().Contains((Vec2)e.GetPos()) && e.GetType() == Mouse::Event::LRelease && interact)
		{
			if (ppF != nullptr && bFunc != nullptr)
			{
				return bFunc(ppF);
			}
			else if (ppF != nullptr)
			{
				return true;
			}
		}
		return false;
	}
};

class Frame : public Component
{
protected:
	sharedResC resC;
	bool grabbed = false;
	int type, curState = 0, nStates = 1;								//'curState' = 0 is minimized in every type
	std::vector<std::unique_ptr<Component>> comps;
	Vec2 lastMouseP = Vec2(0, 0);
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
						gfx.DrawSurface((RectI)cPos, resC->tC.windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.8f));
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
			Vei2 delta((posOfLastPress - mP).GetAbsVec());
			if (nStates > 1 && hit && e.GetType() == Mouse::Event::LRelease && delta.GetLength() < 15)
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
		return comps[index].get();
	}
	int GetNumberOfComps() { return comps.size(); }
	int GetCurState() { return curState; }
	int GetExtendedHeight();
	bool IsExtended();
	std::vector<int> FillWith1WhenSize0(std::vector<int> activInStates, int nStages);

	virtual Text* AddText(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates = {}, int textLoc = 0)
	{
		activInStates = FillWith1WhenSize0(activInStates, nStates);
		assert(activInStates.size() == nStates);
		comps.push_back(std::make_unique<Text>(text, pos, size, f, c, activInStates, this, textLoc));
		return static_cast<Text*>(comps[comps.size()-1].get());
	}
	virtual Button* AddButton(RectF pos, Animation& a, Animation& aHover, std::vector<int> activInStates = {})
	{
		activInStates = FillWith1WhenSize0(activInStates, nStates);
		assert(activInStates.size() == nStates);
		comps.push_back(std::make_unique<Button>(pos, a, aHover, activInStates, this));
		return static_cast<Button*>(comps[comps.size() - 1].get());
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
	std::vector<std::vector<int>> compActivOnPages;
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

	Text* AddText(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {}, int textLoc = 0)
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages.push_back(activOnPages);
		return Frame::AddText(text, pos, size, f, c, activInStates, textLoc);
	}
	Button* AddButton(RectF pos, Animation& a, Animation& aHover, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {})
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages.push_back(activOnPages);
		return Frame::AddButton(pos, a, aHover, activInStates);
	}
	void DrawComps(Graphics& gfx) override
	{
		for (int i = 0; i < comps.size(); i++)
		{
			if (comps[i]->activInStates[curState] == 1 && compActivOnPages[i][curPage] == 1 && comps[i]->visible) {
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
			else
			{
				comps[1]->visible = true;
			}
			return hit;
		}
		return false;
	}

	void NextPage()
	{
		if (curPage + 1 < nPages)
		{
			curPage ++;
		}
	}
	void PriviousPage()
	{
		if (curPage - 1 >= 0)
		{
			curPage--;
		}
	}
};


bool B1(PageFrame* pF);
bool B2(PageFrame* pF);
//bool B3(PageFrame* pF, World& curW);

class MultiFrame : public Frame
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
class FrameHandle
{
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

		MultiFrame* m = AddMultiFrame(RectF(Vec2(540, 110), 140, 280), 0, 1, resC);			//Size of frames is hardcoded
		
		Frame* f1 = m->AddFrame(RectF(Vec2(0, 0), 140, 280), 0, resC, m);
		PageFrame* p2 = m->AddPageFrame(RectF(Vec2(0, 12), 140, 280), 0, resC, m, 4);
		PageFrame* p3 = m->AddPageFrame(RectF(Vec2(0, 24), 140, 280), 0, resC, m, 3);
		
		// #1
/* 0*/	f1->AddText(Settings::lang_fieldInformation[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, resC->tC.fonts[0], Colors::Black);
	
/* 1*/	f1->AddText(Settings::lang_noInformations[Settings::lang], RectF(Vec2(20, 19), 50, 8), 7, resC->tC.fonts[0], Colors::Black, a);
		
		// #2
/* 0*/	p2->AddText(Settings::lang_buildmenu[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, resC->tC.fonts[0], Colors::Black);

/* 1*/	p2->AddText(Settings::lang_housing[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0, 0 });
/* 2*/	p2->AddText(Settings::lang_productions[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 0, 1, 0, 0 });
/* 3*/	p2->AddText(Settings::lang_decoration[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 0, 0, 1, 0 });
/* 4*/	p2->AddText(Settings::lang_agility[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 0, 0, 0, 1 });
		
/* 5*/	p2->AddText(Settings::lang_tent[Settings::lang], RectF(Vec2(5, 40), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0, 0 }, 1);
/* 6*/	//Button* b3 = p2->AddButton(RectF(Vec2(99, 40), 34, 9), resC->tC.buttons[2], resC->tC.buttons[3], a, { 1,0,0,0 });
		//b3->bFunc = B3;

		// #3
/* 0*/	p3->AddText(Settings::lang_constructionMaterials[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, resC->tC.fonts[0], Colors::Black);
		
/* 1*/	p3->AddText(Settings::lang_resources[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0});
/* 2*/	p3->AddText(Settings::lang_materials[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 0, 1, 0});
/* 3*/	p3->AddText(Settings::lang_organic[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 0, 0, 1 });

/* 4*/	p3->AddText(Settings::lang_wood[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/* 5*/	p3->AddText(Settings::lang_iron[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/* 6*/	p3->AddText(Settings::lang_sand[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/* 7*/	p3->AddText(Settings::lang_stone[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/* 8*/	p3->AddText(Settings::lang_copper[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/* 9*/	p3->AddText(Settings::lang_gold[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*10*/	p3->AddText(Settings::lang_aluminum[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*11*/	p3->AddText(Settings::lang_emerald[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*12*/	p3->AddText(Settings::lang_sapphire[Settings::lang] + ":", RectF(Vec2(5, 130), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*13*/	p3->AddText(Settings::lang_robin[Settings::lang] + ":", RectF(Vec2(5, 140), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*14*/	p3->AddText(Settings::lang_diamond[Settings::lang] + ":", RectF(Vec2(5, 150), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*15*/	p3->AddText(Settings::lang_amber[Settings::lang] + ":", RectF(Vec2(5, 160), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);

/*16*/	p3->AddText(Settings::lang_kilogram[Settings::lang], RectF(Vec2(100, 40), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*17*/	p3->AddText("11", RectF(Vec2(100, 50), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*18*/	p3->AddText("2", RectF(Vec2(100, 60), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*19*/	p3->AddText("2", RectF(Vec2(100, 70), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*20*/	p3->AddText("44", RectF(Vec2(100, 80), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*21*/	p3->AddText("2", RectF(Vec2(100, 90), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*22*/	p3->AddText("24", RectF(Vec2(100, 100), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*23*/	p3->AddText("3", RectF(Vec2(100, 110), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*24*/	p3->AddText("4", RectF(Vec2(100, 120), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*25*/	p3->AddText("2", RectF(Vec2(100, 130), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*26*/	p3->AddText("4", RectF(Vec2(100, 140), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*27*/	p3->AddText("24", RectF(Vec2(100, 150), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);
/*28*/	p3->AddText("24", RectF(Vec2(100, 160), 50, 10), 7, resC->tC.fonts[0], Colors::Black, { 0,1 }, { 1, 0, 0 }, 1);


	}
	// ### Update components of existing Frames ###

	// ###### Update existing Frames ######
	void UpdateFieldinformation(World& curW)				
	{
		MultiFrame* m = static_cast<MultiFrame*>(windows[0].get());
		Team& player = curW.GetPlayer();
		Materials& playerM = player.GetMaterials();
		//#1
		Frame* f1 = static_cast<Frame*>(m->GetFrame(0));			
		f1->SetText(Settings::GetTypeString(curW.GetfCellType()), 1);
		//#2
		PageFrame* p2 = static_cast<PageFrame*>(m->GetFrame(1));
		//#3
		PageFrame* p3 = static_cast<PageFrame*>(m->GetFrame(2));
		p3->SetText(std::to_string(playerM.wood), 19);
		p3->SetText(std::to_string(playerM.iron), 20);
		p3->SetText(std::to_string(playerM.sand), 21);
		p3->SetText(std::to_string(playerM.stone), 22);
		p3->SetText(std::to_string(playerM.copper), 23);
		p3->SetText(std::to_string(playerM.gold), 24);
		p3->SetText(std::to_string(playerM.aluminum), 25);
		p3->SetText(std::to_string(playerM.emerald), 26);
		p3->SetText(std::to_string(playerM.sapphire), 27);
		p3->SetText(std::to_string(playerM.robin), 28); 
		p3->SetText(std::to_string(playerM.diamond), 29);
		p3->SetText(std::to_string(playerM.amber), 30);
	}

	static constexpr float percentForGrab = 0.05;			
};

