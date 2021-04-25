#pragma once
#include "World.h"
#include <memory>
#include <map>
#include <algorithm>
class ButtonFunctions;
class PageFrame;
class FrameHandle;
class Component;

class FrameEvent
{
public:
	enum Type
	{
		ButtonPressed,
		Invalid
	};
private:
	Type type;
	int extra = -1;
	std::string action = "";
	Component* caller = nullptr;
public:
	FrameEvent()
		:
		type(Type::Invalid)
	{}
	FrameEvent(Type type, std::string action, int extra, Component* caller)
		:
		type(type),
		action(action),
		extra(extra),
		caller(caller)
	{
	}
	bool IsValid() const
	{
		return type != Invalid;
	}
	Type GetType() const
	{
		return type;
	}
	Component* GetCaller()
	{
		return caller;
	}
	std::string GetAction()
	{
		return action;
	}
	int GetExtra()
	{
		return extra;
	}
};

class Component
{
	bool visible = true;
protected:
	std::queue<FrameEvent>* buffer;
	std::map<std::string, std::unique_ptr<Component>> comps;
	virtual bool HandleMouseInputFrame(Mouse::Event& e, bool interact)
	{
		if (interact && GetPos().Contains((Vec2)e.GetPos()))
		{
			mouseHovers = true;
			return true;
		}
		mouseHovers = false;
		return false;
	}
	virtual bool HandleMouseInputComps(Mouse::Event& e, bool interact)
	{
		bool hitComp = false;
		std::for_each(comps.begin(), comps.end(), [&](auto& comp)
			{
				hitComp = comp.second->IsVisible() && comp.second->HandleMouseInput(e, interact && !hitComp) || hitComp;
			});

		if (hitComp && interact)
		{
			return true;
		}
		return false;
	}
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
	bool mouseHovers = false, hitable = false;

	Component(RectF pos, Component* parentC, std::queue<FrameEvent>* buffer, int type = 0) :pos(pos), parentC(parentC),buffer(buffer){}
	virtual void Draw(Graphics& gfx)
	{
		gfx.DrawFilledRect(GetPos(), c, SpriteEffect::Nothing());
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		if (IsVisible())
		{
			if (HandleMouseInputComps(e, interact))
			{
				return true;
			}
			return HandleMouseInputFrame(e, interact);
		}
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
	Font* f;
	int size, textLoc;			//		'0' = centered			 '1' = left
	Text(std::string text, RectF pos, int size, Font* f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc, std::queue<FrameEvent>* buffer);
	void Draw(Graphics& gfx) override
	{
		if (textLoc == 0)
		{
			RectF drawPos = parentC->GetPos() + pos.GetCenter();
			f->DrawTextCentered(text, drawPos.GetTopLeft<int>(), size, c);
		}
		else if (textLoc == 1)
		{
			RectF drawPos = parentC->GetPos() + pos.GetTopLeft<float>();
			f->DrawText(text, (int)(drawPos.left), (int)(drawPos.top), size, c);
		}
		//gfx.DrawRect(GetPos(), Colors::Red);
	}
};
class Button : public Component
{
protected:
	Animation* a = nullptr;
	Animation* aHover = nullptr;

	virtual bool HandleMouseInputFrame(Mouse::Event& e, bool interact)override
	{
		Component::HandleMouseInputFrame(e, interact);
		if (GetPos().Contains((Vec2)e.GetPos()) && e.GetType() == Mouse::Event::LRelease && interact)
		{
			if (bFunc != nullptr)
			{
				return bFunc(buffer, this);
			}
			else if (ppF != nullptr)
			{
				return true;
			}
		}
		return false;
	}
public:
	bool(*bFunc)(std::queue<FrameEvent>* buffer, Component* caller) = nullptr;
	PageFrame* ppF = nullptr;
	//Frame* pF = nullptr;

	Button(RectF pos, Animation* a, Animation* aHover, std::vector<int> activInStates, Component* parentC, std::queue<FrameEvent>* buffer);
	void Draw(Graphics& gfx) override
	{
		if (mouseHovers)
		{
			gfx.DrawSurface((RectI)GetPos(), aHover->GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.6f));
		}
		else
		{
			gfx.DrawSurface((RectI)GetPos(), a->GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta,0.6));
		}
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		if (IsVisible())
		{
			if (HandleMouseInputComps(e, interact))
			{
				return true;
			}
			return HandleMouseInputFrame(e, interact);
		}
		return false;
	}
};
class Frame : public Component
{
protected:
	sharedResC resC;
	bool grabbed = false;
	bool moveable = true;
	Vec2 lastMouseP = Vec2(0, 0);
	Vec2 posOfLastPress = Vec2(-1, -1);
	int type = 0, curState = 0, nStates = 1;
public:
	const Surface* s = nullptr;
	Frame(RectF pos, int type, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer);
	bool(*bFunc)(std::queue<FrameEvent>* buffer, Component* caller) = nullptr;

	void SetMoveable(bool moveable)
	{
		this->moveable = moveable;
	}

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
			assert(type == 0 || type == 1 || type == -1);
			RectF cPos = GetPos();
			if (mouseHovers)
			{
				switch (type)
				{
				case 0:
					switch (curState)
					{
					case 0:
						gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), (int)(pos.GetWidth()), (int)(pos.GetHeight() / 20)), resC->tC.windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						break;
					case 1:
						gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), (int)(pos.GetWidth()), (int)(pos.GetHeight() / 20)), resC->tC.windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						gfx.DrawSurface((RectI)cPos, resC->tC.windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.9f));
						break;
					}
					break;
				case 1:
					gfx.DrawSurface(RectI(pos), *s, SpriteEffect::Chroma(Colors::Magenta));
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
						gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), (int)(pos.GetWidth()), (int)(pos.GetHeight() / 20)), resC->tC.windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						break;
					case 1:
						gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), (int)(pos.GetWidth()), (int)(pos.GetHeight() / 20)), resC->tC.windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						gfx.DrawSurface((RectI)cPos, resC->tC.windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.8f));
						break;
					}
					break;
				case 1:
					gfx.DrawSurface(RectI(pos), *s, SpriteEffect::Chroma(Colors::Magenta));
					break;
				}
			}
			DrawComps(gfx);
		}
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		if (IsVisible())
		{
			if (HandleMouseInputComps(e, interact))
			{
				return true;
			}
			return HandleMouseInputFrame(e, interact);
		}
		return false;
	}
	virtual bool HandleMouseInputFrame(Mouse::Event& e, bool interact)
	{
			Vec2 mP = (Vec2)e.GetPos();
			bool hit = Hit(mP);

			if (grabbed && moveable)
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
			if (e.GetType() == Mouse::Event::LPress && hit && moveable)
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
				if (bFunc != nullptr && e.GetType() == Mouse::Event::LRelease)
				{
					bFunc(buffer, this);
				}
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
		return false;
	}
	virtual bool HandleMouseInputComps(Mouse::Event& e, bool interact)
	{
		bool hitComp = false;
		std::for_each(comps.begin(), comps.end(), [&](auto& comp)
			{
				hitComp = comp.second->hitable && comp.second->activInStates[curState] == 1 && comp.second->IsVisible() && comp.second->HandleMouseInput(e, interact && !hitComp) || hitComp;
			});

		if (hitComp && interact)
		{
			return true;
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

	virtual Text* AddText(std::string text, RectF pos, int size, Font* f, Color c, std::string key, std::vector<int> activInStates = {}, int textLoc = 0) 
	{
		activInStates = FillWith1WhenSize0(activInStates, nStates);
		//assert(activInStates.size() == nStates);
		comps[key] = std::make_unique<Text>(text, pos, size, f, c, activInStates, this, textLoc, buffer);
		return static_cast<Text*>(comps[key].get());
	}
	virtual Button* AddButton(RectF pos, Animation* a, Animation* aHover, std::string key, std::vector<int> activInStates = {})
	{
		activInStates = FillWith1WhenSize0(activInStates, nStates);
		//assert(activInStates.size() == nStates);
		comps[key] = std::make_unique<Button>(pos, a, aHover, activInStates, this, buffer);
		return static_cast<Button*>(comps[key].get());
	}
	
	/*
	virtual Composition* AddComposition(RectF pos, sharedResC resC, std::string key, std::vector<int> activInStates = {}, int type = 0)
	{
		activInStates = FillWith1WhenSize0(activInStates, nStates);
		assert(activInStates.size() == nStates);
		comps[key] = std::make_unique<Composition>(pos, resC, activInStates, this, buffer, type);
		return static_cast<Composition*>(comps[key].get());
	}
	*/
	
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
	PageFrame(RectF pos, int type, sharedResC resC, Component* parentC, int nPages, std::queue<FrameEvent>* buffer);

	template<typename T = bool>
	void AddText(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates = {})
	{
		static_assert (fail<T>::value, "Do not use!");
	}
	template<typename T = bool>
	void AddButton(RectF pos, Animation* a, Animation* aHover, std::vector<int> activInStates = {})
	{
		static_assert (fail<T>::value, "Do not use!");
	}

	Text* AddText(std::string text, RectF pos, int size, Font* f, Color c, std::string key, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {}, int textLoc = 0)
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages[key] = activOnPages;
		return Frame::AddText(text, pos, size, f, c, key, activInStates, textLoc);
	}
	Button* AddButton(RectF pos, Animation* a, Animation* aHover, std::string key, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {})
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages[key] = activOnPages;
		return Frame::AddButton(pos, a, aHover, key, activInStates);
	}
	/*
	Composition* AddComposition(RectF pos, sharedResC resC, std::string key, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {})
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages[key] = activOnPages;
		return Frame::AddComposition(pos, resC , key, activInStates);
	}
	*/
	/*
	virtual Composition* AddComposition(RectF pos, sharedResC resC, std::string key, int type = 0, std::vector<int> activInStates = {})
	{
		activInStates = FillWith1WhenSize0(activInStates, nStates);
		assert(activInStates.size() == nStates);
		comps[key] = std::make_unique<Composition>(pos, resC, activInStates, this, buffer, type);
		return static_cast<Composition*>(comps[key].get());
	}
	*/
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
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		if (IsVisible())
		{
			if (HandleMouseInputComps(e, interact))
			{
				return true;
			}
			return HandleMouseInputFrame(e, interact);
		}
		return false;
	}
	virtual bool HandleMouseInputFrame(Mouse::Event& e, bool interact)
	{
		return Frame::HandleMouseInputFrame(e, interact);
	}
	virtual bool HandleMouseInputComps(Mouse::Event& e, bool interact)
	{
		bool hitComp = false;
		std::for_each(comps.begin(), comps.end(), [&](auto& comp)
			{
				hitComp = comp.second->hitable && compActivOnPages[comp.first][curPage] == 1 && comp.second->activInStates[curState] == 1 && comp.second->IsVisible() && comp.second->HandleMouseInput(e, interact && !hitComp) || hitComp;
			});

		if (hitComp && interact)
		{
			return true;
		}
		return false;
	}
	
	void NextPage()
	{
		if (curPage + 1 < nPages)
		{
			curPage ++;
			AdjustArrowButtons();
		}
	}
	void PriviousPage()
	{
		if (curPage - 1 >= 0)
		{
			curPage--;
			AdjustArrowButtons();
		}
	}
	void AdjustArrowButtons()
	{
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
	}
};


bool B1(std::queue<FrameEvent>* buffer, Component* caller);
bool B2(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMode0(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMode2(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMode3(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMode21(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMode22(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMode23(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMode24(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMode25(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMode26(std::queue<FrameEvent>* buffer, Component* caller);



bool BNextTurn(std::queue<FrameEvent>* buffer, Component* caller);
bool BBuildMenu(std::queue<FrameEvent>* buffer, Component* caller);
bool BOpenGamefield(std::queue<FrameEvent>* buffer, Component* caller);
bool BSetAttackMode(std::queue<FrameEvent>* buffer, Component* caller);

class MultiFrame : public Frame
{
	std::vector<std::unique_ptr<Frame>> frames;
public:
	MultiFrame(RectF pos, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer) :Frame(pos, -1, resC, parentC, buffer) {}

	Frame* AddFrame(RectF pos, int type, sharedResC resC, Component* parentC);
	PageFrame* AddPageFrame(RectF pos, int type, sharedResC resC, Component* parentC, int nPages);

	void Draw(Graphics& gfx)override
	{
		if (IsVisible())
		{
			for (int i = 0; i < (int)(frames.size()); i++)
			{
				frames[i]->Draw(gfx);
			}
		}
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		if (e.GetType() == Mouse::Event::LRelease)
		{
			int k = 23;
		}
		if (IsVisible())
		{
			return HandleMouseInputFrame(e, interact);
		}
		return false;
	}
	virtual bool HandleMouseInputFrame(Mouse::Event& e, bool interact)
	{
			bool hit = Frame::HandleMouseInputFrame(e, interact);
			std::vector<bool> extended;
			for (int i = 0; i < (int)(frames.size()); i++)
			{
				extended.push_back(frames[i]->GetCurState());
			}
			int changed = -1;
			int yMove = 0;
			for (int n = 0; n < (int)(frames.size()); n++)
			{
				bool hitFrame = frames[n]->HandleMouseInput(e, interact);
				hit = hit || hitFrame;
				if (hitFrame)
				{
					for (int i = 0; i < (int)(frames.size()); i++)
					{
						frames[i]->pos.top += yMove;
						frames[i]->pos.bottom += yMove;
						if (extended[i] != (bool)frames[i]->GetCurState())
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
				for (int i = 0; i < (int)(frames.size()); i++)
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
	virtual bool HandleMouseInputComps(Mouse::Event& e, bool interact)
	{
		bool hitComp = false;
		std::for_each(comps.begin(), comps.end(), [&](auto& comp)
			{
				hitComp = comp.second->activInStates[curState] == 1 && comp.second->IsVisible() && comp.second->HandleMouseInput(e, interact && !hitComp) || hitComp;
			});

		if (hitComp && interact)
		{
			return true;
		}
		return false;
	}

	Frame* GetFrame(int index)
	{
		assert(index >= 0 && index < (int)(frames.size()));
		return frames[index].get();
	}
};
class FrameHandle
{
private:
	std::vector<std::unique_ptr<Frame>> windows;
	sharedResC resC;
	RectF overallParent = Graphics::GetScreenRect<float>();

	std::queue<FrameEvent> buffer;
	static constexpr unsigned int bufferSize = 4u;
public:
	bool BufferIsEmpty() const
	{
		return buffer.empty();
	}
	FrameEvent Read();

	const Frame& operator[](std::size_t idx) const { return *windows[idx].get(); }
	Frame& operator[](std::size_t idx) { return *windows[idx].get();  }

	FrameHandle(sharedResC resC);
	bool HandleMouseInput(Mouse::Event& e);
	void Draw(Graphics& gfx);
	
	Frame* AddFrame(RectF pos, int type);
	PageFrame* AddPageFrame(RectF pos, int type, int nPages);
	MultiFrame* AddMultiFrame(RectF pos, int type, int nStates);
	
	// ###### Init Frames #####
	void InitFrames()
	{
		using namespace Settings;
		std::vector<int> a = { 0,1 };	

		MultiFrame* m = AddMultiFrame(RectF(Vec2(540, 110), 140, 280), 0, 1);			//Size of frames is hardcoded
																							//FIRST FRAME
		Frame* f1 = m->AddFrame(RectF(Vec2(0, 0), 140, 280), 0, resC, m);	
		PageFrame* p3 = m->AddPageFrame(RectF(Vec2(0, 12), 140, 280), 0, resC, m, 3);
		
		// #1
    	f1->AddText(Settings::lang_fieldInformation[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->tC.fonts[0], Colors::Black,"h_f1");

		f1->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(60, 19), 50, 8), 7, &resC->tC.fonts[0], Colors::Black,"t_cellType", a, 1);
		f1->AddText(Settings::lang_flora[Settings::lang] + ":", RectF(Vec2(2, 19), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_flora", a, 1);
		f1->AddText(Settings::lang_Obstacle[Settings::lang] + ":", RectF(Vec2(2, 35), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_obstacle", a, 1);
		f1->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(60, 35), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_obstacleInfo", a, 1);
		f1->AddText(Settings::lang_hp[Settings::lang] + ":", RectF(Vec2(2, 46), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_obstacleHp", a, 1);
		f1->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(60, 46), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_obstacleHpIs", a, 1);

		// #2
		// #3
		// Page 1
		p3->AddText(Settings::lang_constructionMaterials[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "h_f3");

		p3->AddText(Settings::lang_resources[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "sh_resourcen", { 0,1 }, { 1, 0, 0 });
		p3->AddText(Settings::lang_materials[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "sh_materials", { 0,1 }, { 0, 1, 0 });
		p3->AddText(Settings::lang_organic[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "sh_organic", { 0,1 }, { 0, 0, 1 });

		p3->AddText(Settings::lang_wood[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_wood", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_iron[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_iron", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_sand[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_sand", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_stone[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_stone", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_copper[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_copper", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_gold[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_gold", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_aluminum[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_aluminum", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_emerald[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_emerald", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_sapphire[Settings::lang] + ":", RectF(Vec2(5, 130), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_sapphire", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_robin[Settings::lang] + ":", RectF(Vec2(5, 140), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_robin", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_diamond[Settings::lang] + ":", RectF(Vec2(5, 150), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_dimond", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText(Settings::lang_amber[Settings::lang] + ":", RectF(Vec2(5, 160), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_amber", { 0,1 }, { 1, 0, 0 }, 1);

		p3->AddText(Settings::lang_kilogram[Settings::lang], RectF(Vec2(100, 40), 50, 10), 7, &resC->tC.fonts[0], Colors::Black,"t_kilogram", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("11", RectF(Vec2(100, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nWood", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nIron", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSand", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("44", RectF(Vec2(100, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nStone", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nCopper", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("24", RectF(Vec2(100, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nGold", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("3", RectF(Vec2(100, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nAluminum", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("4", RectF(Vec2(100, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nEmerald", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 130), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSapphire", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("4", RectF(Vec2(100, 140), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nRobin", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("24", RectF(Vec2(100, 150), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nDimond", { 0,1 }, { 1, 0, 0 }, 1);
		p3->AddText("24", RectF(Vec2(100, 160), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nAmber", { 0,1 }, { 1, 0, 0 }, 1);

		//Page 2
		p3->AddText(Settings::lang_steel[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_steel", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_plastic[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_plastic", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_concrete[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_concrete", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_glass[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_glass", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_ceramics[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_ceramics", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_snow[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_snow", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_bricks[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_bricks", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText(Settings::lang_slate[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_slate", { 0,1 }, { 0, 1, 0 }, 1);

		p3->AddText("11", RectF(Vec2(100, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSteel", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nPlastic", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nConcrete", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("44", RectF(Vec2(100, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nGlass", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nCeramics", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSnow", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nBricks", { 0,1 }, { 0, 1, 0 }, 1);
		p3->AddText("2", RectF(Vec2(100, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSlate", { 0,1 }, { 0, 1, 0 }, 1);

		//Page 3
		p3->AddText(Settings::lang_corals[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_corals", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_sticks[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_sticks", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_leaves[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_leaves", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_wool[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_wool", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_leather[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_leather", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_fur[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_fur", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_meat[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_meat", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_fish[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_fish", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_berrys[Settings::lang] + ":", RectF(Vec2(5, 130), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_berrys", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_apples[Settings::lang] + ":", RectF(Vec2(5, 140), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_apples", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText(Settings::lang_cactus[Settings::lang] + ":", RectF(Vec2(5, 150), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_cactus", { 0,1 }, { 0, 0, 1 }, 1);

		p3->AddText("11", RectF(Vec2(100, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nCorals", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("2", RectF(Vec2(100, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSticks", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("2", RectF(Vec2(100, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nLeaves", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("44", RectF(Vec2(100, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nWool", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("2", RectF(Vec2(100, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nLeather", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("24", RectF(Vec2(100, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nFur", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("3", RectF(Vec2(100, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nMeat", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("4", RectF(Vec2(100, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nFish", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("2", RectF(Vec2(100, 130), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nBerrys", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("4", RectF(Vec2(100, 140), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nApples", { 0,1 }, { 0, 0, 1 }, 1);
		p3->AddText("24", RectF(Vec2(100, 150), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nCactus", { 0,1 }, { 0, 0, 1 }, 1);


		Frame* fUnity = AddFrame(RectF(Vec2(100, 150), 140, 280), 0);																			//Unit Frame
		fUnity->AddText(Settings::lang_unitInfo[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_h");
		fUnity->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(80, 19), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_unitNameIs", a, 1);
		fUnity->AddText(Settings::lang_unitName[Settings::lang] + ":", RectF(Vec2(2, 19), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_unitName", a, 1);
		fUnity->AddText(Settings::lang_hp[Settings::lang] + ":", RectF(Vec2(2, 35), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hp", a, 1);
		fUnity->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(80, 35), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hpIs", a, 1);
		fUnity->AddText(Settings::lang_team[Settings::lang] + ":", RectF(Vec2(2, 51), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_team", a, 1);
		fUnity->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(80, 51), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_teamIs", a, 1);
		fUnity->AddText(Settings::lang_stepsLeft[Settings::lang] + ":", RectF(Vec2(2, 67), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_steps", a, 1);
		fUnity->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(80, 67), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_stepsIs", a, 1);
		fUnity->SetState(1);
		fUnity->SetVisible(false);

		Button* b_setAttack = fUnity->AddButton(RectF(Vec2(30, 80), 60, 60), &resC->tC.windowsFrame[6], &resC->tC.windowsFrame[6], "b_setAttack", a);
		b_setAttack->bFunc = BSetAttackMode;
		b_setAttack->hitable = true;

		Frame* fNextTurn = AddFrame(RectF(Vec2(620, 500), 120, 60), 1);																			//NEXT TURN FRAME
		fNextTurn->s = &resC->tC.windowsFrame[3].GetCurSurface();
		fNextTurn->bFunc = BNextTurn;

		Frame* fButtonBuild = AddFrame(RectF(Vec2(30, 30), 60, 30), 1);																			//BUILD MENU BUTTON
		fButtonBuild->s = &resC->tC.windowsFrame[4].GetCurSurface();
		fButtonBuild->bFunc = BBuildMenu;

		PageFrame* fBuildSelection = AddPageFrame(RectF(Vec2(0, 0), 800, 600), 1, 4);															//build selection menu
		fBuildSelection->s = &resC->tC.windowsFrame[5].GetCurSurface();
		fBuildSelection->SetVisible(false);
		fBuildSelection->SetMoveable(false);
		fBuildSelection->GetComp("b_left")->pos = RectF(Vec2(25, 25), 100, 25);
		fBuildSelection->GetComp("b_right")->pos = RectF(Vec2(675, 25), 100, 25);
		Button* b_back = fBuildSelection->AddButton(RectF(Vec2(30, 30), 60, 30), &resC->tC.windowsFrame[6], &resC->tC.windowsFrame[6], "b_buildback", a, { 1,0,0,0 });
		b_back->bFunc = BOpenGamefield;
		//Composition* comp_Tent = fBuildSelection->AddComposition(RectF(Vec2(60, 120), 180, 60), resC, "comp_tent", a, { 1,0,0,0 });
		Button* bg_Tent = fBuildSelection->AddButton(RectF(Vec2(60, 120), 180, 60), &resC->tC.windowsFrame[7], &resC->tC.windowsFrame[7], "b_buildtent", a, { 1,0,0,0 });
		bg_Tent->bFunc = BBuildMode0;
		fBuildSelection->AddText(lang_tent[lang], RectF(Vec2(132, 127), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hTent", a, { 1,0,0,0 });
		fBuildSelection->AddText("- "+lang_leather[lang]+": x"+std::to_string((int)neededRes[0].at("leather"))+" "+lang_kilogram[lang], RectF(Vec2(102, 140), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_TentR1", a, { 1,0,0,0 },1);
		fBuildSelection->AddText("- " + lang_sticks[lang] + ": x" + std::to_string((int)neededRes[0].at("sticks")) + " " + lang_kilogram[lang], RectF(Vec2(102, 150), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_TentR2", a, { 1,0,0,0 }, 1);

		Button* bg_igloo = fBuildSelection->AddButton(RectF(Vec2(60, 200), 180, 60), &resC->tC.windowsFrame[7], &resC->tC.windowsFrame[7], "b_buildigloo", a, { 1,0,0,0 });
		bg_igloo->bFunc = BBuildMode21;
		fBuildSelection->AddText(lang_igloo[lang], RectF(Vec2(132, 207), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hIgloo", a, { 1,0,0,0 });
		fBuildSelection->AddText("- " + lang_snow[lang] + ": x" + std::to_string((int)neededRes[21].at("snow")) + " " + lang_kilogram[lang], RectF(Vec2(102, 220), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_IglooR1", a, { 1,0,0,0 }, 1);
		fBuildSelection->AddText("- " + lang_wood[lang] + ": x" + std::to_string((int)neededRes[21].at("wood")) + " " + lang_kilogram[lang], RectF(Vec2(102, 230), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_IglooR2", a, { 1,0,0,0 }, 1);

		Button* bg_woodHouse = fBuildSelection->AddButton(RectF(Vec2(60, 280), 180, 60), &resC->tC.windowsFrame[7], &resC->tC.windowsFrame[7], "b_buildwoodenHouse", a, { 1,0,0,0 });
		bg_woodHouse->bFunc = BBuildMode22;
		fBuildSelection->AddText(lang_woodenHouse[lang], RectF(Vec2(132, 287), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hWoodenhouse", a, { 1,0,0,0 });
		fBuildSelection->AddText("- " + lang_wood[lang] + ": x" + std::to_string((int)neededRes[22].at("wood")) + " " + lang_kilogram[lang], RectF(Vec2(102, 300), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_woodenHouseR1", a, { 1,0,0,0 }, 1);

		Button* bg_stoneHouse = fBuildSelection->AddButton(RectF(Vec2(60, 360), 180, 60), &resC->tC.windowsFrame[7], &resC->tC.windowsFrame[7], "b_buildstoneHouse", a, { 1,0,0,0 });
		bg_stoneHouse->bFunc = BBuildMode23;
		fBuildSelection->AddText(lang_stoneHouse[lang], RectF(Vec2(132, 367), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hStoneHouse", a, { 1,0,0,0 });
		fBuildSelection->AddText("- " + lang_stone[lang] + ": x" + std::to_string((int)neededRes[23].at("stone")) + " " + lang_kilogram[lang], RectF(Vec2(102, 380), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_stoneHouseR1", a, { 1,0,0,0 }, 1);
		fBuildSelection->AddText("- " + lang_wood[lang] + ": x" + std::to_string((int)neededRes[23].at("wood")) + " " + lang_kilogram[lang], RectF(Vec2(102, 390), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_stoneHouseR2", a, { 1,0,0,0 }, 1);

		Button* bg_brickHouse = fBuildSelection->AddButton(RectF(Vec2(60, 440), 180, 60), &resC->tC.windowsFrame[7], &resC->tC.windowsFrame[7], "b_buildbrickhouse", a, { 1,0,0,0 });
		bg_brickHouse->bFunc = BBuildMode24;
		fBuildSelection->AddText(lang_brickhouse[lang], RectF(Vec2(132, 447), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hBrickhouse", a, { 1,0,0,0 });
		fBuildSelection->AddText("- " + lang_stone[lang] + ": x" + std::to_string((int)neededRes[24].at("stone")) + " " + lang_kilogram[lang], RectF(Vec2(102, 460), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_brickHouseR1", a, { 1,0,0,0 }, 1);
		fBuildSelection->AddText("- " + lang_wood[lang] + ": x" + std::to_string((int)neededRes[24].at("wood")) + " " + lang_kilogram[lang], RectF(Vec2(102, 470), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_brickHouseR2", a, { 1,0,0,0 }, 1);
		fBuildSelection->AddText("- " + lang_bricks[lang] + ": x" + std::to_string((int)neededRes[24].at("bricks")) + " " + lang_kilogram[lang], RectF(Vec2(102, 480), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_brickHouseR3", a, { 1,0,0,0 }, 1);

		Button* bg_skyscraper = fBuildSelection->AddButton(RectF(Vec2(60, 520), 180, 60), &resC->tC.windowsFrame[7], &resC->tC.windowsFrame[7], "b_buildskyscraper", a, { 1,0,0,0 });
		bg_skyscraper->bFunc = BBuildMode25;
		fBuildSelection->AddText(lang_skyscraper[lang], RectF(Vec2(132, 527), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hSkyscraper", a, { 1,0,0,0 });
		fBuildSelection->AddText("- " + lang_concrete[lang] + ": x" + std::to_string((int)neededRes[25].at("concrete")) + " " + lang_kilogram[lang], RectF(Vec2(102, 540), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_skyscraperR1", a, { 1,0,0,0 }, 1);

		Button* bg_villa = fBuildSelection->AddButton(RectF(Vec2(260, 120), 180, 60), &resC->tC.windowsFrame[7], &resC->tC.windowsFrame[7], "b_buildVilla", a, { 1,0,0,0 });
		bg_villa->bFunc = BBuildMode26;
		fBuildSelection->AddText(lang_villa[lang], RectF(Vec2(332, 127), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hVilla", a, { 1,0,0,0 });
		fBuildSelection->AddText("- " + lang_slate[lang] + ": x" + std::to_string((int)neededRes[26].at("slate")) + " " + lang_kilogram[lang], RectF(Vec2(302, 140), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_VillaR1", a, { 1,0,0,0 }, 1);
		fBuildSelection->AddText("- " + lang_bricks[lang] + ": x" + std::to_string((int)neededRes[26].at("bricks")) + " " + lang_kilogram[lang], RectF(Vec2(302, 150), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_VillaR2", a, { 1,0,0,0 }, 1);
		fBuildSelection->AddText("- " + lang_concrete[lang] + ": x" + std::to_string((int)neededRes[26].at("concrete")) + " " + lang_kilogram[lang], RectF(Vec2(302, 160), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_VillaR3", a, { 1,0,0,0 }, 1);

		//Button* bg_Tent = fBuildSelection->AddButton(RectF(Vec2(60, 120), 180, 60), &resC->tC.windowsFrame[7], &resC->tC.windowsFrame[7], "b_buildtent", a, { 1,0,0,0 });

}
	// ### Update components of existing Frames ###

	// ###### Update existing Frames ######
	void UpdateFieldinformation(World& curW, Team* player)				
	{
		if (Settings::framesOn)
		{
			
			MultiFrame* m = static_cast<MultiFrame*>(windows[0].get());
			Materials& playerM = player->GetMaterials();
			//#1
			Frame* f1 = static_cast<Frame*>(m->GetFrame(0));
			f1->SetText(Settings::GetTypeString(curW.GetfCellType()), "t_cellType");
			if (curW.GetFocusedObstacle() != nullptr)
			{
				f1->SetText(Settings::GetObstacleString(curW.GetFocusedObstacle()->type), "t_obstacleInfo");
				f1->SetText(std::to_string(curW.GetFocusedObstacle()->hp), "t_obstacleHpIs");
			}

			//#2

			//#3
			PageFrame* p3 = static_cast<PageFrame*>(m->GetFrame(1));
			
			// Page 1
			p3->SetText(std::to_string(playerM.values["wood"]), "t_nWood");
			p3->SetText(std::to_string(playerM.values["iron"]), "t_nIron");
			p3->SetText(std::to_string(playerM.values["sand"]), "t_nSand");
			p3->SetText(std::to_string(playerM.values["stone"]), "t_nStone");
			p3->SetText(std::to_string(playerM.values["copper"]), "t_nCopper");
			p3->SetText(std::to_string(playerM.values["gold"]), "t_nGold");
			p3->SetText(std::to_string(playerM.values["aluminum"]), "t_nAluminum");
			p3->SetText(std::to_string(playerM.values["emerald"]), "t_nEmerald");
			p3->SetText(std::to_string(playerM.values["sapphire"]), "t_nSapphire");
			p3->SetText(std::to_string(playerM.values["robin"]), "t_nRobin");
			p3->SetText(std::to_string(playerM.values["diamond"]), "t_nDimond");
			p3->SetText(std::to_string(playerM.values["amber"]), "t_nAmber");

			//Page 2
			p3->SetText(std::to_string(playerM.values["steel"]), "t_nSteel");
			p3->SetText(std::to_string(playerM.values["plastic"]), "t_nPlastic");
			p3->SetText(std::to_string(playerM.values["concrete"]), "t_nConcrete");
			p3->SetText(std::to_string(playerM.values["glass"]), "t_nGlass");
			p3->SetText(std::to_string(playerM.values["ceramics"]), "t_nCeramics");
			p3->SetText(std::to_string(playerM.values["snow"]), "t_nSnow");
			p3->SetText(std::to_string(playerM.values["bricks"]), "t_nBricks");
			p3->SetText(std::to_string(playerM.values["slate"]), "t_nSlate");

			//Page 3
			p3->SetText(std::to_string(playerM.values["corals"]), "t_nCorals");
			p3->SetText(std::to_string(playerM.values["sticks"]), "t_nSticks");
			p3->SetText(std::to_string(playerM.values["leafes"]), "t_nLeaves");
			p3->SetText(std::to_string(playerM.values["wool"]), "t_nWool");
			p3->SetText(std::to_string(playerM.values["leather"]), "t_nLeather");
			p3->SetText(std::to_string(playerM.values["fur"]), "t_nFur");
			p3->SetText(std::to_string(playerM.values["meat"]), "t_nMeat");
			p3->SetText(std::to_string(playerM.values["fish"]), "t_nFish");
			p3->SetText(std::to_string(playerM.values["berrys"]), "t_nBerrys");
			p3->SetText(std::to_string(playerM.values["apples"]), "t_nApples");
			p3->SetText(std::to_string(playerM.values["cactus"]), "t_nCactus");
			
		}
	}
	void UpdateUnitinformation(Obstacle* obst)
	{
		Frame* f = static_cast<Frame*>(windows[1].get());
		f->SetVisible(true);
		std::string s1 = Settings::GetObstacleString(obst->type);

		f->SetText(s1, "t_unitNameIs");
		f->SetText(std::to_string(obst->hp), "t_hpIs");
		f->SetText(std::to_string(obst->stepsLeft), "t_stepsIs");
		if (obst->team != nullptr)
		{
			f->SetText(obst->team->GetTeamName(), "t_teamIs");
		}
		else
		{
			f->SetText(Settings::lang_noInformation[Settings::lang], "t_teamIs");
		}
	}
	void HideUnitInfo()
	{
		Frame* f = static_cast<Frame*>(windows[1].get());
		f->SetVisible(false);
	}
	void BuildSelectionStarted()
	{
		MultiFrame* m = static_cast<MultiFrame*>(windows[0].get());
		m->SetVisible(false);
	
		Frame* f1 = static_cast<Frame*>(windows[1].get());
		Frame* f2 = static_cast<Frame*>(windows[2].get());
		Frame* f3 = static_cast<Frame*>(windows[3].get());

		f1->SetVisible(false);
		f2->SetVisible(false);
		f3->SetVisible(false);

		PageFrame* f4 = static_cast<PageFrame*>(windows[4].get());
		f4->SetVisible(true);
		f4->AdjustArrowButtons();
	}
	void OpenGamefield()
	{
		MultiFrame* m = static_cast<MultiFrame*>(windows[0].get());
		m->SetVisible(true);

		Frame* f1 = static_cast<Frame*>(windows[1].get());
		Frame* f2 = static_cast<Frame*>(windows[2].get());
		Frame* f3 = static_cast<Frame*>(windows[3].get());

		f2->SetVisible(true);
		f3->SetVisible(true);

		Frame* f4 = static_cast<Frame*>(windows[4].get());
		f4->SetVisible(false);

	}
	static constexpr float percentForGrab = 0.05;			
};

