#pragma once
#include "GraphicObjects.h"

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
GraphicObjects::Object::Object(Vec2 pos, Vec2 vel, Vec2 gravity): pos(pos), vel(vel), gravity(gravity)
{

}
//GraphicObjects::Object(float x, float y, float radius, Vec2 vel, Vec2 gravity) :x(x), y(y), radius(radius), vel(vel), gravity(gravity) {}
