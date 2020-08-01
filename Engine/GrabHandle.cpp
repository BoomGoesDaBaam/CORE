#include "GrabHandle.h"
Vec2 GrabHandle::MoveCamera(const Mouse::Event& e)
{
	const Vec2 mPos = (Vec2)e.GetPos();

	if (!e.IsValid())
	{
		grabbed = false; hold = false;
	}
	if (e.GetType() == Mouse::Event::Type::LPress)
	{
		//GRAB HANDLE
		if (!hold)
		{
			holdPos = mPos;
			hold = true;
		}
		//#####
	}
	if (e.GetType() == Mouse::Event::Type::Move)
	{
		//GRAB HANDLE
		if (hold && (holdPos - mPos).GetLength() > 50.0f && !grabbed)
		{
			float dist = (holdPos - mPos).GetLength();
			grabbed = true;
			lastPos = mPos;
		}
		if (grabbed)
		{
			Vec2 delta = mPos - lastPos;
			lastPos = mPos;
			return delta;
		}
		//#####
	}
	if (e.GetType() == Mouse::Event::Type::LRelease)
	{
		//GRAB HANDLE
		if (grabbed||hold) { grabbed = false; hold = false; }
		//#####
	}
	return Vec2(0, 0);
}