#pragma once
#include "Rect.h"
#include <vector>
#include "Graphics.h"
#include <memory>
#include "RandyRandom.h"
class GraphicObjects
{
	std::vector<Surface> tileFramePics;
public:
	class Object
	{
	protected:
		bool killMe = false;
		Vec2 pos = Vec2(0.0f, 0.0f);
		Vec2 vel = Vec2(0.0f, 0.0f);
		Vec2 gravity = Vec2(0.0f, 0.0f);
		Color primC = Colors::Yellow, secC = Colors::Red;
	public:
		Object() = default;
		Object(Vec2 pos, Vec2 vel, Vec2 gravity);
		virtual void Update(float dt);
		virtual void Draw(Graphics& gfx) {};
		bool ChoosenToDie() { return killMe; }
	};

	class Particle : public Object
	{
		float radius;
	public:
		Particle(Vec2 pos, float radius, Vec2 vel, Vec2 gravity) : Object(pos, vel, gravity), radius(radius) {}
		void Draw(Graphics& gfx)override {
			gfx.DrawCircle((int)pos.x, (int)pos.y, radius, radius - (radius / 3), primC, secC);
		}
		void RealNice()
		{
			radius += 0.1f;
		}
	};
	
	class TileFrame : public Object
	{
		int  nRows, nColums;
	public:
		TileFrame(int x, int y, int nRows, int nColums, int style = 0): Object(Vec2(x, y), Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f)), nRows(nRows), nColums(nColums) {}
		void Draw(Graphics& gfx)override
		{

		}
	};
	
	class Shot : public Object
	{
		float width, length;
	public:
		Shot(Vec2 pos, Vec2 vel, Vec2 gravity, float length, float width) : Object(pos, vel, gravity), length(length), width(width) {}
		void Draw(Graphics& gfx)override {
			gfx.DrawLine(pos, pos + vel * length * 0.05f, primC, (int)width);
		}
	};
private:
	std::vector<std::unique_ptr<Object>> objects;
	Graphics& gfx;
	RandyRandom rr;
public:
	GraphicObjects(Graphics& gfx);
	void Update(float dt);
	void Draw();
	void Add(Object* object)
	{
		if (Particle* v = dynamic_cast<Particle*>(object))
		{
			objects.push_back(std::make_unique<Particle>(*v));
		}
		if (Shot* v = dynamic_cast<Shot*>(object))
		{
			objects.push_back(std::make_unique<Shot>(*v));
		}
	}

	//Particle Blueprints			
	void AddVolvano(Vec2 p0, int size, int spreadSpeed);
	void AddSpark(Vec2 p0, int size, int spreadSpeed);
	void AddShot(Vec2 p0, Vec2 pGoal, float speed);

	typedef GraphicObjects GRAPH_OBJ;
	typedef GraphicObjects::Particle PARTICLE;
};
