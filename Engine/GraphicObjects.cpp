#pragma once
#include "GraphicObjects.h"

GraphicObjects::GraphicObjects(Graphics& gfx):gfx(gfx)
{
}

void GraphicObjects::Update(float dt)
{

	for (auto& obj : objects)
	{
		obj->Update(dt);
	}
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->ChoosenToDie())
		{
			objects.erase(objects.begin() + i);
			i--;
		}
	}
}
void GraphicObjects::Draw()
{
	for (auto& object : objects)
	{
		object->Draw(gfx);
	}
}

//######################## OBJECTS
//OBJECTS
void GraphicObjects::Object::Update(float dt)
{
	pos += vel*dt;
	vel += gravity*dt;
	if (!(Graphics::GetScreenRect<int>().Contains((Vei2)pos)))
	{
		killMe = true;
	}
}
GraphicObjects::Object::Object(Vec2 pos, Vec2 vel, Vec2 gravity): pos(pos), vel(vel), gravity(gravity){}
//GraphicObjects::Object(float x, float y, float radius, Vec2 vel, Vec2 gravity) :x(x), y(y), radius(radius), vel(vel), gravity(gravity) {}
//OBJECTS



//######################## BLUEPRINTS
void GraphicObjects::AddVolcano(Vec2 p0, int size, int spreadSpeed)
{
	Add(&GraphicObjects::Particle(p0, (float)rr.Calc(size / 3) + 1, Vec2((float)rr.Calc(spreadSpeed) - (spreadSpeed / 2), (float)rr.Calc(spreadSpeed) - (spreadSpeed / 2)), Vec2(0, 3)));
}
void GraphicObjects::AddSpark(Vec2 p0, int size, int spreadSpeed)
{
	Add(&GraphicObjects::Particle(p0, (float)rr.Calc(size / 3) + 1, Vec2((float)rr.Calc(spreadSpeed) - (spreadSpeed / 2), (float)rr.Calc(spreadSpeed) - (spreadSpeed / 2)), Vec2(0, 0)));
}
void GraphicObjects::AddShot(Vec2 p0, Vec2 pGoal, float speed)
{
	int where = p0.x < pGoal.x ? 1 : -1;

	float m;
	if (p0.x != pGoal.x)
	{
		m = where * ((pGoal.y - p0.y) / (pGoal.x - p0.x));
	}
	else
	{
		m = where * speed;
		where = 0;
	}
	//float m = where*((pGoal.y - p0.y)/(pGoal.x - p0.x));
	Add(&GraphicObjects::Shot(p0, Vec2((float)where, m).GetNormalized() * speed, Vec2(0.0f, 0.0f), 20, 3));
}
/*
void GraphicObjects::AddTileframe(Vec2 pos, Matrix<bool> size, int style)
{
	Add(&GraphicObjects::TileFrame(pos, size, pic, style));
}
*/