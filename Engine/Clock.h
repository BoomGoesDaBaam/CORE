#pragma once
class Clock
{
	float passedTime = 0.f;
	float duration;
public:
	Clock(float duration) :duration(duration) {}
	bool Check(float dt)
	{
		passedTime += dt;
		if (passedTime > duration)
		{
			passedTime = 0.f;
			return true;
		}
		return false;
	}
	void reset()
	{
		passedTime = 0.0f;
	}
};

