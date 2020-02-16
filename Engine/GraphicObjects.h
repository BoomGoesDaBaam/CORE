#pragma once
#include "Rect.h"
#include <vector>
#include "Graphics.h"
#include <memory>
#include "RandyRandom.h"
class GraphicObjects
{
public:
	class Object
	{
	protected:
		bool killMe = false;
		Vec2 pos=Vec2(0.0f, 0.0f);
		Vec2 vel = Vec2(0.0f, 0.0f);
		Vec2 gravity= Vec2(0.0f, 0.0f);
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
		Particle(Vec2 pos, float radius, Vec2 vel, Vec2 gravity): Object(pos, vel, gravity), radius(radius) {}
		void Draw(Graphics& gfx)override {
			gfx.DrawCircle(pos.x, pos.y, radius, radius-(radius/3), primC, secC);
		}
	};
	/*
	class TileFrame : public Object
	{
		int  nRows, nColums;
	public:
		TileFrame(int x, int y, int nRows, int nColums) :x(x), y(y), nRows(nRows), nColums(nColums)
		{

		}
		void Draw(Graphics& gfx)override
		{

		}
	};
	*/
	class Shot : public Object
	{
		float width,length;
	public:
		Shot(Vec2 pos, Vec2 vel, Vec2 gravity,float length, float width): Object(pos, vel, gravity), length(length), width(width){}
		void Draw(Graphics& gfx)override {
			gfx.DrawLine(pos, pos + vel*length* 0.05f, primC, width);
		}
	};
private:
	std::vector<std::unique_ptr<Object>> objects;
	Graphics& gfx;
	RandyRandom rr;
public:
	GraphicObjects(Graphics& gfx) :gfx(gfx) {}
	void Update(float dt);
	void Draw();
	void Add(Particle* object) 
	{
		objects.push_back(std::make_unique<Particle>(*object));
	}
	void Add(Shot* object)
	{
		objects.push_back(std::make_unique<Shot>(*object));
	}

	//Particle Blueprints
	void AddVolvano(Vec2 pos, int size, int spreadSpeed) //   size / spreadSpeed | 10 = smal 100 = HUGE    
	{
		Add(&GraphicObjects::Particle(pos,rr.Calc(size/3)+1, Vec2(rr.Calc(spreadSpeed) - (spreadSpeed / 2), rr.Calc(spreadSpeed) - (spreadSpeed / 2)), Vec2(0, 3)));
	}
	void AddShot(Vec2 pos, Vec2 vel, Vec2 gravity) //   size / spreadSpeed | 10 = smal 100 = HUGE    
	{
		Add(&GraphicObjects::Shot(pos, vel, gravity, 20, 3));
	}

	typedef GraphicObjects GRAPH_OBJ;
	typedef GraphicObjects::Particle PARTICLE;
};
