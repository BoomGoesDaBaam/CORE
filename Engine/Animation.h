#pragma once
#include <vector>
#include "Surface.h"
class Animation
{
	std::vector<Surface> frames;
	int curF = 0;
	float keepTime = 1.0f, timePassed = 0.0f;
public:
	Animation() = default;
	Animation(float keepTime = 1.0f): keepTime(keepTime) {}
	Animation(std::vector<Surface> frames, float keepTime = 1.0f) :frames(frames),keepTime(keepTime)
	{}
	void Push(Surface newSurface)
	{
		frames.push_back(newSurface);
	}
	void Update(float dt)
	{
		timePassed += dt;
		if (timePassed > keepTime)
		{
			timePassed -= timePassed;
			curF++;
		}
		if (curF >= frames.size())
		{
			curF = 0;
		}
	}
	const Surface& GetCurSurface() const
	{
		return frames.at(curF);
	}
	void SetKeepTime(float keepTime)
	{
		this->keepTime = keepTime;
	}
};

