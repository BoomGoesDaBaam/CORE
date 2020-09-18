#pragma once
#include "FrameHandle.h"
//		### Componente ###
Text::Text(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates, RectF& parentPos)
	:
	Component(parentPos,pos),
	f(f),
	size(size)
{
	this->activInStates = activInStates;
	this->text = text;
	this->c = c;
}
Button::Button(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates, RectF& parentPos)
	:
	Component(parentPos,pos),
	f(f),
	size(size)
{
	this->activInStates = activInStates;
	this->text = text;
	this->c = c;
}
//		### Framehandle::Frame ###

FrameHandle::Frame::Frame(RectF pos, int type, RectF& parentPos, sharedResC resC)
	:
	Component(parentPos,pos),
	nStates(nStates),
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


void FrameHandle::Frame::SetText(std::string text, int index)
{
	assert(index >= 0 && index < comps.size());
	comps[index]->text = text;
}
bool FrameHandle::Frame::Hit(Vec2 mP)
{
	assert(type == 0 || type == -1);
	Vei2 mpRel = (Vei2)(mP - pos.GetTopLeft<float>() - parentPos.GetTopLeft<float>());
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
bool FrameHandle::Frame::IsExtended() 
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
int FrameHandle::Frame::GetExtendedHeight()
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
void FrameHandle::Frame::DrawComps(Graphics& gfx)
{
	for (int i = 0; i < comps.size(); i++)
	{
		if (comps[i]->activInStates[curState] == 1){
			comps[i]->Draw(gfx);
		}
	}
}
void FrameHandle::Frame::AddText(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates)
{
	if (activInStates.size() == 0)
	{
		for (int i = 0; i < nStates; i++)
		{
			activInStates.push_back(1);
		}
	}
	comps.push_back(std::make_unique<Text>(text, pos, size, f, c, activInStates,this->pos));
}
void FrameHandle::Frame::AddButton(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates)
{
	if (activInStates.size() == 0)
	{
		for (int i = 0; i < nStates; i++)
		{
			activInStates.push_back(1);
		}
	}
	comps.push_back(std::make_unique<Button>(text, pos, size, f, c, activInStates, this->pos));
}
void FrameHandle::Frame::AddSubFrame(RectF pos, int type, sharedResC resC)
{
	comps.push_back(std::make_unique<Frame>(pos, type, this->pos, resC));
}
void FrameHandle::Frame::Grab(Vec2 mP)
{
	Vec2 delta = mP - pos.GetTopLeft<float>();
	if (delta.y < pos.GetHeight() * percentForGrab && delta.y >= 0 && delta.x >= 0 && delta.x < pos.GetWidth())
	{
		grabbed = true;
		lastMouseP = mP;
	}
}
void FrameHandle::Frame::Move(Vec2 mP)
{
	Vec2 deltaMove = lastMouseP - mP;
	lastMouseP = mP;
	pos -= deltaMove;
}
void FrameHandle::Frame::Release()
{
	grabbed = false;
}
//	### Framehandle::MultiFrame ###
FrameHandle::Frame* FrameHandle::MultiFrame::AddFrame(Frame frame)
{
	frames.push_back(std::make_unique<Frame>(frame));
	return frames[frames.size() - 1].get();
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
		if (frame->HandleMouseInput(e))
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
void FrameHandle::AddFrame(RectF pos, int type, RectF& parent, sharedResC resC)
{
	windows.push_back(std::make_unique<Frame>(pos, type, parent, resC));
}
FrameHandle::MultiFrame* FrameHandle::AddMultiFrame(RectF pos, int type, int nStates, RectF& parentPos, sharedResC resC)
{
	std::vector<int> activInStates;
	for (int i = 0; i < nStates; i++)
	{
		activInStates.push_back(1);
	}
	windows.push_back(std::make_unique<MultiFrame>(parentPos, pos, resC));
	return static_cast<MultiFrame*>(windows[windows.size() - 1].get());
}