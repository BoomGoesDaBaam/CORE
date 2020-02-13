#pragma once
#include <random>
class RandyRandom
{
public:
	int Calc(int bounce)
	{
		std::uniform_int_distribution<int> b = std::uniform_int_distribution<int>(0, bounce);
		return b(rng);
	}
	float GetFaktor() //Returns number between 0.0f and 1.0f
	{
		std::uniform_int_distribution<int> b = std::uniform_int_distribution<int>(0, 100);
		return (float)(b(rng))/100.0f;
	}
	float GetNormalDist()//Returns number between 0.0f and 1.0f.  0f is more favoured to appear
	{
		std::normal_distribution<double> distribution(0.0, 0.5);
		float shot = distribution(generator);
		while (shot<0.0f)
		{
			shot = distribution(generator);
		}
		return shot;
	}
private:
	//RNG
	std::mt19937 rng = std::mt19937(std::random_device{}());
	//NormalDist
	std::default_random_engine generator;
};