#pragma once
#include "FrameHandle.h"
//		### Componente ###
Text::Text(std::string text, RectF pos, int size, Font* f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc, std::queue<FrameEvent>* buffer)
	:
	Component(pos, parentC, buffer),
	f(f),
	size(size),
	textLoc(textLoc)
{
	this->activInStates = activInStates;
	this->text = text;
	this->c = c;
}
Button::Button(RectF pos, Animation* a, Animation* aHover, std::vector<int> activInStates, Component* parentC, std::queue<FrameEvent>* buffer)
	:
	Component(pos, parentC, buffer)
{
	this->a = a;
	this->aHover = aHover;
	hitable = true;
	//if (Frame* p = dynamic_cast<Frame*>(parentC))
	//{
		//pF = p;
	//}
	//else 
	if (PageFrame* p = dynamic_cast<PageFrame*>(parentC))
	{
		ppF = p;
	}
	this->activInStates = activInStates;
}
/*
Composition::Composition(RectF pos, sharedResC resC, std::vector<int> activInStates, Component* parentC, std::queue<FrameEvent>* buffer, int type)
	:
	Frame(pos, 0, resC, parentC, buffer)
{
	/*
	if (type == 0)
	{
		a = &this->resC->tC.windowsFrame[7];
		aHover = &this->resC->tC.windowsFrame[7];
		this->activInStates = activInStates;
	}
}
*/
//		### Framehandle::Frame ###

Frame::Frame(RectF pos, int type, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer)
	:
	Component(pos, parentC,buffer),
	resC(std::move(resC))
{
	this->type = type;
	this->nStates = 1;
	assert(type == 0 || type == 1 || type == -1);
	if (type == 0)
	{
		nStates = 2;
		activInStates = { 1, 0 };
	}

}

PageFrame::PageFrame(RectF pos, int type, sharedResC resC, Component* parentC, int nPages, std::queue<FrameEvent>* buffer)
	:
	Frame(pos, type, resC, parentC,buffer),
	nPages(nPages)
{
	std::vector<int> a = {0, 1};
	Button* b1 = AddButton(RectF(Vec2(5, 18), 34, 9), &resC->tC.buttons[0], &resC->tC.buttons[1], "b_left", a);
	b1->bFunc = B1;
	Button* b2 = AddButton(RectF(Vec2(99, 18), 34, 9), &resC->tC.buttons[2], &resC->tC.buttons[3], "b_right", a);
	b2->bFunc = B2;
	if(type != 0)
	{
		curState = 1;
	}
}

void Frame::SetText(std::string text, std::string key)
{
	std::map<std::string, std::unique_ptr<Component>>::iterator it = comps.find(key);
	if (it != comps.end())
	{
		it->second->text = text;
	}
}
bool Frame::Hit(Vec2 mP)
{
	if (IsVisible())
	{
		assert(type == 0 || type == 1 || type == -1);
		Vei2 mpRel = (Vei2)(mP - GetPos().GetTopLeft<float>());
		switch (type)
		{
		case 0:
			switch (curState)
			{
			case 0:
				return resC->tC.windowsFrame[1].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel);
				break;
			case 1:
				return resC->tC.windowsFrame[0].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel) || resC->tC.windowsFrame[2].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel);
				break;
			}
			break;
		case 1:
			return s->TestIfHitOnScreen((Vec2)mpRel);
			break;
		case -1:
			return resC->tC.windowsFrame[1].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel);
			break;
		}
		return false;
	}
}
bool Frame::IsExtended() 
{ 
	assert(type == 0  || type == 1 || type == -1);
	switch (type)
	{
	case 0:
		return curState == 1;
		break;
	}
	return false;
}
int Frame::GetExtendedHeight()
{
	assert(type == 0 || type == 1 || type == -1);
	switch (type)
	{
	case 0:
		return ((int)pos.GetHeight() / 20) * 19;
		break;
	}
	return 0;
}
void Frame::Grab(Vec2 mP)
{
	Vec2 delta = mP - pos.GetTopLeft<float>();
	if (delta.y < pos.GetHeight() * FrameHandle::percentForGrab && delta.y >= 0 && delta.x >= 0 && delta.x < pos.GetWidth())
	{
		grabbed = true;
		lastMouseP = mP;
	}
}
bool B1(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "priv page", -1, caller));
	//pF->PriviousPage();
	return true;
}
bool B2(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "next page", -1, caller));
	//pF->NextPage();
	return true;
}
bool BBuildMode0(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", 0, caller));
	//curW.SetBuildMode(0);
	return true;
}
bool BBuildMode2(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", 2, caller));
	//curW.SetBuildMode(2);
	return true;
}
bool BBuildMode3(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", 3, caller));
	//curW.SetBuildMode(3);
	return true;
}
bool BBuildMode21(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", 21, caller));
	//curW.SetBuildMode(3);
	return true;
}
bool BBuildMode22(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", 22, caller));
	//curW.SetBuildMode(3);
	return true;
}
bool BBuildMode23(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", 23, caller));
	//curW.SetBuildMode(3);
	return true;
}
bool BBuildMode24(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", 24, caller));
	//curW.SetBuildMode(3);
	return true;
}
bool BBuildMode25(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", 25, caller));
	//curW.SetBuildMode(3);
	return true;
}
bool BBuildMode26(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", 26, caller));
	//curW.SetBuildMode(3);
	return true;
}
bool BNextTurn(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "next turn", -1, caller));
	//curW.NextTurn();
	return true;
}
bool BBuildMenu(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "load buildmenu", -1, caller));
	//curW.LoadBuildMenu();
	return true;
}
bool BOpenGamefield(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "exit currentView", -1, caller));
	//curW.LoadBuildMenu();
	return true;
}
void Frame::Move(Vec2 mP)
{
	Vec2 deltaMove = lastMouseP - mP;
	lastMouseP = mP;
	pos -= deltaMove;
}
void Frame::Release()
{
	grabbed = false;
}
std::vector<int> Frame::FillWith1WhenSize0(std::vector<int> activInStates, int nStages)
{
	if (activInStates.size() == 0)
	{
		std::vector<int> activInStates(nStages, 1);
		return std::vector<int>(nStages, 1);
	}
	return activInStates;
}
//  ### Framehandle::PageFrame ###


//	### Framehandle::MultiFrame ###
Frame* MultiFrame::AddFrame(RectF pos, int type, sharedResC resC, Component* parentC)
{
	frames.push_back(std::make_unique<Frame>(pos,type,resC,parentC, buffer));
	return frames[frames.size() - 1].get();
}
PageFrame* MultiFrame::AddPageFrame(RectF pos, int type, sharedResC resC, Component* parentC, int nPages)
{
	frames.push_back(std::make_unique<PageFrame>(pos, type, resC, parentC, nPages, buffer));
	return static_cast<PageFrame*>(frames[frames.size() - 1].get());
}

FrameEvent FrameHandle::Read()
{
	if (buffer.size() > 0u)
	{
		FrameEvent e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return FrameEvent();
	}
}

//	### Framehandle ###
FrameHandle::FrameHandle(sharedResC resC)
	:resC(std::move(resC))
{
	if (Settings::framesOn)
	{
		InitFrames();
	}
}

bool FrameHandle::HandleMouseInput(Mouse::Event& e)
{
	if (Settings::framesOn)
	{
		for (auto& frame : windows)
		{
			if (frame->HandleMouseInput(e, true))
			{
				return true;
			}
		}
	}
	return false;
}

void FrameHandle::Draw(Graphics& gfx)
{
	if (Settings::framesOn)
	{
		for (int i = windows.size() - 1; i >= 0; i--)
		{
			windows[i]->Draw(gfx);
		}
	}
}
Frame* FrameHandle::AddFrame(RectF pos, int type)
{
	if (Settings::framesOn)
	{
		windows.push_back(std::make_unique<Frame>(pos, type, resC, nullptr,&buffer));
		return static_cast<Frame*>(windows[windows.size() - 1].get());
	}
}
PageFrame* FrameHandle::AddPageFrame(RectF pos, int type, int nPages)
{
	if (Settings::framesOn)
	{
		windows.push_back(std::make_unique<PageFrame>(pos, type, resC, nullptr, nPages, &buffer));
		return static_cast<PageFrame*>(windows[windows.size() - 1].get());
	}
}
MultiFrame* FrameHandle::AddMultiFrame(RectF pos, int type, int nStates)
{
	if (Settings::framesOn)
	{
		std::vector<int> activInStates;
		for (int i = 0; i < nStates; i++)
		{
			activInStates.push_back(1);
		}
		windows.push_back(std::make_unique<MultiFrame>(pos, resC, nullptr, &buffer));
		return static_cast<MultiFrame*>(windows[windows.size() - 1].get());
	}
}



