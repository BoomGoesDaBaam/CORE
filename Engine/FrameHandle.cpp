#pragma once
#include "FrameHandle.h"
//		### Componente ###
Text::Text(std::string text, RectF pos, int size, Font f, Color c)
	:
	Component(pos, c),
	text(text),
	f(f),
	size(size)
{

}
//		### Framehandle::Frame ###

FrameHandle::Frame::Frame(RectF pos, int type, sharedResC resC)
	:
	pos(pos),
	type(type),
	resC(std::move(resC)) {}

void FrameHandle::Frame::DrawComps(Graphics& gfx)
{
	for (int i = 0; i < comps.size(); i++)
	{
		comps[i]->Draw(pos, gfx);
	}
}
void FrameHandle::Frame::AddText(std::string text, RectF pos, int size, Font f, Color c)
{
	comps.push_back(std::make_unique<Text>(text, pos, size, f, c));
}
void FrameHandle::Frame::AddButton(std::string text, RectF pos, int size, Font f, Color c)
{
	comps.push_back(std::make_unique<Text>(text, pos, size, f, c));
}
void FrameHandle::Frame::Grab(Vec2 mP)
{
	Vec2 delta = mP - pos.GetTopLeft<float>();
	if (delta.y < pos.GetHeight() * percentForGrab && delta.y >= 0)
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

//		### Framehandle::TileFrame ###
FrameHandle::TileFrame::TileFrame(Vec2 pos, Matrix<int> matrix, int type, int tileSize, sharedResC resC)
	:
	Frame(RectF(Vec2(pos.x - tileSize, pos.y - tileSize), tileSize* (matrix.GetColums() + 2), tileSize* (matrix.GetRows() + 2)), type, resC),
	matrix(matrix),
	outline(matrix.GetMatPlusZeroOutline()),
	cellSize(tileSize),
	offset(resC->fsC.GetConOffset(Vei2(tileSize, tileSize)))
{

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

void FrameHandle::AddFrame(RectF posWind, int type, sharedResC resC)
{
	frames.push_back(std::make_unique<Frame>(posWind, type, resC));
}
void FrameHandle::AddTileFrame(Vec2 posWind, Matrix<int> matrix, int type, int tileSize, sharedResC resC)
{
	frames.push_back(std::make_unique<TileFrame>(posWind, matrix, type, tileSize, resC));

}
void FrameHandle::AddScrollWindow(RectF posWind, RectF posSB, int type, int nElem, std::string defValue)
{
	//comps.push_back(std::make_unique<ScrollWindow>(posWind + configs.pos, posSB, type, nElem, defValue));
}
void FrameHandle::AddText(RectF posWind, std::string text, int size, Font& font, Color c)
{
	//comps.push_back(std::make_unique<Textfield>(posWind + configs.pos, text, size, font, c));
}
void FrameHandle::AddButton(RectF posWind, Color c)
{
	//comps.push_back(std::make_unique<Button>(posWind + configs.pos, c));
}