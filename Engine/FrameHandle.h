#pragma once
#include "World.h"
#include <memory>
#include <map>
#include <algorithm>
#include "Graphics.h"
#include <sstream>
#include <iomanip>
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
	const std::map<std::string, std::unique_ptr<Component>>* GetComps() {
		return &comps;
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

	Text* AddText(std::string text, RectF pos, int size,const Font* f, Color c, std::string key, std::vector<int> activInStates = {}, int textLoc = 0);
	TextBox* AddTextBox(std::string text, RectF pos, int size,const Font* f, Color c, std::string key, std::vector<int> activInStates = {}, int textLoc = 0);
	Button* AddButton(RectF pos,const Animation* a, const Animation* aHover, std::string key, const Font* f, std::vector<int> activInStates = {});
	CheckBox* AddCheckBox(RectF pos, std::queue<FrameEvent>* buffer, sharedResC resC, std::string key, std::vector<int> activInStates = {});
	Image* AddImage(RectF pos, const Animation* a, const Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates = {});
	GrabImage* AddGrabImage(RectF pos, const Animation* a, const Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates = {});
	Frame* AddFrame(RectF pos, int type, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates = {});
};
class Text: public Component
{
public:
	const Font* f;
	int size, textLoc;			//		'0' = centered			 '1' = left
	Text(std::string text, RectF pos, int size,const Font* f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc, std::queue<FrameEvent>* buffer);
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
			gfx.DrawTextCentered(text, Vei2((int)(drawPos.left), (int)(drawPos.top)), size, f, SpriteEffect::ChromaColor(Colors::Magenta, c));
			//f->DrawTextCentered(text, drawPos.GetTopLeft<int>(), size, c);
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
			gfx.DrawText(text, (int)(drawPos.left), (int)(drawPos.top), size, f, SpriteEffect::ChromaColor(Colors::Magenta,c));
			//f->DrawText(text, (int)(drawPos.left), (int)(drawPos.top), size, c);
		}
		//gfx.DrawRect(GetPos(), Colors::Red);
	}
};
class TextBox : public Text
{
protected:
	std::vector<std::string> lines;
public:
	TextBox(std::string text, RectF pos, int size, const Font* f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc, std::queue<FrameEvent>* buffer);
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
			gfx.DrawText(lines[i], (int)(drawPos.left), (int)(drawPos.top) + i * (size + 2), size, f, SpriteEffect::ChromaColor(Colors::Magenta, c));
		}
	}
	void Settext(std::string text)
	{
		lines.clear();
		lines = SplitTextToLines(f, text, size, GetPos().GetWidth());
	}
	std::string GetText()const
	{
		std::string text = "";
		for (int i = 0; i < lines.size(); i++)
		{
			text += lines[i];
		}
		return text;
	}
	std::vector<std::string> SplitTextToLines(const Font* font,std::string text, int size, int width);

	//
	const std::vector<std::string>& GetLines()const;
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
			gfx.DrawSurface((RectI)GetPos(), resC->GetSurf().buttons[5].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
		}
		else
		{
			gfx.DrawSurface((RectI)GetPos(), resC->GetSurf().buttons[4].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
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
	const Animation* a = nullptr;
	const Animation* aHover = nullptr;
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

	Button(RectF pos, const Animation* a, const Animation* aHover, std::vector<int> activInStates, const Font* f, Component* parentC, std::queue<FrameEvent>* buffer);
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
	const Animation* a;
	const Animation* aHover;
	Vec2 drawPercent = Vec2(1.f, 1.f);
	float alpha = 1.f;
public:
	Image(RectF pos, const Animation* a, const Animation* aHover, Component* parentC, std::queue<FrameEvent>* buffer, std::vector<int> activInStates);
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
	void SetAnimation(const Animation* a)
	{
		this->a = a;
	}
	void SetAnimationOfBouth(const Animation* a)
	{
		this->a = a;
		this->aHover = a;

	}
	void SetHoverAnimation(const Animation* a)
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
	GrabImage(RectF pos, const Animation* a, const Animation* aHover, Component* parentC, std::queue<FrameEvent>* buffer, std::vector<int> activInStates);
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
						gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), (int)(pos.GetWidth()), (int)(pos.GetHeight() / 20)), resC->GetSurf().windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						break;
					case 1:
						gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), (int)(pos.GetWidth()), (int)(pos.GetHeight() / 20)), resC->GetSurf().windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						gfx.DrawSurface((RectI)cPos, resC->GetSurf().windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.9f));
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
						gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), (int)(pos.GetWidth()), (int)(pos.GetHeight() / 20)), resC->GetSurf().windowsFrame[1].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						break;
					case 1:
						gfx.DrawSurface(RectI(cPos.GetTopLeft<int>(), (int)(pos.GetWidth()), (int)(pos.GetHeight() / 20)), resC->GetSurf().windowsFrame[2].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						gfx.DrawSurface((RectI)cPos, resC->GetSurf().windowsFrame[0].GetCurSurface(), SpriteEffect::Transparent(Colors::Magenta, 0.8f));
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
			if (e.GetType() == Mouse::Event::Type::LRelease)
			{
				int k = 23;
			}
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
	void AddButtonPF(RectF pos, const Animation* a, const Animation* aHover, std::vector<int> activInStates = {})
	{
		static_assert (fail<T>::value, "Do not use!");
	}

	Text* AddTextPF(std::string text, RectF pos, int size,const Font* f, Color c, std::string key, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {}, int textLoc = 0)
	{
		activOnPages = FillWith1WhenSize0(activOnPages, nPages);
		assert(activOnPages.size() == nPages);
		compActivOnPages[key] = activOnPages;
		return Component::AddText(text, pos, size, f, c, key, activInStates, textLoc);
	}
	Button* AddButtonPF(RectF pos,const Animation* a, const Animation* aHover, std::string key,const Font* f, std::vector<int> activInStates = {}, std::vector<int> activOnPages = {})
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

class MultiFrame : public Frame
{
	std::map<std::string, std::unique_ptr<Frame>> frames;
	std::map<int, std::string> frameOrder;
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
			for (frame = frames.begin(); frame != frames.end(); frame++)
			{
				bool hitFrame = frame->second->HandleMouseInput(e, interact);
				hit = hit || hitFrame;
				if (hitFrame)
				{
					
					for(int i=0;i<100;i++)
					{
						if (frameOrder.count(i) > 0)
						{
							std::string key = frameOrder[i];
							Frame* innerFrame = frames[key].get();
							innerFrame->pos.top += yMove;
							innerFrame->pos.bottom += yMove;
							if (extended[key] != (bool)innerFrame->GetCurState())
							{
								changed = key;
								if (extended[key])
								{
									yMove -= frame->second->GetExtendedHeight();
								}
								if (!extended[key])
								{
									yMove += frame->second->GetExtendedHeight();
								}
							}
						}
					}
				}
			}
			yMove = 0;
			if (changed != "-")
			{
				for (int i = 0; i < 100; i++)
				{
					if (frameOrder.count(i) > 0)
					{
						std::string key = frameOrder[i];
						Frame* innerFrame = frames[key].get();

						innerFrame->pos.top += yMove;
						innerFrame->pos.bottom += yMove;
						if (innerFrame->IsExtended() && key != changed)
						{
							innerFrame->SetState(0);
							yMove -= innerFrame->GetExtendedHeight();
						}
					}
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
	Frame* GetFrame(int index)
	{
		assert(index >= 0);
		int i = 0;
		std::map<std::string, std::unique_ptr<Frame>>::iterator frame;
		for (frame = frames.begin(); frame != frames.end(); frame++)
		{
			if (i == index)
			{
				return frame->second.get();
			}
			i++;
		}
		assert(false);
	}
	std::string GetKey(int index)
	{
		assert(index >= 0);
		int i = 0;
		std::map<std::string, std::unique_ptr<Frame>>::iterator frame;
		for (frame = frames.begin(); frame != frames.end(); frame++)
		{
			if (i == index)
			{
				return frame->first;
			}
			i++;
		}
		assert(false);
	}
	int GetFrameAmount()
	{
		return frames.size();
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

class FrameHandle
{
private:
	std::map<std::string, std::unique_ptr<Component>> comps;
	sharedResC resC;
	RectF overallParent = Graphics::GetScreenRect<float>();
	std::vector<Component*> handleOrder;

	int curScene = 0;
	std::queue<FrameEvent> buffer;
	static constexpr unsigned int bufferSize = 4u;
	Frame* AddFrame(std::string key,RectF pos, int type);
	PageFrame* AddPageFrame(std::string key, RectF pos, int type, int nPages);
	MultiFrame* AddMultiFrame(std::string key, RectF pos, int type, int nStates);

	//Compositions of Components
	Frame* CreateBuildOption(RectF pos, int obstacleType, PageFrame* parentC, std::vector<int> activOnPages, World* world);
	Frame* CreateCraftOption(RectF pos, int itemType, Frame* parentC, std::vector<int> activOnPages, World* world);
	GrabImage* CreateGIWithHpBar(Component* parentC, RectF pos, const Animation* a, const Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates);
	void AddHeadline(Component* parentC, std::string text, const Font* f, Color c);
	int AddInfo(Component* parentC, int top, const Font* f, Color c, std::string key, std::string infoText = "###");
	int AddInfoTextBox(Component* parentC, std::string text, int top, const Font* f, Color c, std::string key);
	int AddObstacleAttackButton(Component* parentC, int top, const Font* f, Color c);
	int AddObstacleCheckBox(Component* parentC, int top, const Font* f, Color c, std::string text, std::string key);

	std::string GetInfoString(std::string key);
	bool FrameIsEnabled(Obstacle* obstacle, std::string key);
	void SetCheckboxDisabling(std::vector<CheckBox*> checkboxes);

	// ###### single Frame operations #####
	void LoadFrame(std::string key);
	void UpdateFrame(const World* world, int process, std::string key, Component* comp);
	void DeleteFrame(std::string key);

	//
	void CreateFrameUnit(const std::map<std::string, std::string>* parsedText = nullptr);

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
	void UpdateHandleOrder();
	//	###### handle Frames #####
	void LoadScene(int scene, World* world);
	void UpdateFrames(const World* world);
	void DeterminateUpdateProc(const World* world, Component* comp, std::string key);
	// ###### single Frame operations #####
	void UpdateFieldinformation(World& curW, Team* player);
	void UpdateInventoryComps(Inventory* inv, Component* parentC);

	//Mouse collision when item is grabbed
	int GetHitInventorySlot(std::string who, Vec2 mP);
};

