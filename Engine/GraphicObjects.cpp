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
