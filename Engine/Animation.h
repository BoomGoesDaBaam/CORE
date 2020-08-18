#pragma once
#include <vector>
#include "Surface.h"
#include "Matrix.h"
class Animation
{
protected:
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
	std::vector<Surface>& GetFrames()
	{
		return frames;
	}
};

class SubAnimation
{
public:
	Animation& a;
	RectI sourceR;
	SubAnimation(Animation& a, RectI sourceR)
		:
		a(a),
		sourceR(sourceR)
	{

	}
	SubAnimation() = delete;
	Matrix<int> GetPosOfChroma(Color chroma = Colors::Magenta)		// 1 = every animation surface has none chroma at (x,y)			0 = at least one animation surface has chroma at (x,y)
	{
		assert(sourceR.IsContainedBy(a.GetFrames()[0].GetRect()));

		Matrix<int> mat = Matrix<int>(sourceR.GetWidth(), sourceR.GetHeight(), 1);
		int size = a.GetFrames().size();
		auto& frames = a.GetFrames();

		for (int y = 0; y < sourceR.GetHeight(); y++)
		{
			for (int x = 0; x < sourceR.GetWidth(); x++)
			{
				for (int i = 0; i < size; i++)
				{
					if (frames[i].GetPixel(x + sourceR.left, y + sourceR.top) == chroma)
					{
						mat[x][y] = 0;
						break;
					}
				}
			}
		}
		return mat;
	}
};
