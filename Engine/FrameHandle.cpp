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
TextBox::TextBox(std::string text, RectF pos, int size, Font* f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc, std::queue<FrameEvent>* buffer)
	:
	Text("text should be shown",pos,size,f,c,activInStates, parentC,0, buffer),
	lines(f->SplitTextToLines(text, size, pos.GetWidth()))
{

}
Button::Button(RectF pos, Animation* a, Animation* aHover, std::vector<int> activInStates,Font* f, Component* parentC, std::queue<FrameEvent>* buffer)
	:
	Text("",pos, 10,f,Colors::Black,activInStates, parentC,0, buffer)
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

CheckBox::CheckBox(RectF pos, Component* parentC, std::queue<FrameEvent>* buffer, sharedResC resC, std::vector<int> activInStates)
	:
	Component(pos, parentC, buffer),
	resC(std::move(resC))
{
	this->activInStates = activInStates;
	this->hitable = true;
}
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
	if (type == 1)
	{
		a = { 1 };
	}
	std::vector<int> activOnPagesL;
	activOnPagesL.push_back(0);
	for (int i = 0; i < nPages - 1; i++)
	{
		activOnPagesL.push_back(1);
	}
	Button* b1 = AddButton(RectF(Vec2(5, 18), 34, 9), &resC->tC.buttons[0], &resC->tC.buttons[1], "b_left", &resC->tC.fonts[0],a, activOnPagesL);
	b1->bFunc = B1;
	std::vector<int> activOnPagesR;
	for (int i = 0; i < nPages - 1; i++)
	{
		activOnPagesR.push_back(1);
	}
	activOnPagesR.push_back(0);
	Button* b2 = AddButton(RectF(Vec2(99, 18), 34, 9), &resC->tC.buttons[2], &resC->tC.buttons[3], "b_right", &resC->tC.fonts[0],a, activOnPagesR);
	b2->bFunc = B2;
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
			Vec2 scale = Vec2(s->GetSize()) / Vec2(pos.GetSize());
			mpRel.x = mpRel.x * scale.x;
			mpRel.y = mpRel.y * scale.y;

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
bool BBuildMode(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "enable buildmode", caller->extra1, caller));
	//curW.SetBuildMode(0);
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
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "load scene", 1, caller));
	//curW.LoadBuildMenu();
	return true;
}
bool BOpenGamefield(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "load scene", 0, caller));
	//curW.LoadBuildMenu();
	return true;
}
bool BSetAttackMode(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::ButtonPressed, "set attackMode", -1, caller));
	return true;
}
bool BSetObstacleState(std::queue<FrameEvent>* buffer, Component* caller)
{
	if(caller->extraB1)
		buffer->push(FrameEvent(FrameEvent::ButtonPressed, "set obstacle state townhall", caller->extra1, caller));
	else
		buffer->push(FrameEvent(FrameEvent::ButtonPressed, "set obstacle state townhall", caller->extra2, caller));
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
	else
	{
		for (int i = 0; i < nStates - (int)activInStates.size(); i++)
		{
			activInStates.push_back(1);
		}
	}
	return activInStates;
}
//  ### Framehandle::PageFrame ###


//	### Framehandle::MultiFrame ###
Frame* MultiFrame::AddFrame(std::string key, RectF pos, int type, sharedResC resC, Component* parentC)
{
	frames[key] = std::make_unique<Frame>(pos,type,resC,parentC, buffer);
	return frames[key].get();
}
PageFrame* MultiFrame::AddPageFrame(std::string key, RectF pos, int type, sharedResC resC, Component* parentC, int nPages)
{
	frames[key] = std::make_unique<PageFrame>(pos, type, resC, parentC, nPages, buffer);
	return static_cast<PageFrame*>(frames[key].get());
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
	
}

bool FrameHandle::HandleMouseInput(Mouse::Event& e)
{
	if (Settings::framesOn)
	{
		std::map<std::string, std::unique_ptr<Component>>::iterator i;
		for (i = comps.begin(); i != comps.end(); i++)
		{
			if (i->second->HandleMouseInput(e, true))
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
		std::map<std::string, std::unique_ptr<Component>>::iterator i;
		for (i = comps.begin(); i != comps.end(); i++)
		{
			i->second->Draw(gfx);
		}
	}
}
Frame* FrameHandle::AddFrame(std::string key, RectF pos, int type)
{
	if (Settings::framesOn)
	{
		comps[key] = std::make_unique<Frame>(pos, type, resC, nullptr,&buffer);
		return static_cast<Frame*>(comps[key].get());
	}
}
PageFrame* FrameHandle::AddPageFrame(std::string key, RectF pos, int type, int nPages)
{
	if (Settings::framesOn)
	{
		comps[key] = std::make_unique<PageFrame>(pos, type, resC, nullptr, nPages, &buffer);
		return static_cast<PageFrame*>(comps[key].get());
	}
}
MultiFrame* FrameHandle::AddMultiFrame(std::string key, RectF pos, int type, int nStates)
{
	if (Settings::framesOn)
	{
		std::vector<int> activInStates;
		for (int i = 0; i < nStates; i++)
		{
			activInStates.push_back(1);
		}
		comps[key] = std::make_unique<MultiFrame>(pos, resC, nullptr, &buffer);
		return static_cast<MultiFrame*>(comps[key].get());
	}
}



