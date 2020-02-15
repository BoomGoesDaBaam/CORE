#pragma once
#include "Rect.h"
#include <vector>
#include "Graphics.h"
#include <memory>
class GraphicObjects
{
public:
	class Object
	{
	protected:
		bool killMe = false;
	public:
		Object() = default;
		virtual void Update(float dt) {};
		virtual void Draw(Graphics& gfx) {};
		bool ChoosenToDie() { return killMe; }
	};
	class Particle : public Object
	{
		float x, y;
		Vec2 vel;
		Vec2 gravity;
		float radius;
	public:
		Particle(float x, float y, float radius, Vec2 vel, Vec2 gravity) :x(x), y(y), radius(radius), vel(vel), gravity(gravity) {}
		void Update(float dt) override
		{
			x += dt * vel.x; y += dt * vel.y;
			vel.x += dt * gravity.x;
			vel.y += dt * gravity.y;
			if (!(Graphics::GetScreenRect<int>().Contains(Vei2(x, y))))
			{
				killMe = true;
			}
		}
		void Draw(Graphics& gfx)override {
			gfx.DrawCircle(x, y, radius, Colors::Red);
		}
		void SendHelp()
		{
			x += 2;
		}
	};
	class TileFrame : public Object
	{
		int x, y, nRows, nColums;
	public:
		TileFrame(int x, int y, int nRows, int nColums) :x(x), y(y), nRows(nRows), nColums(nColums)
		{

		}
		void Draw(Graphics& gfx)override
		{

		}
	};
private:
	std::vector<std::unique_ptr<Object>> objects;
	Graphics& gfx;
public:
	GraphicObjects(Graphics& gfx) :gfx(gfx) {}
	void Update(float dt);
	void Draw();
	void Add(Particle* object) 
	{
		objects.push_back(std::make_unique<Particle>(*object));
	}
	typedef GraphicObjects GrapObj;
};
