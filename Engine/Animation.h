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
	Animation(Animation& a) : frames(a.frames), keepTime(a.keepTime) 
	{
	
	}
	void Push(Surface newFrame)
	{
		frames.push_back(newFrame);
	}
	void Update(float dt)
	{
		timePassed += dt;
		if (timePassed > keepTime)
		{
			timePassed -= timePassed;
			curF++;
		}
		if (curF >= (int)frames.size())
		{
			curF = 0;
		}
	}
	const Surface& GetCurSurface() const
	{
		return frames.at(curF);
	}
	const Surface& GetSurfaceAt(int x) const
	{
		assert(x >= 0 && x < (int)frames.size());
		return frames.at(x);
	}
	void SetKeepTime(float keepTime)
	{
		this->keepTime = keepTime;
	}
	void SetTimePassed(float timePassed)
	{
		this->timePassed = timePassed;
	}
	float GetKeepTime()
	{
		return keepTime;
	}
	int GetNumberOfFrames()
	{
		return frames.size();
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
	Color GetPixel(int frame, int x, int y)
	{
		return frames[frame].GetPixel(x, y);
	}
	void PutPixel(int frame, int x, int y, Color c)
	{
		frames[frame].PutPixel(x, y,c);
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
		posIn50x50grit(posIn50x50grit),
		chromaM(GetPosOfChroma())
	{
	}
	SubAnimation() = delete;
	Matrix<int> GetPosOfChroma(Color chroma = Colors::Magenta)		// 1 = every animation surface has none chroma at (x,y)			0 = at least one animation surface has chroma at (x,y)
	{
		assert(sourceR.IsContainedBy(a.GetFrames()[0].GetRect()));


		Matrix<int> mat = Matrix<int>(sourceR.GetWidth(), sourceR.GetHeight(), 1);
		int size = (int)a.GetFrames().size();
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

	static Matrix<int> PutOnTopOfEachOther(std::vector<SubAnimation> oldM, Vei2 dim, int keep, int other)		//'keep' value will be layered and 'other' is the rest
	{
		Matrix<int> newM = Matrix<int>(dim.x, dim.y, 0);
		RectI size = RectI(Vei2(0, 0), dim.x, dim.y);


		for (int y = 0; y < dim.y; y++)
		{
			for (int x = 0; x < dim.x; x++)
			{
				int value = other;
				for (int i = 0; i < (int)oldM.size(); i++)
				{
					int solid = 0;

					if (oldM[i].posIn50x50grit.Contains(Vei2(x, y)) && oldM[i].chromaM[(__int64)x - oldM[i].posIn50x50grit.left][(__int64)y - oldM[i].posIn50x50grit.top] == keep)
					{
						value = keep;
					}

					if (solid == 4)
					{
						value = keep;
					}
				}
				newM[x][y] = value;
			}
		}
		return newM;
	}
};