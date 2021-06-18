#pragma once
#include "World.h"
#include <memory>
#include <map>
#include <algorithm>
#include "Graphics.h"
class ButtonFunctions;
class PageFrame;
class FrameHandle;
class Component;
class Text;
class Frame;
class PageFrame;
class Button;
class Image;
class GrabImage;
class CheckBox;
class TextBox;

class FrameEvent
{
public:
	enum class Type
	{
		ButtonPressed,
		ItemDragReleased,
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
		return type != Type::Invalid;
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
protected:
	bool visible = true;
	int prio = 5;	//prio range [0-10] lower means first handled
	std::queue<FrameEvent>* buffer;
	std::map<std::string, std::unique_ptr<Component>> comps;
	void IncreasePrio()
	{
		if (GetPrio() < 10)
			prio++;
	}
	virtual bool HandleMouseInputFrame(Mouse::Event& e, bool interact)
	{
		if (e.GetType() == Mouse::Event::Type::LRelease && interact && GetPos().Contains((Vec2)e.GetPos()))
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
				hitComp = e.GetType() == Mouse::Event::Type::LRelease && comp.second->IsVisible() && comp.second->HandleMouseInput(e, interact && !hitComp) || hitComp;
			});

		if (hitComp && interact)
		{
			return true;
		}
		return false;
	}
public:
	Component* parentC;
	virtual RectF GetPos() {
		if (parentC == nullptr)
		{
			return pos;
		}
		return pos + parentC->GetPos().GetTopLeft<float>(); }
	RectF pos;
	int curState = 0;
	int extra1 = 0;
	int extra2 = 0;
	int nStates = 1;
	bool extraB1 = false;
	std::string extraS1 = "";
	Color c = Colors::Black;
	std::vector<int> activInStates;
	std::string text = "no title";
	bool mouseHovers = false, hitable = false;
	Component(RectF pos, Component* parentC, std::queue<FrameEvent>* buffer) :pos(pos), parentC(parentC),buffer(buffer){}
	virtual void Draw(Graphics& gfx)
	{
		gfx.DrawFilledRect(GetPos(), c, SpriteEffect::Nothing());
	}
	virtual void DrawComps(Graphics& gfx)
	{
		std::vector<Component*> drawOrder;

		std::for_each(comps.begin(), comps.end(), [&](auto& comp)		//auto = something like 'std::pair<std::string, std::unique_ptr<Component>>'
		{
			if (comp.second->activInStates[curState] == 1 && comp.second->IsVisible()) {
				drawOrder.push_back(comp.second.get());
			}
		});
		std::sort(drawOrder.begin(), drawOrder.end(), [&](Component* obst1, Component* obst2)
		{
			return obst1->GetPrio() > obst2->GetPrio();
		});
		for (int i = 0; i < drawOrder.size(); i++)
		{
			drawOrder[i]->Draw(gfx);
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
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		IncreasePrio();
		if (IsVisible())
		{
			if (HandleMouseInputComps(e, interact) || HandleMouseInputFrame(e, interact))
			{
				SetPrio(1);
				return true;
			}
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
	void SetPrio(int prio)
	{
		this->prio = prio;
	}
	int GetPrio()const
	{
		return prio;
	}
	std::vector<int> FillWith1WhenSize0(std::vector<int> activInStates, int nStages);

	Text* AddText(std::string text, RectF pos, int size, Font* f, Color c, std::string key, std::vector<int> activInStates = {}, int textLoc = 0);
	TextBox* AddTextBox(std::string text, RectF pos, int size, Font* f, Color c, std::string key, std::vector<int> activInStates = {}, int textLoc = 0);
	Button* AddButton(RectF pos, Animation* a, Animation* aHover, std::string key, Font* f, std::vector<int> activInStates = {});
	CheckBox* AddCheckBox(RectF pos, std::queue<FrameEvent>* buffer, sharedResC resC, std::string key, std::vector<int> activInStates = {});
	Image* AddImage(RectF pos, Animation* a, Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates = {});
	GrabImage* AddGrabImage(RectF pos, Animation* a, Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates = {});
	Frame* AddFrame(RectF pos, int type, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates = {});
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
			RectF drawPos;
			if (parentC == nullptr)
			{
				drawPos = GetPos() + (Vec2)(pos.GetSize() / 2);
			}
			else
			{
				drawPos = parentC->GetPos() + pos.GetCenter();
			}
			f->DrawTextCentered(text, drawPos.GetTopLeft<int>(), size, c);
		}
		else if (textLoc == 1)
		{
			RectF drawPos;
			if (parentC == nullptr)
			{
				drawPos = GetPos();
			}
			else
			{
				drawPos = parentC->GetPos() + pos.GetTopLeft<float>();
			}
			f->DrawText(text, (int)(drawPos.left), (int)(drawPos.top), size, c);
		}
		//gfx.DrawRect(GetPos(), Colors::Red);
	}
};
class TextBox : public Text
{
protected:
	std::vector<std::string> lines;
public:
	TextBox(std::string text, RectF pos, int size, Font* f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc, std::queue<FrameEvent>* buffer);
	void Draw(Graphics& gfx) override
	{
		for (int i = 0; i < (int)lines.size(); i++)
		{
			RectF drawPos;
			if (parentC == nullptr)
			{
				drawPos = GetPos();
			}
			else
			{
				drawPos = parentC->GetPos() + pos.GetTopLeft<float>();
			}
			f->DrawText(lines[i], (int)(drawPos.left), (int)(drawPos.top) + i * (size+2), size, c);
		}
	}
};
class CheckBox : public Component
{
protected:
	sharedResC resC;
	bool ticked = false;
	std::vector<CheckBox*> disable;
	void UncheckOther()
	{
		for (auto checkBox : disable)
		{
			checkBox->Uncheck();
		}
	}
	bool(*bFunc)(std::queue<FrameEvent>* buffer, Component* caller) = nullptr;
public:
	CheckBox(RectF pos, Component* parentC, std::queue<FrameEvent>* buffer, sharedResC resC, std::vector<int> activInStates = {});
	void Draw(Graphics& gfx) override
	{
		if (ticked)
		{
			gfx.DrawSurface((RectI)GetPos(), resC->tC.buttons[5].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
		}
		else
		{
			gfx.DrawSurface((RectI)GetPos(), resC->tC.buttons[4].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
		}
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)override
	{
		IncreasePrio();
		if (Component::HandleMouseInputFrame(e, interact) && e.GetType() == Mouse::Event::Type::LRelease)
		{
			ticked = !ticked;
			extraB1 = false;
			if (ticked)
			{
				UncheckOther();
				extraB1 = true;
			}
			if (bFunc != nullptr)
			{
				bFunc(buffer, this);
			}
			return true;
		}
		return false;
	}
	void AddCheckBox2Disable(CheckBox* other)
	{
		disable.push_back(other);
	}
	void Uncheck()
	{
		ticked = false;
	}
	void Check()
	{
		ticked = true;
	}
	void SetBFunc(bool(*bFunc)(std::queue<FrameEvent>* buffer, Component* caller))
	{
		this->bFunc = bFunc;
	}
};
class Button : public Text
{
protected:
	Animation* a = nullptr;
	Animation* aHover = nullptr;
	virtual bool HandleMouseInputFrame(Mouse::Event& e, bool interact)override
	{
		if (visible)
		{
			Component::HandleMouseInputFrame(e, interact);
			if (GetPos().Contains((Vec2)e.GetPos()) && e.GetType() == Mouse::Event::Type::LRelease && interact)
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
		}
		return false;
	}
public:
	bool(*bFunc)(std::queue<FrameEvent>* buffer, Component* caller) = nullptr;
	PageFrame* ppF = nullptr;
	//Frame* pF = nullptr;

	Button(RectF pos, Animation* a, Animation* aHover, std::vector<int> activInStates, Font* f, Component* parentC, std::queue<FrameEvent>* buffer);
	void Draw(Graphics& gfx) override
	{
		if (visible)
		{
			if (mouseHovers)
			{
				gfx.DrawSurface((RectI)GetPos(), aHover->GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			else
			{
				gfx.DrawSurface((RectI)GetPos(), a->GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
			}
			Text::Draw(gfx);
		}
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		if (IsVisible())
		{
			if (HandleMouseInputComps(e, interact) || HandleMouseInputFrame(e, interact))
			{
				SetPrio(1);
				return true;
			}
		}
		return false;
	}
};
class Image : public Component
{
	Animation* a;
	Animation* aHover;
	Vec2 drawPercent = Vec2(1.f, 1.f);
	float alpha = 1.f;
public:
	Image(RectF pos, Animation* a, Animation* aHover, Component* parentC, std::queue<FrameEvent>* buffer, std::vector<int> activInStates);
	void Draw(Graphics& gfx) override
	{
		const Surface* draw = nullptr;
		if (mouseHovers && aHover != nullptr)
		{
			draw = &aHover->GetCurSurface();
		}
		else if(a != nullptr)
		{
			draw = &a->GetCurSurface();
		}
		if (draw != nullptr)
		{
			assert(alpha >= 0.f && alpha <= 1.f);
			if (drawPercent == Vec2(1.f, 1.f))
			{
				if (alpha == 1.f)
				{
					gfx.DrawSurface((RectI)GetPos(), *draw, SpriteEffect::Chroma(Colors::Magenta));
				}
				else
				{
					gfx.DrawSurface((RectI)GetPos(), *draw, SpriteEffect::Transparent(Colors::Magenta,alpha));
				}
			}
			else
			{
				RectF pos = GetPos();
				Vec2 surfaceSize = (Vec2)aHover->GetCurSurface().GetSize();
				RectF sourceR = RectF(Vec2(0, 0), surfaceSize.x * drawPercent.x, surfaceSize.y * drawPercent.y);
				if (alpha == 1.f)
				{
					gfx.DrawSurface(RectI(Vei2((int)pos.left, (int)pos.top), (int)(pos.GetWidth() * drawPercent.x), (int)(pos.GetHeight() * drawPercent.y)), (RectI)sourceR, *draw, SpriteEffect::Chroma(Colors::Magenta));
				}
				else
				{
					gfx.DrawSurface(RectI(Vei2((int)pos.left, (int)pos.top), (int)(pos.GetWidth() * drawPercent.x), (int)(pos.GetHeight() * drawPercent.y)), (RectI)sourceR, *draw, SpriteEffect::Transparent(Colors::Magenta, 0.5f));
				}
			}
		}
		DrawComps(gfx);
	}
	void SetAnimation(Animation* a)
	{
		this->a = a;
	}
	void SetAnimationOfBouth(Animation* a)
	{
		this->a = a;
		this->aHover = a;

	}
	void SetHoverAnimation(Animation* a)
	{
		this->a = a;
	}
	void SetDrawPercent(Vec2 drawPercent)
	{
		this->drawPercent = drawPercent;
	}
	void SetAlpha(float alpha)
	{
		this->alpha = alpha;
	}
};
class GrabImage : public Image
{
	GrabHandle gH = GrabHandle(0.0f);
	Vec2 delta = Vec2(0, 0);
public:
	GrabImage(RectF pos, Animation* a, Animation* aHover, Component* parentC, std::queue<FrameEvent>* buffer, std::vector<int> activInStates);
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)override
	{
		IncreasePrio();
		if (IsVisible())
		{
			Vec2 mP = (Vec2)e.GetPos();
			if (e.GetType() == Mouse::Event::Type::LRelease && gH.IsLocked())
			{
				if (delta != Vec2(0, 0))
					buffer->push(FrameEvent(FrameEvent::Type::ItemDragReleased, extraS1, extra1, this));
				gH.Release();
				delta = Vec2(0, 0);
				return true;
			}
			if (GetPos().Contains(mP) || gH.IsLocked())
			{
				delta += gH.MoveCamera(e);
				if (parentC != nullptr)
				{
					parentC->SetPrio(1);
				}
				else
				{
					SetPrio(1);
				}
				return true;
			}
		}
		return false;
	}
	virtual RectF GetPos()override {
		if (parentC == nullptr)
		{
			return pos + delta;
		}
		return pos + delta + parentC->GetPos().GetTopLeft<float>();
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
	int type = 0;
public:
	const Surface* s = nullptr;
	Frame(RectF pos, int type, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer);
	bool(*bFunc)(std::queue<FrameEvent>* buffer, Component* caller) = nullptr;

	void SetMoveable(bool moveable)
	{
		this->moveable = moveable;
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
		IncreasePrio();
		if (IsVisible())
		{
			if (HandleMouseInputComps(e, interact) || HandleMouseInputFrame(e, interact))
			{
				SetPrio(1);
				return true;
			}
		}
		return false;
	}
	virtual bool HandleMouseInputFrame(Mouse::Event& e, bool interact)
	{
			Vec2 mP = (Vec2)e.GetPos();
			bool hit = Hit(mP);

			if (grabbed && moveable)
			{
				if (e.GetType() == Mouse::Event::Type::LRelease)
				{
					Release();
				}
				else
				{
					Move(mP);
				}
			}
			if (e.GetType() == Mouse::Event::Type::LPress && hit && moveable)
			{
				Grab(mP);
				posOfLastPress = mP;
			}
			Vei2 delta((posOfLastPress - mP).GetAbsVec());
			if (nStates > 1 && hit && e.GetType() == Mouse::Event::Type::LRelease && delta.GetLength() < 15)
			{
				NextState();
			}
			if (hit)
			{
				if (bFunc != nullptr && e.GetType() == Mouse::Event::Type::LRelease)
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
		bool hitComp = interact;
		for (int prio = 0; prio <= 10; prio++)
		{
			if (std::any_of(comps.begin(), comps.end(), [&](auto& comp)
				{
					if (comp.second->GetPrio() == prio && hitComp && comp.second->hitable && comp.second->activInStates[curState] == 1 && comp.second->IsVisible() && comp.second->HandleMouseInput(e, interact && hitComp))
					{
						comp.second->SetPrio(1);
						hitComp = false;
						return true;
					}
					return false;
				}))
			{
				if (interact)
				{
					return true;
				}
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
	int GetNumberOfComps() { return (int)comps.size(); }
	int GetCurState() { return curState; }
	int GetExtendedHeight();
	bool IsExtended();
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
	void AddButtonPF(RectF pos, Animation* a, Animation* aHover, std::vector<int> activInStates = {})
	{
		static_assert (fail<T>::value, "Do not use!");
	}

	Text* AddTextPF(std::string text, RectF pos, int size, Font* f, Color c, std::string key, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {}, int textLoc = 0)
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages[key] = activOnPages;
		return Component::AddText(text, pos, size, f, c, key, activInStates, textLoc);
	}
	Button* AddButtonPF(RectF pos, Animation* a, Animation* aHover, std::string key, Font* f, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {})
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages[key] = activOnPages;
		return Component::AddButton(pos, a, aHover, key, f, activInStates);
	}
	
	Frame* AddFramePF(RectF pos, std::string key, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {})
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages[key] = activOnPages;
		return Component::AddFrame(pos,type,resC,this,buffer, key, activInStates);
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
			if (HandleMouseInputComps(e, interact) || Frame::HandleMouseInputFrame(e, interact))
			{
				SetPrio(1);
				return true;
			}
		}
		return false;
	}
	virtual bool HandleMouseInputComps(Mouse::Event& e, bool interact)
	{
		if (e.GetType() == Mouse::Event::Type::LRelease)
		{
			int k = 23;
		}
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
		/*
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
		*/
	}
};


bool B1(std::queue<FrameEvent>* buffer, Component* caller);
bool B2(std::queue<FrameEvent>* buffer, Component* caller);

bool BBuildMode(std::queue<FrameEvent>* buffer, Component* caller); 
bool BCraftingQueue(std::queue<FrameEvent>* buffer, Component* caller);

bool BNextTurn(std::queue<FrameEvent>* buffer, Component* caller);
bool BLoadScene(std::queue<FrameEvent>* buffer, Component* caller);
bool BSetAttackMode(std::queue<FrameEvent>* buffer, Component* caller);

bool BSetObstacleState(std::queue<FrameEvent>* buffer, Component* caller);

class MultiFrame : public Frame
{
	std::map<std::string, std::unique_ptr<Frame>> frames;
public:
	MultiFrame(RectF pos, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer) :Frame(pos, -1, resC, parentC, buffer) {}

	Frame* AddFrame(std::string key, RectF pos, int type, sharedResC resC, Component* parentC);
	PageFrame* AddPageFrame(std::string key, RectF pos, int type, sharedResC resC, Component* parentC, int nPages);

	void Draw(Graphics& gfx)override
	{
		if (IsVisible())
		{
			std::map<std::string, std::unique_ptr<Frame>>::iterator frame;
			for (frame = frames.begin(); frame != frames.end(); frame++)
			{
				frame->second->Draw(gfx);
			}
		}
	}
	virtual bool HandleMouseInput(Mouse::Event& e, bool interact)
	{
		if (IsVisible())
		{
			if (HandleMouseInputFrame(e, interact))
			{
				SetPrio(1);
				return true;
			}
		}
		return false;
	}
	virtual bool HandleMouseInputFrame(Mouse::Event& e, bool interact)
	{
			bool hit = Frame::HandleMouseInputFrame(e, interact);
			std::map<std::string,bool> extended;
			std::map<std::string, std::unique_ptr<Frame>>::iterator frame;
			for (frame = frames.begin(); frame != frames.end(); frame++)
			{
				extended[frame->first] = frame->second->GetCurState();
			}
			std::string changed ="-";
			int yMove = 0;
			int n = 0;
			for (frame = frames.begin(); frame != frames.end(); frame++)
			{
				bool hitFrame = frame->second->HandleMouseInput(e, interact);
				hit = hit || hitFrame;
				if (hitFrame)
				{
					
					std::map<std::string, std::unique_ptr<Frame>>::iterator innerFrame;
					for (innerFrame = frames.begin(); innerFrame != frames.end(); innerFrame++)
					{
						innerFrame->second->pos.top += yMove;
						innerFrame->second->pos.bottom += yMove;
						if (extended[innerFrame->first] != (bool)innerFrame->second->GetCurState())
						{
							changed = innerFrame->first;
							n = (int)frames.size();
							if (extended[innerFrame->first])
							{
								yMove -= frame->second->GetExtendedHeight();
							}
							if (!extended[innerFrame->first])
							{
								yMove += frame->second->GetExtendedHeight();
							}
						}
					}
				}
				n++;
			}
			yMove = 0;
			if (changed != "-")
			{
				int i = 0;
				std::map<std::string, std::unique_ptr<Frame>>::iterator frame;
				for (frame = frames.begin(); frame != frames.end(); frame++)
				{
					frame->second->pos.top += yMove;
					frame->second->pos.bottom += yMove;
					if (frame->second->IsExtended() && frame->first != changed)
					{
						frame->second->SetState(0);
						yMove -= frame->second->GetExtendedHeight();
					}
					i++;
				}
			}
			if (hit || grabbed)
			{
				return true;
			}
			return changed != "-";
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

	Frame* GetFrame(std::string key)
	{
		return frames[key].get();
	}
};
class FrameHandle
{
private:
	std::map<std::string, std::unique_ptr<Component>> comps;
	sharedResC resC;
	RectF overallParent = Graphics::GetScreenRect<float>();

	int curScene = 0;
	std::queue<FrameEvent> buffer;
	static constexpr unsigned int bufferSize = 4u;
	Frame* AddFrame(std::string key,RectF pos, int type);
	PageFrame* AddPageFrame(std::string key, RectF pos, int type, int nPages);
	MultiFrame* AddMultiFrame(std::string key, RectF pos, int type, int nStates);
public:
	bool BufferIsEmpty() const
	{
		return buffer.empty();
	}
	FrameEvent Read();
	int GetCurScene() { return curScene; }
	//const Frame& operator[](std::size_t idx) const { return *comps[idx].get(); }
	//Frame& operator[](std::size_t idx) { return *windows[idx].get();  }

	FrameHandle(sharedResC resC);
	bool HandleMouseInput(Mouse::Event& e);
	void Draw(Graphics& gfx);
	
	// ###### Init Frames #####
	void LoadScene(int scene, World* world)
	{
		comps.clear();
		curScene = scene;
		if (scene == 0)
		{
			using namespace Settings;
			std::vector<int> a = { 0,1 };
			
			MultiFrame* m = AddMultiFrame("f_resD", RectF(Vec2(1040, 110), 140, 280), 0, 1);

			Frame* f1 = m->AddFrame("fresD_f1", RectF(Vec2(0, 0), 140, 280), 0, resC, m);
			PageFrame* p3 = m->AddPageFrame("fresD_f2", RectF(Vec2(0, 12), 140, 280), 0, resC, m, 3);

			// #1
			f1->AddText(Settings::lang_fieldInformation[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "h_f1");

			f1->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(60, 19), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_cellType", a, 1);
			f1->AddText(Settings::lang_flora[Settings::lang] + ":", RectF(Vec2(2, 19), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_flora", a, 1);
			f1->AddText(Settings::lang_Obstacle[Settings::lang] + ":", RectF(Vec2(2, 35), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_obstacle", a, 1);
			f1->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(60, 35), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_obstacleInfo", a, 1);
			f1->AddText(Settings::lang_hp[Settings::lang] + ":", RectF(Vec2(2, 46), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_obstacleHp", a, 1);
			f1->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(60, 46), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_obstacleHpIs", a, 1);

			// #2
			// #3
			// Page 1
			p3->AddTextPF(Settings::lang_constructionMaterials[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "h_f3");

			p3->AddTextPF(Settings::lang_resources[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "sh_resourcen", { 0,1 }, { 1, 0, 0 });
			p3->AddTextPF(Settings::lang_materials[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "sh_materials", { 0,1 }, { 0, 1, 0 });
			p3->AddTextPF(Settings::lang_organic[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "sh_organic", { 0,1 }, { 0, 0, 1 });

			p3->AddTextPF(Settings::lang_wood[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_wood", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_iron[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_iron", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_sand[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_sand", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_stone[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_stone", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_copper[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_copper", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_gold[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_gold", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_aluminum[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_aluminum", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_emerald[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_emerald", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_sapphire[Settings::lang] + ":", RectF(Vec2(5, 130), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_sapphire", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_robin[Settings::lang] + ":", RectF(Vec2(5, 140), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_robin", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_diamond[Settings::lang] + ":", RectF(Vec2(5, 150), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_dimond", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_amber[Settings::lang] + ":", RectF(Vec2(5, 160), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_amber", { 0,1 }, { 1, 0, 0 }, 1);

			p3->AddTextPF(Settings::lang_kilogram[Settings::lang], RectF(Vec2(100, 40), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_kilogram", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("11", RectF(Vec2(100, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nWood", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nIron", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSand", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("44", RectF(Vec2(100, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nStone", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nCopper", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nGold", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("3", RectF(Vec2(100, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nAluminum", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("4", RectF(Vec2(100, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nEmerald", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 130), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSapphire", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("4", RectF(Vec2(100, 140), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nRobin", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 150), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nDimond", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 160), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nAmber", { 0,1 }, { 1, 0, 0 }, 1);

			//Page 2
			p3->AddTextPF(Settings::lang_steel[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_steel", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_plastic[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_plastic", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_concrete[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_concrete", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_glass[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_glass", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_ceramics[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_ceramics", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_snow[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_snow", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_bricks[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_bricks", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_slate[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_slate", { 0,1 }, { 0, 1, 0 }, 1);

			p3->AddTextPF("11", RectF(Vec2(100, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSteel", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nPlastic", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nConcrete", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("44", RectF(Vec2(100, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nGlass", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nCeramics", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSnow", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nBricks", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSlate", { 0,1 }, { 0, 1, 0 }, 1);

			//Page 3
			p3->AddTextPF(Settings::lang_corals[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_corals", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_sticks[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_sticks", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_leaves[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_leaves", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_wool[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_wool", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_leather[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_leather", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_fur[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_fur", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_meat[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_meat", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_fish[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_fish", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_berrys[Settings::lang] + ":", RectF(Vec2(5, 130), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_berrys", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_apples[Settings::lang] + ":", RectF(Vec2(5, 140), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_apples", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_cactus[Settings::lang] + ":", RectF(Vec2(5, 150), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_cactus", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_units[Settings::lang] + ":", RectF(Vec2(5, 160), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_units", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_maxUnits[Settings::lang] + ":", RectF(Vec2(5, 170), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_maxUnits", { 0,1 }, { 0, 0, 1 }, 1);

			p3->AddTextPF("11", RectF(Vec2(100, 50), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nCorals", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 60), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nSticks", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 70), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nLeaves", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("44", RectF(Vec2(100, 80), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nWool", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 90), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nLeather", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 100), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nFur", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("3", RectF(Vec2(100, 110), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nMeat", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("4", RectF(Vec2(100, 120), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nFish", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 130), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nBerrys", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("4", RectF(Vec2(100, 140), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nApples", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 150), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nCactus", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 160), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nUnits", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 170), 50, 10), 7, &resC->tC.fonts[0], Colors::Black, "t_nMaxUnits", { 0,1 }, { 0, 0, 1 }, 1);
			

			Frame* fUnity = AddFrame("f_Unit", RectF(Vec2(100, 150), 140, 280), 0);																			//Unit Frame
			fUnity->AddText(Settings::lang_unitInfo[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_h");
			fUnity->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(80, 19), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_unitNameIs", a, 1);
			fUnity->AddText(Settings::lang_unitName[Settings::lang] + ":", RectF(Vec2(2, 19), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_unitName", a, 1);
			fUnity->AddText(Settings::lang_hp[Settings::lang] + ":", RectF(Vec2(2, 35), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hp", a, 1);
			fUnity->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(80, 35), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hpIs", a, 1);
			fUnity->AddText(Settings::lang_team[Settings::lang] + ":", RectF(Vec2(2, 51), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_team", a, 1);
			fUnity->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(80, 51), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_teamIs", a, 1);
			fUnity->AddText(Settings::lang_stepsLeft[Settings::lang] + ":", RectF(Vec2(2, 67), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_steps", a, 1);
			fUnity->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(80, 67), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_stepsIs", a, 1);
			fUnity->AddText(Settings::lang_attacksLeft[Settings::lang] + ":", RectF(Vec2(2, 83), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_attacks", a, 1);
			fUnity->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(80, 83), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_attacksIs", a, 1);
			fUnity->SetState(1);
			fUnity->SetVisible(false);
			Button* b_setAttack = fUnity->AddButton(RectF(Vec2(35, 100), 60, 30), &resC->tC.windowsFrame[6], &resC->tC.windowsFrame[6], "b_setAttack", &resC->tC.fonts[0], a);
			b_setAttack->bFunc = BSetAttackMode;
			b_setAttack->text = Settings::lang_attack[Settings::lang]+" (A)";
			b_setAttack->size = 7;
			b_setAttack->hitable = true;
			
			Frame* fTownhall = AddFrame("f_Townhall", RectF(Vec2(100, 150), 140, 280), 0);																			//Unit Frame
			fTownhall->SetState(1);
			Text* text = fTownhall->AddText(Settings::lang_townhall[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_h");
			fTownhall->AddText(Settings::lang_hp[Settings::lang] + ":", RectF(Vec2(2, 95), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hp", a, 1);
			fTownhall->AddText(Settings::lang_noInformation[Settings::lang] , RectF(Vec2(80, 95), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hpIs", a, 1);
			fTownhall->AddText(Settings::lang_attacksLeft[Settings::lang] + ":", RectF(Vec2(2, 110), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_attacks", a, 1);
			fTownhall->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(80, 110), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_attacksIs", a, 1);
			fTownhall->AddText(Settings::lang_team[Settings::lang] + ":", RectF(Vec2(2, 125), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_team", a, 1);
			fTownhall->AddText(Settings::lang_noInformation[Settings::lang] , RectF(Vec2(80, 125), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_teamIs", a, 1);
			fTownhall->AddText(Settings::lang_educate[Settings::lang] + ":", RectF(Vec2(2, 145), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_ausbilden", a, 1);
			fTownhall->AddText(Settings::lang_heal[Settings::lang] + ":", RectF(Vec2(2, 160), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_heilen", a, 1);
			fTownhall->AddText(Settings::lang_recharge[Settings::lang] + ":", RectF(Vec2(2, 175), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_attack", a, 1);
			fTownhall->SetVisible(false);

			TextBox* tB_townHall = fTownhall->AddTextBox(Settings::lang_TownhallInfo[Settings::lang], RectF(Vec2(5, 17), 135, 40), 7, &resC->tC.fonts[0], Colors::Black, "tB_townhallInfo", a);

			CheckBox* cB_educate = fTownhall->AddCheckBox(RectF(Vec2(80, 137),15, 15), &buffer, resC, "cB_educate", a);
			cB_educate->extra1 = 0;
			cB_educate->extra2 = 1;
			cB_educate->extraS1 = "townhall";
			cB_educate->SetBFunc(BSetObstacleState);

			CheckBox* cB_heal = fTownhall->AddCheckBox(RectF(Vec2(80, 157), 15, 15), &buffer, resC, "cB_heal", a);
			cB_heal->extra1 = 2;
			cB_heal->extra2 = 3;
			cB_heal->extraS1 = "townhall";
			cB_heal->SetBFunc(BSetObstacleState);

			CheckBox* cB_attack = fTownhall->AddCheckBox(RectF(Vec2(80, 177), 15, 15), &buffer, resC, "cB_attack", a);
			cB_attack->extra1 = 4;
			cB_attack->extra2 = 5;
			cB_attack->extraS1 = "townhall";
			cB_attack->SetBFunc(BSetObstacleState);

			cB_educate->AddCheckBox2Disable(cB_heal);
			cB_educate->AddCheckBox2Disable(cB_attack);

			cB_heal->AddCheckBox2Disable(cB_attack);
			cB_heal->AddCheckBox2Disable(cB_educate);
			
			cB_attack->AddCheckBox2Disable(cB_heal);
			cB_attack->AddCheckBox2Disable(cB_educate);

			Button* b_THsetAttack = fTownhall->AddButton(RectF(Vec2(35, 200), 60, 30), &resC->tC.windowsFrame[6], &resC->tC.windowsFrame[6], "b_setAttack", &resC->tC.fonts[0], a);
			b_THsetAttack->bFunc = BSetAttackMode;
			b_THsetAttack->text = Settings::lang_attack[Settings::lang];
			b_THsetAttack->size = 7;
			b_THsetAttack->hitable = true;

			Frame* fLumberjackHut = AddFrame("f_LumberjackHut", RectF(Vec2(100, 150), 140, 280), 0);																			//Lumberjack Frame
			fLumberjackHut->SetState(1);
			fLumberjackHut->AddText(Settings::lang_lumberjackHut[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_h");
			fLumberjackHut->AddText(Settings::lang_hp[Settings::lang] + ":", RectF(Vec2(2, 95), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hp", a, 1);
			fLumberjackHut->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(80, 95), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_hpIs", a, 1);
			fLumberjackHut->AddText(Settings::lang_chopsLeft[Settings::lang] + ":", RectF(Vec2(2, 110), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_attacks", a, 1);
			fLumberjackHut->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(80, 110), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_attacksIs", a, 1);
			fLumberjackHut->AddText(Settings::lang_team[Settings::lang] + ":", RectF(Vec2(2, 125), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_team", a, 1);
			fLumberjackHut->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(80, 125), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_teamIs", a, 1);
			fLumberjackHut->AddText(Settings::lang_automatic[Settings::lang] + ":", RectF(Vec2(2, 145), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, "t_ausbilden", a, 1);
			fLumberjackHut->SetVisible(false);

			fLumberjackHut->AddTextBox(Settings::lang_LumberjackHutInfo[Settings::lang], RectF(Vec2(5, 17), 135, 40), 7, &resC->tC.fonts[0], Colors::Black, "tB_lumberjackHutInfo", a);

			CheckBox* cB_chop = fLumberjackHut->AddCheckBox(RectF(Vec2(80, 137), 15, 15), &buffer, resC, "cB_automatic", a);
			cB_chop->extra1 = 0;
			cB_chop->extra2 = 1;
			cB_chop->extraS1 = "lumberjackHut";
			cB_chop->SetBFunc(BSetObstacleState);

			Button* b_THsetChop = fLumberjackHut->AddButton(RectF(Vec2(35, 200), 60, 30), &resC->tC.windowsFrame[6], &resC->tC.windowsFrame[6], "b_setAttack", &resC->tC.fonts[0], a);
			b_THsetChop->bFunc = BSetAttackMode;
			b_THsetChop->text = Settings::lang_chop[Settings::lang];
			b_THsetChop->size = 7;
			b_THsetChop->hitable = true;

			//Frame* fNextTurn = AddFrame(RectF(Vec2(1120, 600), 120, 60), 1);																			//NEXT TURN FRAME
			//fNextTurn->s = &resC->tC.windowsFrame[3].GetCurSurface();
			//fNextTurn->bFunc = BNextTurn;

			comps["b_NextTurn"] = std::make_unique<Button>(Button(RectF(Vec2(1120, 600), 120, 60), &resC->tC.windowsFrame[3], &resC->tC.windowsFrame[3], { 0,0 }, &resC->tC.fonts[0], nullptr, &buffer));
			static_cast<Button*>(comps["b_NextTurn"].get())->bFunc = BNextTurn;

			comps["b_buildScene"] = std::make_unique<Button>(Button(RectF(Vec2(30, 60), 60, 30), &resC->tC.windowsFrame[6], &resC->tC.windowsFrame[6], { 0,0 }, &resC->tC.fonts[0], nullptr, &buffer));
			Button* button = static_cast<Button*>(comps["b_buildScene"].get());
			button->extra1 = 1;
			button->bFunc = BLoadScene;
			button->text = Settings::lang_build[Settings::lang];

			comps["b_wroughScene"] = std::make_unique<Button>(Button(RectF(Vec2(30, 100), 60, 30), &resC->tC.windowsFrame[6], &resC->tC.windowsFrame[6], { 0,0 }, &resC->tC.fonts[0], nullptr, &buffer));
			button = static_cast<Button*>(comps["b_wroughScene"].get());
			button->extra1 = 2;
			button->SetVisible(false);
			button->bFunc = BLoadScene;
			button->text = Settings::lang_forge[Settings::lang];
			/*
			Frame* fButtonBuild = AddFrame(RectF(Vec2(30, 30), 60, 30), 1);																			//BUILD MENU BUTTON
			fButtonBuild->s = &resC->tC.windowsFrame[4].GetCurSurface();
			fButtonBuild->bFunc = BBuildMenu;
			*/
			Frame* fInventory = AddFrame("f_Inventory", RectF(Vec2(Graphics::ScreenWidth/2 - 125, Graphics::ScreenHeight/12), 250, 191), 1);
			fInventory->s = &resC->tC.windowsFrame[8].GetCurSurface();
			fInventory->SetVisible(false);

			GrabImage* hand1 = CreateGIWithHpBar(fInventory, RectF(Vec2(70, 10), 50, 50), &resC->tC.items[0], &resC->tC.items[0], &buffer,"gI_item0", { 1,1 });
			hand1->extra1 = 0;
			hand1->extraS1 = "inventory swap unit";

			GrabImage* bonus2 = CreateGIWithHpBar(fInventory, RectF(Vec2(130, 10), 50, 50), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item1", { 1,1 });
			bonus2->extra1 = 1;
			bonus2->extraS1 = "inventory swap unit";
			
			GrabImage* armor = CreateGIWithHpBar(fInventory, RectF(Vec2(70, 70), 50, 50), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item2", { 1,1 });
			armor->SetVisible(false);
			armor->extra1 = 2;
			armor->extraS1 = "inventory swap unit";
			
			GrabImage* bonus1 = CreateGIWithHpBar(fInventory, RectF(Vec2(130, 70), 50, 50), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item3", { 1,1 });
			bonus1->SetVisible(false);
			bonus1->extra1 = 3;
			bonus1->extraS1 = "inventory swap unit";

			GrabImage* item1 = CreateGIWithHpBar(fInventory, RectF(Vec2(10, 130), 50, 50), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item4", { 1,1 });
			item1->SetVisible(false);
			item1->extra1 = 4;
			item1->extraS1 = "inventory swap unit";
			
			GrabImage* item2 = CreateGIWithHpBar(fInventory, RectF(Vec2(70, 130), 50, 50), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item5", { 1,1 });
			item2->SetVisible(false);
			item2->extra1 = 5;
			item2->extraS1 = "inventory swap unit";
			
			GrabImage* item3 = CreateGIWithHpBar(fInventory, RectF(Vec2(130, 130), 50, 50), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item6", { 1,1 });
			item3->SetVisible(false);
			item3->extra1 = 6;
			item3->extraS1 = "inventory swap unit";
			
			GrabImage* item4 = CreateGIWithHpBar(fInventory, RectF(Vec2(190, 130), 50, 50), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item7", { 1,1 });
			item4->SetVisible(false);
			item4->extra1 = 7;
			item4->extraS1 = "inventory swap unit";
			
			Frame* fInventoryBox = AddFrame("f_InventoryBox", RectF(Vec2(Graphics::ScreenWidth / 8 * 5, Graphics::ScreenHeight / 12), 190, 190), 1);
			fInventoryBox->s = &resC->tC.windowsFrame[9].GetCurSurface();
			fInventoryBox->SetVisible(false);

			for (int i = 0; i < 9; i++)
			{
				GrabImage* image = CreateGIWithHpBar(fInventoryBox, RectF(Vec2(10.f + (int)(i % 3) * 60.f, 10.f + (int)(i / 3) * 60.f), 50.f, 50.f), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item" + std::to_string(i), { 1,1 });
				image->SetVisible(false);
				image->extra1 = i;
				image->extraS1 = "inventory swap box";
			}

			Frame* fInventoryStorage = AddFrame("f_InventoryStorage", RectF(Vec2(Graphics::ScreenWidth / 8 * 5, Graphics::ScreenHeight / 12), 310, 310), 1);
			fInventoryStorage->s = &resC->tC.windowsFrame[10].GetCurSurface();
			fInventoryStorage->SetVisible(false);

			for (int i = 0; i < 25; i++)
			{
				GrabImage* image = CreateGIWithHpBar(fInventoryStorage, RectF(Vec2(10.f + (int)(i % 5) * 60, 10.f + (int)(i / 5) * 60), 50.f, 50.f), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item" + std::to_string(i), { 1,1 });
				image->SetVisible(false);
				image->extra1 = i;
				image->extraS1 = "inventory swap storage";
			}

			Frame* fInventoryWrought = AddFrame("f_InventoryWrought", RectF(Vec2(Graphics::ScreenWidth / 8 * 5, Graphics::ScreenHeight / 12), 190, 190), 1);
			fInventoryWrought->s = &resC->tC.windowsFrame[11].GetCurSurface();
			fInventoryWrought->SetVisible(false);

			text = fInventoryWrought->AddText(Settings::lang_craftedItem[Settings::lang] + ":", RectF(Vec2(10, 15), 50, 8), 14, &resC->tC.fonts[0], Colors::Black, "t_craft", { 1,1 }, 1);
			text->SetVisible(true);
			text = fInventoryWrought->AddText(Settings::lang_reparation[Settings::lang] + ":", RectF(Vec2(10, 110), 50, 8), 14, &resC->tC.fonts[0], Colors::Black, "t_rep", { 1,1 }, 1);
			text->SetVisible(true);

			for (int i = 0; i < 6; i++)
			{
				GrabImage* image = CreateGIWithHpBar(fInventoryWrought, RectF(Vec2(10.f + (int)(i % 3) * 60.f, 40.f + (int)(i / 3) * 90.f), 50.f, 50.f), &resC->tC.items[0], &resC->tC.items[0], &buffer, "gI_item" + std::to_string(i), { 1,1 });
				image->SetVisible(false);
				image->extra1 = i;
				image->extraS1 = "inventory swap wrought";
			}
		}
		else if(scene == 1)
		{
			std::vector<int> a = { 0,1 };  
			PageFrame * fBuildSelection = AddPageFrame("f_bg",RectF(Vec2(0, 0), Graphics::ScreenWidth, Graphics::ScreenHeight), 1, 4);															//build selection menu
			fBuildSelection->s = &resC->tC.windowsFrame[5].GetCurSurface();
			fBuildSelection->SetMoveable(false);
			fBuildSelection->GetComp("b_left")->pos = RectF(Vec2(25, 25), 100, 25);
			fBuildSelection->GetComp("b_right")->pos = RectF(Vec2(Graphics::ScreenWidth - 125, 25), 100, 25);
			fBuildSelection->AddTextPF(Settings::lang_housing[Settings::lang], RectF(Vec2(470, 30), 300, 60), 50, &resC->tC.fonts[0], Colors::Black, "HousingH", { 1 }, { 1,0,0,0 });

			Button* b_back = fBuildSelection->AddButtonPF(RectF(Vec2(30, 60), 60, 30), &resC->tC.windowsFrame[6], &resC->tC.windowsFrame[6], "b_buildback", &resC->tC.fonts[0], { 1 }, { 1,1,1,1 });
			b_back->extra1 = 0;
			b_back->bFunc = BLoadScene;
			b_back->text = Settings::lang_back[Settings::lang];

			CreateBuildOption(RectF(Vec2(60, 120), 180, 60), 0, fBuildSelection, { 1,0,0,0 },world);
			CreateBuildOption(RectF(Vec2(60, 190), 180, 60), 2, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 260), 180, 60), 3, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 330), 180, 60), 21, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 400), 180, 60), 22, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 470), 180, 60), 23, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 540), 180, 60), 24, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 610), 180, 60), 25, fBuildSelection, { 1,0,0,0 }, world);

			CreateBuildOption(RectF(Vec2(250, 190), 180, 60), 26, fBuildSelection, { 1,0,0,0 }, world);

			fBuildSelection->AddTextPF(Settings::lang_productions[Settings::lang], RectF(Vec2(470, 30), 300, 60), 50, &resC->tC.fonts[0], Colors::Black, "roductionH", { 1 }, { 0,1,0,0 });

			CreateBuildOption(RectF(Vec2(60, 120), 180, 60), 27, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 190), 180, 60), 28, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 260), 180, 60), 29, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 330), 180, 60), 30, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 400), 180, 60), 31, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 470), 180, 60), 32, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 540), 180, 60), 33, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 610), 180, 60), 34, fBuildSelection, { 0,1,0,0 }, world);

			CreateBuildOption(RectF(Vec2(250, 120), 180, 60), 35, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 190), 180, 60), 36, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 260), 180, 60), 37, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 330), 180, 60), 38, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 400), 180, 60), 39, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 470), 180, 60), 40, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 540), 180, 60), 41, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 610), 180, 60), 42, fBuildSelection, { 0,1,0,0 }, world);

			CreateBuildOption(RectF(Vec2(440, 120), 180, 60), 48, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(440, 190), 180, 60), 49, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(440, 260), 180, 60), 50, fBuildSelection, { 0,1,0,0 }, world);

		}
		else if (scene == 2)
		{
			std::vector<int> a = { 0,1 };
			Frame* fCraftSelection = AddFrame("f_craftMenu", RectF(Vec2(0, 0), Graphics::ScreenWidth, Graphics::ScreenHeight), 1);//AddPageFrame("f_bg", RectF(Vec2(0, 0), Graphics::ScreenWidth, Graphics::ScreenHeight), 1, 4);															//build selection menu
			fCraftSelection->s = &resC->tC.windowsFrame[5].GetCurSurface();
			fCraftSelection->SetMoveable(false);
			fCraftSelection->AddText(Settings::lang_forge[Settings::lang], RectF(Vec2(470, 30), 300, 60), 50, &resC->tC.fonts[0], Colors::Black, "ForgeH", { 1 });

			Button* b_back = fCraftSelection->AddButton(RectF(Vec2(30, 100), 60, 30), &resC->tC.windowsFrame[6], &resC->tC.windowsFrame[6], "b_buildback", &resC->tC.fonts[0], { 1 });
			b_back->extra1 = 0;
			b_back->bFunc = BLoadScene;
			b_back->text = Settings::lang_back[Settings::lang];

			CreateCraftOption(RectF(Vec2(60, 190), 180, 60), 0, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 260), 180, 60), 1, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 330), 180, 60), 2, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 400), 180, 60), 9, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 470), 180, 60), 10, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 540), 180, 60), 11, fCraftSelection, { 1,0,0,0 }, world);
		}
	}
	//HELP CREATE FUNCTIONS
	Frame* CreateBuildOption(RectF pos, int obstacleType, PageFrame* parentC, std::vector<int> activOnPages, World* world)
	{
		std::string key = Settings::GetObstacleString(obstacleType);
		Frame* frame = parentC->AddFramePF(pos, key, { 1 }, activOnPages);
		frame->s = &resC->tC.windowsFrame[7].GetCurSurface();
		frame->extra1 = obstacleType;
		frame->bFunc = BBuildMode;
		frame->hitable = true;
		frame->AddText(Settings::GetObstacleString(obstacleType), RectF(Vec2(60, 5), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, key + "H", { 1 });
		std::map<std::string, float> neededRes = Settings::obstacleStats[obstacleType].neededResToBuild;
		std::map<std::string, float>::iterator res;
		int i = 0;
		for (res = neededRes.begin(); res != neededRes.end(); res++)
		{
			//std::map<st::string,float> check ={{}}
			if (world->GetPlayer()->GetMaterials().Has({ { res->first,res->second } }))
				frame->AddText("-"+res->first+": x"+std::to_string(res->second)+" "+ Settings::lang_kilogram[Settings::lang], RectF(Vec2(70.f, 20.f +i*10.f), 50.f, 8.f), 7, &resC->tC.fonts[0], Colors::Green, key + "res"+std::to_string(i), { 1 });
			else
			{
				frame->AddText("-" + res->first + ": x" + std::to_string(res->second) + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(70.f, 20.f + i * 10.f), 50.f, 8.f), 7, &resC->tC.fonts[0], Colors::Red, key + "res" + std::to_string(i), { 1 });
			}
			i++;
		}
		//Frame* frame = AddFrame(key, pos, 0);
		return nullptr;// static_cast<Frame*>(comps[key].get());
	}
	Frame* CreateCraftOption(RectF pos, int itemType, Frame* parentC, std::vector<int> activOnPages, World* world)
	{
		std::string key = Settings::GetItemString(itemType);
		//Frame* frame = parentC->AddFramePF(pos, key, { 1 }, activOnPages);
		Frame* frame = parentC->AddFrame(pos,1,resC,parentC,&buffer, key, { 1 });
		frame->s = &resC->tC.windowsFrame[7].GetCurSurface();
		frame->extra1 = itemType;
		frame->bFunc = BCraftingQueue;
		frame->hitable = true;
		frame->AddText(Settings::GetItemString(itemType), RectF(Vec2(60, 5), 50, 8), 7, &resC->tC.fonts[0], Colors::Black, key + "H", { 1 });
		std::map<std::string, float> neededRes = Settings::itemStats[itemType].neededResToCraft;
		std::map<std::string, float>::iterator res;
		int i = 0;
		for (res = neededRes.begin(); res != neededRes.end(); res++)
		{
			//std::map<st::string,float> check ={{}}
			if (world->GetPlayer()->GetMaterials().Has({ { res->first,res->second } }))
				frame->AddText("-" + res->first + ": x" + std::to_string(res->second) + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(70.f, 20.f + i * 10.f), 50.f, 8.f), 7, &resC->tC.fonts[0], Colors::Green, key + "res" + std::to_string(i), { 1 });
			else
			{
				frame->AddText("-" + res->first + ": x" + std::to_string(res->second) + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(70.f, 20.f + i * 10.f), 50.f, 8.f), 7, &resC->tC.fonts[0], Colors::Red, key + "res" + std::to_string(i), { 1 });
			}
			i++;
		}
		//Frame* frame = AddFrame(key, pos, 0);
		return nullptr;// static_cast<Frame*>(comps[key].get());
	}
	GrabImage* CreateGIWithHpBar(Component* parentC, RectF pos, Animation* a, Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates)
	{
		GrabImage* gi = parentC->AddGrabImage(pos,a,aHover,buffer,key,activInStates);
		gi->SetVisible(false);

		Image* image = gi->AddImage(RectF(Vec2(0, pos.GetHeight() / 5 * 4), pos.GetWidth(), pos.GetHeight() / 5), &resC->tC.frames[1], &resC->tC.frames[1], buffer, key+"Hp");
		image = gi->AddImage(RectF(Vec2(0, pos.GetHeight() / 5 * 4), pos.GetWidth(), pos.GetHeight() / 5), &resC->tC.frames[2], &resC->tC.frames[2], buffer, key + "HpIs");
		return gi;
	}
	// ### Update components of existing Frames ###

	// ###### Update existing Frames ######
	void UpdateFieldinformation(World& curW, Team* player)				
	{
		if (Settings::framesOn)
		{

			MultiFrame* m = static_cast<MultiFrame*>(comps["f_resD"].get());
			Materials& playerM = player->GetMaterials();
			//#1
			Frame* f1 = static_cast<Frame*>(m->GetFrame("fresD_f1"));
			f1->SetText(Settings::GetTypeString(curW.GetfCellType()), "t_cellType");
			if (curW.GetFocusedObstacle() != nullptr)
			{
				f1->SetText(Settings::GetObstacleString(curW.GetFocusedObstacle()->type), "t_obstacleInfo");
				f1->SetText(std::to_string(curW.GetFocusedObstacle()->hp)+" / "+ std::to_string(Settings::obstacleStats[curW.GetFocusedObstacle()->type].baseHp), "t_obstacleHpIs");
			}

			//#2

			//#3
			PageFrame* p3 = static_cast<PageFrame*>(m->GetFrame("fresD_f2"));
			
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
			p3->SetText(std::to_string(playerM.values["units"]), "t_nUnits");
			p3->SetText(std::to_string(playerM.values["maxUnits"]), "t_nMaxUnits");

		}
	}
	void UpdateFrames(Obstacle* obst, Obstacle* storage)
	{
		Frame* f_unit = static_cast<Frame*>(comps["f_Unit"].get());
		f_unit->SetVisible(false);
		Frame* f_townhall = static_cast<Frame*>(comps["f_Townhall"].get());
		f_townhall->SetVisible(false);
		Frame* f_lumberjackHut = static_cast<Frame*>(comps["f_LumberjackHut"].get());
		f_lumberjackHut->SetVisible(false);
		Frame* f_Inventory = static_cast<Frame*>(comps["f_Inventory"].get());
		f_Inventory->SetVisible(false);
		Frame* f_InventoryBox = static_cast<Frame*>(comps["f_InventoryBox"].get());
		f_InventoryBox->SetVisible(false);
		Frame* f_InventoryStorage = static_cast<Frame*>(comps["f_InventoryStorage"].get());
		f_InventoryStorage->SetVisible(false);
		Frame* f_InventoryWrought = static_cast<Frame*>(comps["f_InventoryWrought"].get());
		f_InventoryWrought->SetVisible(false);

		//buttons
		Button* b_wroughScene = static_cast<Button*>(comps["b_wroughScene"].get());
		b_wroughScene->SetVisible(false);

		if (obst != nullptr)
		{
			if (Settings::anyOfCreature(obst->type))
			{
				f_unit->SetVisible(true);
				std::string s1 = Settings::GetObstacleString(obst->type);

				f_unit->SetText(s1, "t_unitNameIs");
				f_unit->SetText(std::to_string(obst->hp) + " / " + std::to_string(Settings::obstacleStats[obst->type].baseHp), "t_hpIs");
				f_unit->SetText(std::to_string(obst->stepsLeft), "t_stepsIs");
				f_unit->SetText(std::to_string(obst->attack->GetAttacksLeft()), "t_attacksIs");
				if (obst->team != nullptr)
				{
					f_unit->SetText(obst->team->GetTeamName(), "t_teamIs");
				}
				else
				{
					f_unit->SetText(Settings::lang_noInformation[Settings::lang], "t_teamIs");
				}
				f_Inventory->SetVisible(true);
				
				Inventory& inv = *obst->inv;

				UpdateInventoryComps(obst->inv.get(), f_Inventory);
			}
			if (obst->type == 3)
			{
				f_townhall->SetVisible(true);
				//t_teamIs
				f_townhall->SetText(std::to_string(obst->hp) + " / " + std::to_string(Settings::obstacleStats[obst->type].baseHp), "t_hpIs");
				f_townhall->SetText(std::to_string(obst->attack->GetAttacksLeft()) + " / " + std::to_string(Settings::obstacleStats[obst->type].attacksPerTurn), "t_attacksIs");
				if (obst->team != nullptr)
				{
					f_townhall->SetText(obst->team->GetTeamName(), "t_teamIs");
				}
				else
				{
					f_townhall->SetText(Settings::lang_noInformation[Settings::lang], "t_teamIs");
				}
				if (obst->education->Educates())
					static_cast<CheckBox*>(f_townhall->GetComp("cB_educate"))->Check();
				else
					static_cast<CheckBox*>(f_townhall->GetComp("cB_educate"))->Uncheck();
				if (obst->heal->Isenabled())
					static_cast<CheckBox*>(f_townhall->GetComp("cB_heal"))->Check();
				else
					static_cast<CheckBox*>(f_townhall->GetComp("cB_heal"))->Uncheck();
				if (obst->attack->GetReloadNextTurn())
					static_cast<CheckBox*>(f_townhall->GetComp("cB_attack"))->Check();
				else
					static_cast<CheckBox*>(f_townhall->GetComp("cB_attack"))->Uncheck();
			}
			if (obst->type == 27)
			{
				f_lumberjackHut->SetVisible(true);
				//t_teamIs
				f_lumberjackHut->SetText(std::to_string(obst->hp) + " / " + std::to_string(Settings::obstacleStats[obst->type].baseHp), "t_hpIs");
				f_lumberjackHut->SetText(std::to_string(obst->attack->GetAttacksLeft()) + " / " + std::to_string(Settings::obstacleStats[obst->type].attacksPerTurn), "t_attacksIs");
				if (obst->team != nullptr)
				{
					f_lumberjackHut->SetText(obst->team->GetTeamName(), "t_teamIs");
				}
				else
				{
					f_lumberjackHut->SetText(Settings::lang_noInformation[Settings::lang], "t_teamIs");
				}

			}
		}
		if (storage != nullptr)
		{
			if (storage->type == 6)
			{
				f_InventoryBox->SetVisible(true);
				UpdateInventoryComps(storage->inv.get(), f_InventoryBox);
			}
			if (storage->type == 30)
			{
				f_InventoryWrought->SetVisible(true);
				b_wroughScene->SetVisible(true);
				UpdateInventoryComps(storage->inv.get(), f_InventoryWrought);

			}
			if (storage->type == 50)
			{
				f_InventoryStorage->SetVisible(true);
				UpdateInventoryComps(storage->inv.get(), f_InventoryStorage);
			}
		}
	}
	void UpdateInventoryComps(Inventory* inv, Component* parentC)
	{
		for (int i = 0; i < inv->GetSize(); i++)
		{
			std::string key = "gI_item" + std::to_string(i);
			if (inv->GetItem(i)->get() != nullptr)
			{
				parentC->GetComp(key)->SetVisible(true);
				static_cast<GrabImage*>(parentC->GetComp(key))->SetAnimationOfBouth(&resC->tC.items[inv->GetItem(i)->get()->GetId()]);
				if (inv->GetItem(i)->get()->GetDurability() != -1 && inv->GetItem(i)->get()->GetDurability() != Settings::itemStats[inv->GetItem(i)->get()->GetId()].durability)
				{
					static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "HpIs"))->SetVisible(true);
					static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "Hp"))->SetVisible(true);
					float percentage = (float)inv->GetItem(i)->get()->GetDurability() / Settings::itemStats[inv->GetItem(i)->get()->GetId()].durability;
					static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "HpIs"))->SetDrawPercent(Vec2(percentage, 1.f));
				}
				else
				{
					static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "HpIs"))->SetVisible(false);
					static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "Hp"))->SetVisible(false);
				}
			}
			else
			{
				parentC->GetComp(key)->SetVisible(false);
			}
		}
	}
	int GetHitInventorySpace(Vec2 mP)
	{
		Frame* f_Inventory = static_cast<Frame*>(comps["f_Inventory"].get());
		if (f_Inventory->IsVisible())
		{
			for (int i = 0; i < 8; i++)
			{
				std::string key = "gI_item" + std::to_string(i);
				if (static_cast<GrabImage*>(f_Inventory->GetComp(key))->GetPos().Contains(mP))
				{
					return f_Inventory->GetComp(key)->extra1;
				}
			}
		}
		return -1;
	}
	int GetHitInventoryBox(Vec2 mP)//f_InventoryBox    f_InventoryStorage
	{
		Frame* f_InventoryBox = static_cast<Frame*>(comps["f_InventoryBox"].get());
		if (f_InventoryBox->IsVisible())
		{
			for (int i = 0; i < 9; i++)
			{
				std::string key = "gI_item" + std::to_string(i);
				if (static_cast<GrabImage*>(f_InventoryBox->GetComp(key))->GetPos().Contains(mP))
				{
					return f_InventoryBox->GetComp(key)->extra1;
				}
			}
		}
		return -1;
	}
	int GetHitInventoryStorage(Vec2 mP)//f_InventoryBox    f_InventoryStorage
	{
		Frame* f_InventoryStorage = static_cast<Frame*>(comps["f_InventoryStorage"].get());
		if (f_InventoryStorage->IsVisible())
		{
			for (int i = 0; i < 25; i++)
			{
				std::string key = "gI_item" + std::to_string(i);
				if (static_cast<GrabImage*>(f_InventoryStorage->GetComp(key))->GetPos().Contains(mP))
				{
					return f_InventoryStorage->GetComp(key)->extra1;
				}
			}
		}
		return -1;
	}
	int GetHitInventoryWrought(Vec2 mP)//f_InventoryBox    f_InventoryStorage
	{
		Frame* f_InventoryWrought = static_cast<Frame*>(comps["f_InventoryWrought"].get());
		if (f_InventoryWrought->IsVisible())
		{
			for (int i = 0; i < 6; i++)
			{
				std::string key = "gI_item" + std::to_string(i);
				if (static_cast<GrabImage*>(f_InventoryWrought->GetComp(key))->GetPos().Contains(mP))
				{
					return f_InventoryWrought->GetComp(key)->extra1;
				}
			}
		}
		return -1;
	}
	static constexpr float percentForGrab = 0.05;			
};

