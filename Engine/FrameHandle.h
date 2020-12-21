#pragma once
#include "World.h"
#include <memory>
#include <map>
#include <algorithm>
class ButtonFunctions;
class PageFrame;

class Component
{
	bool visible = true;
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
	Component(RectF pos, Component* parentC) :pos(pos), parentC(parentC){}
	virtual void Draw(Graphics& gfx)
	{
		gfx.DrawFilledRect(GetPos(), c, SpriteEffect::Nothing());
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact, World& curW)
	{
		if (interact && GetPos().Contains((Vec2)e.GetPos()))
		{
			mouseHovers = true;
			return true;
		}
		mouseHovers = false;
		return false;
	}
	void SetVisible(bool visible)
	{
		this->visible = visible;
		if (!visible)
		{
			mouseHovers = false;
		}
	}
	bool IsVisible()
	{
		return visible;
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
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact, World& curW)override
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
	bool(*bFunc)(PageFrame* pF, World& curW) = nullptr;
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
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact, World& curW)
	{
		Component::HandleMouseInput(e, interact, curW);
		if (GetPos().Contains((Vec2)e.GetPos()) && e.GetType() == Mouse::Event::LRelease && interact)
		{
			if (ppF != nullptr && bFunc != nullptr)
			{
				return bFunc(ppF, curW);
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
	std::map<std::string, std::unique_ptr<Component>> comps;
	Vec2 lastMouseP = Vec2(0, 0);
	Vec2 posOfLastPress = Vec2(-1, -1);
public:
	Frame(RectF pos, int type, sharedResC resC, Component* parentC);


	virtual void DrawComps(Graphics& gfx)
	{
		std::for_each(comps.begin(), comps.end(), [&](auto& comp)		//auto = something like 'std::pair<std::string, std::unique_ptr<Component>>'
		{
			if (comp.second->activInStates[curState] == 1 && comp.second->IsVisible()) {
				comp.second->Draw(gfx);
			}
		});
	}
	virtual void Draw(Graphics& gfx)override {
		if (IsVisible())
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

	bool HandleMouseInput(Mouse::Event& e, bool interact, World& curW)override
	{
		if (IsVisible())
		{
			bool hitComp = false;
			std::for_each(comps.begin(), comps.end(), [&](auto& comp)
			{
				hitComp = comp.second->activInStates[curState] == 1 && comp.second->IsVisible() && comp.second->HandleMouseInput(e, interact && !hitComp, curW) || hitComp;
			});

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

	void SetText(std::string text, std::string key);
	bool SetState(int state) { bool changed = curState == state; curState = state; return changed; }
	void NextState()
	{
		curState++;
		if (curState == nStates)
		{
			curState = 0;
		}
	}
	Component* GetComp(std::string key) {
		std::map<std::string, std::unique_ptr<Component>>::iterator it = comps.find(key);
		if (it != comps.end())
		{
			return it->second.get();
		}
		return nullptr;
	}
	int GetNumberOfComps() { return comps.size(); }
	int GetCurState() { return curState; }
	int GetExtendedHeight();
	bool IsExtended();
	std::vector<int> FillWith1WhenSize0(std::vector<int> activInStates, int nStages);

	virtual Text* AddText(std::string text, RectF pos, int size, Font f, Color c, std::string key, std::vector<int> activInStates = {}, int textLoc = 0) 
	{
		activInStates = FillWith1WhenSize0(activInStates, nStates);
		assert(activInStates.size() == nStates);
		comps[key] = std::make_unique<Text>(text, pos, size, f, c, activInStates, this, textLoc);
		return static_cast<Text*>(comps[key].get());
	}
	virtual Button* AddButton(RectF pos, Animation& a, Animation& aHover, std::string key, std::vector<int> activInStates = {})
	{
		activInStates = FillWith1WhenSize0(activInStates, nStates);
		assert(activInStates.size() == nStates);
		comps[key] = std::make_unique<Button>(pos, a, aHover, activInStates, this);
		return static_cast<Button*>(comps[key].get());
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
	std::map<std::string, std::vector<int>> compActivOnPages;
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

	Text* AddText(std::string text, RectF pos, int size, Font f, Color c, std::string key, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {}, int textLoc = 0)
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages[key] = activOnPages;
		return Frame::AddText(text, pos, size, f, c, key, activInStates, textLoc);
	}
	Button* AddButton(RectF pos, Animation& a, Animation& aHover, std::string key, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {})
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages[key] = activOnPages;
		return Frame::AddButton(pos, a, aHover, key, activInStates);
	}
	void DrawComps(Graphics& gfx) override
	{
		std::for_each(comps.begin(), comps.end(), [&](auto& comp)
		{
			if (comp.second->activInStates[curState] == 1 && comp.second->IsVisible() && compActivOnPages[comp.first][curPage] == 1)
			{
				comp.second->Draw(gfx);
			}
		});
	}
	bool HandleMouseInput(Mouse::Event& e, bool interact, World& curW)override
	{
		if (IsVisible())
		{
			bool hit = Frame::HandleMouseInput(e, interact, curW);
			if (curPage == 0)
			{
				comps["b_left"]->SetVisible(false);
			}
			else
			{
				comps["b_left"]->SetVisible(true);
			}

			if (curPage == nPages - 1)
			{
				comps["b_right"]->SetVisible(false);
			}
			else
			{
				comps["b_right"]->SetVisible(true);
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


bool B1(PageFrame* pF, World& curW);
bool B2(PageFrame* pF, World& curW);
bool B3(PageFrame* pF, World& curW);
bool B4(PageFrame* pF, World& curW);
bool B5(PageFrame* pF, World& curW);

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
	bool HandleMouseInput(Mouse::Event& e, bool interact, World& curW)override
	{
		bool hit = Frame::HandleMouseInput(e, interact, curW);
		std::vector<bool> extended;
		for (int i = 0; i < frames.size(); i++)
		{
			extended.push_back(frames[i]->GetCurState());
		}
		int changed = -1;
		int yMove = 0;
		for (int n = 0; n < frames.size(); n++)
		{
			bool hitFrame = frames[n]->HandleMouseInput(e, interact, curW);
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
	bool HandleMouseInput(Mouse::Event& e, World& curW);
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
    	f1->AddText(Settings::lang_fieldInformation[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, resC->tC.fonts[0], Colors::Black,"h_f1");

		f1->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(60, 19), 50, 8), 7, resC->tC.fonts[0], Colors::Black,"t_cellType", a, 1);
		f1->AddText(Settings::lang_flora[Settings::lang] + ":", RectF(Vec2(2, 19), 50, 8), 7, resC->tC.fonts[0], Colors::Black, "t_flora", a, 1);
		f1->AddText(Settings::lang_Obstacle[Settings::lang] + ":", RectF(Vec2(2, 35), 50, 8), 7, resC->tC.fonts[0], Colors::Black, "t_obstacle", a, 1);
		f1->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(60, 35), 50, 8), 7, resC->tC.fonts[0], Colors::Black, "t_obstacleInfo", a, 1);

		// #2
		p2->AddText(Settings::lang_buildmenu[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, resC->tC.fonts[0], Colors::Black, "h_f2");

		p2->AddText(Settings::lang_housing[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_housing", { 0,1 }, { 1, 0, 0, 0 });
		p2->AddText(Settings::lang_productions[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_productions", { 0,1 }, { 0, 1, 0, 0 });
		p2->AddText(Settings::lang_decoration[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_decoration", { 0,1 }, { 0, 0, 1, 0 });
		p2->AddText(Settings::lang_agility[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_agility", { 0,1 }, { 0, 0, 0, 1 });

		p2->AddText(Settings::lang_tent[Settings::lang], RectF(Vec2(5, 40), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_tent", { 0,1 }, { 1, 0, 0, 0 }, 1);
		Button* b3 = p2->AddButton(RectF(Vec2(99, 40), 34, 9), resC->tC.buttons[2], resC->tC.buttons[3], "b_buildTent", a, { 1,0,0,0 });
		b3->bFunc = B3;

		p2->AddText(Settings::lang_bonfire[Settings::lang], RectF(Vec2(5, 50), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_bonfire", { 0,1 }, { 1, 0, 0, 0 }, 1);
		Button* b4 = p2->AddButton(RectF(Vec2(99, 50), 34, 9), resC->tC.buttons[2], resC->tC.buttons[3], "b_buildBonfire", a, { 1,0,0,0 });
		b4->bFunc = B4;

		p2->AddText(Settings::lang_townhall[Settings::lang], RectF(Vec2(5, 60), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_townhall", { 0,1 }, { 1, 0, 0, 0 }, 1);
		Button* b5 = p2->AddButton(RectF(Vec2(99, 60), 34, 9), resC->tC.buttons[2], resC->tC.buttons[3], "b_buildTownhall", a, { 1,0,0,0 });
		b5->bFunc = B5;
		// #3
		// Page 1
		p3->AddText(Settings::lang_constructionMaterials[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, resC->tC.fonts[0], Colors::Black, "h_f3");

		p3->AddText(Settings::lang_resources[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "sh_resourcen", { 0,1 }, { 1, 0, 0 });
		p3->AddText(Settings::lang_materials[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "sh_materials", { 0,1 }, { 0, 1, 0 });
		p3->AddText(Settings::lang_organic[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "sh_organic", { 0,1 }, { 0, 0, 1 });

		p3->AddText(Settings::lang_wood[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_wood", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_iron[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_iron", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_sand[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_sand", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_stone[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_stone", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_copper[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_copper", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_gold[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_gold", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_aluminum[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_aluminum", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_emerald[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_emerald", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_sapphire[Settings::lang] + ":", RectF(Vec2(5, 130), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_sapphire", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_robin[Settings::lang] + ":", RectF(Vec2(5, 140), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_robin", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_diamond[Settings::lang] + ":", RectF(Vec2(5, 150), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_dimond", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_amber[Settings::lang] + ":", RectF(Vec2(5, 160), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_amber", { 0,1 }, { 1, 0, 0 }, 1);

		p3->AddText(Settings::lang_kilogram[Settings::lang], RectF(Vec2(100, 40), 50, 10), 7, resC->tC.fonts[0], Colors::Black,"t_kilogram", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("11", RectF(Vec2(100, 50), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nWood", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 60), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nIron", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 70), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nSand", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("44", RectF(Vec2(100, 80), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nStone", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 90), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nCopper", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("24", RectF(Vec2(100, 100), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nGold", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("3", RectF(Vec2(100, 110), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nAluminum", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("4", RectF(Vec2(100, 120), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nEmerald", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 130), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nSapphire", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("4", RectF(Vec2(100, 140), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nRobin", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("24", RectF(Vec2(100, 150), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nDimond", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("24", RectF(Vec2(100, 160), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nAmber", { 0,1 }, { 1, 0, 0 }, 1);

		//Page 2
		p3->AddText(Settings::lang_steel[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_steel", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_plastic[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_plastic", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_concrete[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_concrete", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_glass[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_glass", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_ceramics[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_ceramics", { 0,1 }, { 0, 1, 0 }, 1);

		p3->AddText("11", RectF(Vec2(100, 50), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nSteel", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 60), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nPlastic", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 70), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nConcrete", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("44", RectF(Vec2(100, 80), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nGlass", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 90), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nCeramics", { 0,1 }, { 0, 1, 0 }, 1);

		//Page 3
		p3->AddText(Settings::lang_corals[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_corals", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_sticks[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_sticks", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_leaves[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_leaves", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_wool[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_wool", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_leather[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_leather", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_fur[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_fur", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_meat[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_meat", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_fish[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_fish", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_berrys[Settings::lang] + ":", RectF(Vec2(5, 130), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_berrys", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_apples[Settings::lang] + ":", RectF(Vec2(5, 140), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_apples", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_cactus[Settings::lang] + ":", RectF(Vec2(5, 150), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_cactus", { 0,1 }, { 0, 0, 1 }, 1);

		p3->AddText("11", RectF(Vec2(100, 50), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nCorals", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("2", RectF(Vec2(100, 60), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nSticks", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("2", RectF(Vec2(100, 70), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nLeaves", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("44", RectF(Vec2(100, 80), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nWool", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("2", RectF(Vec2(100, 90), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nLether", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("24", RectF(Vec2(100, 100), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nFur", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("3", RectF(Vec2(100, 110), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nMeat", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("4", RectF(Vec2(100, 120), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nFish", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("2", RectF(Vec2(100, 130), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nBerrys", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("4", RectF(Vec2(100, 140), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nApples", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("24", RectF(Vec2(100, 150), 50, 10), 7, resC->tC.fonts[0], Colors::Black, "t_nCactus", { 0,1 }, { 0, 0, 1 }, 1);

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
		f1->SetText(Settings::GetTypeString(curW.GetfCellType()), "t_cellType");
		if(curW.GetFocusedObstacle() != nullptr)
		{
			f1->SetText(Settings::GetObstacleString(curW.GetFocusedObstacle()->type), "t_obstacleInfo");
		}

		//#2
		PageFrame* p2 = static_cast<PageFrame*>(m->GetFrame(1));

		//#3
		PageFrame* p3 = static_cast<PageFrame*>(m->GetFrame(2));

		// Page 1
		p3->SetText(std::to_string(playerM.wood), "t_nWood");
		p3->SetText(std::to_string(playerM.iron), "t_nIron");
		p3->SetText(std::to_string(playerM.sand), "t_nSand");
		p3->SetText(std::to_string(playerM.stone), "t_nStone");
		p3->SetText(std::to_string(playerM.copper), "t_nCopper");
		p3->SetText(std::to_string(playerM.gold), "t_nGold");
		p3->SetText(std::to_string(playerM.aluminum), "t_nAluminum");
		p3->SetText(std::to_string(playerM.emerald), "t_nEmerald");
		p3->SetText(std::to_string(playerM.sapphire), "t_nSapphire");
		p3->SetText(std::to_string(playerM.robin), "t_nRobin");
		p3->SetText(std::to_string(playerM.diamond), "t_nDimond");
		p3->SetText(std::to_string(playerM.amber), "t_nAmber");
		
		//Page 2
		p3->SetText(std::to_string(playerM.steel), "t_nSteel");
		p3->SetText(std::to_string(playerM.plastic), "t_nPlastic");
		p3->SetText(std::to_string(playerM.concrete), "t_nConcrete");
		p3->SetText(std::to_string(playerM.glass), "t_nGlass");
		p3->SetText(std::to_string(playerM.ceramics), "t_nCeramics");

		//Page 3
		p3->SetText(std::to_string(playerM.corals), "t_nCorals");
		p3->SetText(std::to_string(playerM.sticks), "t_nSticks");
		p3->SetText(std::to_string(playerM.leaves), "t_nLeaves");
		p3->SetText(std::to_string(playerM.wool), "t_nWool");
		p3->SetText(std::to_string(playerM.leather), "t_nLeather");
		p3->SetText(std::to_string(playerM.fur), "t_nFur");
		p3->SetText(std::to_string(playerM.meat), "t_nMeat");
		p3->SetText(std::to_string(playerM.fish), "t_nFish");
		p3->SetText(std::to_string(playerM.berrys), "t_nBerrys");
		p3->SetText(std::to_string(playerM.apples), "t_nApples");
		p3->SetText(std::to_string(playerM.cactus), "t_nCactus");
	}

	static constexpr float percentForGrab = 0.05;			
};

