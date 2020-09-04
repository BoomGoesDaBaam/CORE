#pragma once
#include "MainWindow.h"
class GrabHandle
{
	bool grabbed=false;
	bool hold = false;

	Vec2 holdPos = { -1,-1 };
	Vec2 lastPos = { -1,-1 };

public:
	GrabHandle() = default;
	Vec2 MoveCamera(const Mouse::Event& e);
	bool IsLocked() { return grabbed; }
	void Unlock()
	{
		grabbed = false;
		hold = false;
	}
};

