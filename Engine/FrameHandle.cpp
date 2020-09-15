#pragma once
#include "FrameHandle.h"
//		### Componente ###
Text::Text(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates)
	:
	Component(pos, text, c, activInStates),
	f(f),
	size(size)
{

}
Button::Button(std::string text, RectF pos, int size, Font f, Color c, std::vector<int> activInStates)
	:
	Component(pos, text, c, activInStates),
	f(f),
	size(size)
{

}
//		### Framehandle::Frame ###

FrameHandle::Frame::Frame(RectF pos, int type, int nStates, sharedResC resC)
	:
	nStates(nStates),
	pos(pos),
	type(type),
	resC(std::move(resC)) {}

void FrameHandle::Frame::DrawComps(Graphics& gfx)
{
	for (int i = 0; i < comps.size(); i++)
	{
		if (comps[i]->activInStates[curState] == 1){
			comps[i]->Draw(pos, gfx);
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
	comps.push_back(std::make_unique<Text>(text, pos, size, f, c, activInStates));
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
	comps.push_back(std::make_unique<Button>(text, pos, size, f, c, activInStates));
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
void FrameHandle::Frame::SetStates(int nStates)
{
	this->nStates = nStates;
}
//		### Framehandle ###
FrameHandle::FrameHandle(sharedResC resC)
	:resC(std::move(resC))
{

}

bool FrameHandle::HandleMouseInput(Mouse::Event& e)
{

	for (auto& frame : frames)
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
	for (int i = frames.size() - 1; i >= 0; i--)
	{
		frames[i]->Draw(gfx);
	}
}
void FrameHandle::AddFrame(RectF posWind, int type, int nStates, sharedResC resC)
{
	frames.push_back(std::make_unique<Frame>(posWind, type, nStates, resC));
}
void FrameHandle::AddScrollWindow(RectF posWind, RectF posSB, int type, int nElem, std::string defValue)
{
	//comps.push_back(std::make_unique<ScrollWindow>(posWind + configs.pos, posSB, type, nElem, defValue));
}