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
	Vei2 GetSize()
	{
		return Vei2(frames[0].GetWidth(), frames[0].GetHeight());
	}
	int GetWidth()
	{
		return frames[0].GetWidth();
	}
	int GetHeight()
	{
		return frames[0].GetHeight();
	}
};

class SubAnimation
{
public:
	Animation& a;
	RectI sourceR, posIn50x50grit;
	Matrix<int> chromaM;
	SubAnimation(Animation& a, RectI sourceR, RectI posIn50x50grit)
		:
		a(a),
		sourceR(sourceR),
		posIn50x50grit(posIn50x50grit)
	{
		chromaM = GetPosOfChroma();
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

	static Matrix<int> PutOnTopOfEachOther(std::vector<SubAnimation> oldM, Vei2 dim, int lookFor, int other)		//keeps lookFor 
	{
		Matrix<int> newM = Matrix<int>(dim.x, dim.y, 0);
		RectI size = RectI(Vei2(0, 0), dim.x, dim.y);


		for (int y = 0; y < dim.y; y++)
		{
			for (int x = 0; x < dim.x; x++)
			{
				int value = other;
				for (int i = 0; i < oldM.size(); i++)
				{
					int solid = 0;

					if (oldM[i].posIn50x50grit.Contains(Vei2(x, y)) && oldM[i].chromaM[(__int64)x - oldM[i].posIn50x50grit.left][(__int64)y - oldM[i].posIn50x50grit.top] == lookFor)
					{
						value = lookFor;
					}

					if (solid == 4)
					{
						value = lookFor;
					}
				}
				newM[x][y] = value;
			}
		}
		return newM;
	}
	static Matrix<int> HalfSize(Matrix<int> oldM, int prio)
	{
		Matrix<int> newM = Matrix<int>(oldM.GetColums()/2, oldM.GetRaws()/2, 0);
		for (int y = 0; y < newM.GetRaws(); y++)
		{
			for (int x = 0; x < newM.GetColums(); x++)
			{
				bool hasPrio = false;
				for (int yInner = 0; yInner < 2; yInner++)
				{
					for (int xInner = 0; xInner < 2; xInner++)
					{
						if (oldM[(__int64)x * 2 + xInner][(__int64)y * 2 + yInner] == prio)
						{
							hasPrio = true;
						}
					}
				}
				if (hasPrio)
				{
					newM[x][y] = prio;
				}
				else
				{
					newM[x][y] = oldM[(__int64)x * 2][(__int64)y * 2];
				}
			}
		}
		return newM;
	}
};