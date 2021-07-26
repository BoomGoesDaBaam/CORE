#pragma once
#include "MainWindow.h"
class GrabHandle
{
	bool grabbed=false;
	bool hold = false;

	Vec2 holdPos = { -1,-1 };
	Vec2 lastPos = { -1,-1 };

	float dist2Grab;
public:
	GrabHandle(float dist2Grab = 20.f)
		:
		dist2Grab(dist2Grab)
	{}
	Vec2 MoveCamera(const Mouse::Event& e);
	bool IsLocked() { return grabbed; }
	void Release()
	{
		grabbed = false;
		hold = false;
		lastPos = { -1,-1 };
		holdPos = { -1,-1 };
	}
};

