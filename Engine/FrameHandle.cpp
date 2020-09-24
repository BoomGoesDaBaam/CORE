#pragma once
#include "FrameHandle.h"
//		### Componente ###
Text::Text(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc)
	:
	Component(pos, parentC),
	f(f),
	size(size),
	textLoc(textLoc)
{
	this->activInStates = activInStates;
	this->text = text;
	this->c = c;
}
Button::Button(RectF pos, Animation& a, Animation& aHover, std::vector<int> activInStates, Component* parentC)
	:
	Component(pos, parentC),
	a(a), 
	aHover(aHover)
{
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
//		### Framehandle::Frame ###

Frame::Frame(RectF pos, int type, sharedResC resC, Component* parentC)
	:
	Component(pos, parentC),
	nStates(1),
	type(type),
	resC(std::move(resC))
{
	assert(type == 0 || type == -1);
	if (type == 0)
	{
		nStates = 2;
		activInStates = { 1, 0 };
	}
}

PageFrame::PageFrame(RectF pos, int type, sharedResC resC, Component* parentC, int nPages)
	:
	Frame(pos, type, resC, parentC),
	nPages(nPages)
{
	std::vector<int> a = {0, 1};
	Button* b1 = AddButton(RectF(Vec2(5, 18), 34, 9), resC->tC.buttons[0], resC->tC.buttons[1], a);
	b1->bFunc = B1;
	Button* b2 = AddButton(RectF(Vec2(99, 18), 34, 9), resC->tC.buttons[2], resC->tC.buttons[3], a);
	b2->bFunc = B2;
}

void Frame::SetText(std::string text, int index)
{
	assert(index >= 0 && index < comps.size());
	comps[index]->text = text;
}
bool Frame::Hit(Vec2 mP)
{
	if (visible)
	{
		assert(type == 0 || type == -1);
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
		case -1:
			return resC->tC.windowsFrame[1].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel);
			break;
		}
		return false;
	}
}
bool Frame::IsExtended() 
{ 
	assert(type == 0 || type == -1);
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
	assert(type == 0 || type == -1);
	switch (type)
	{
	case 0:
		return (pos.GetHeight() / 20) * 19;
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
bool B1(PageFrame* pF)
{
	pF->PriviousPage();
	return true;
}
bool B2(PageFrame* pF)
{
	pF->NextPage();
	return true;
}
/*
bool B3(PageFrame* pF, World& curW)
{
	//curW.SetBuildMode(0);
	return true;
}
*/
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
	frames.push_back(std::make_unique<Frame>(pos,type,resC,parentC));
	return frames[frames.size() - 1].get();
}
PageFrame* MultiFrame::AddPageFrame(RectF pos, int type, sharedResC resC, Component* parentC, int nPages)
{
	frames.push_back(std::make_unique<PageFrame>(pos, type, resC, parentC, nPages));
	return static_cast<PageFrame*>(frames[frames.size() - 1].get());
}

//	### Framehandle ###
FrameHandle::FrameHandle(sharedResC resC)
	:resC(std::move(resC))
{
	InitFrames();
}

bool FrameHandle::HandleMouseInput(Mouse::Event& e)
{
	for (auto& frame : windows)
	{
		if (frame->HandleMouseInput(e, true))
		{
			return true;
		}
	}
	return false;
}

void FrameHandle::Draw(Graphics& gfx)
{
	for (int i = windows.size() - 1; i >= 0; i--)
	{
		windows[i]->Draw(gfx);
	}
}
void FrameHandle::AddFrame(RectF pos, int type, sharedResC resC)
{
	windows.push_back(std::make_unique<Frame>(pos, type, resC, nullptr));
}
MultiFrame* FrameHandle::AddMultiFrame(RectF pos, int type, int nStates, sharedResC resC)
{
	std::vector<int> activInStates;
	for (int i = 0; i < nStates; i++)
	{
		activInStates.push_back(1);
	}
	windows.push_back(std::make_unique<MultiFrame>(pos, resC, nullptr));
	return static_cast<MultiFrame*>(windows[windows.size() - 1].get());
}



