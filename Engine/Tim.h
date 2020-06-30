#pragma once
#include <chrono>
class Tim
{
	std::chrono::steady_clock::time_point last;
public:
	Tim()
	{
		last = std::chrono::steady_clock::now();
	}
	bool CheckIfTimePassed(float reqDur)
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> dur = now - last;
		if(dur.count() >= reqDur)
		{
			last = now;
			return true;
		}
		return false;
	}
	double GetPassedTime()
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> dur = now - last;
		return dur.count();
	}
};
