#pragma once
#include "GraphicObjects.h"

GraphicObjects::GraphicObjects(Graphics& gfx, std::shared_ptr<ResourceCollection> resC):gfx(gfx), resC(std::move(resC))
{
	tC = &resC->tC;
	fsC = &resC->fsC;
}

void GraphicObjects::Update(float dt)
{

	for (auto& obj : objects)
	{
		obj->Update(dt);
	}
	for (int i = 0; i < (int)objects.size(); i++)
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
	configs.pos += configs.vel*dt;
	configs.vel += configs.gravity*dt;
	for (int i = 0; i < (int)configs.body.size(); i++)
	{
		configs.body[i] = (Vec2)GigaMath::RotPointToOrigin(configs.body[i].x, configs.body[i].y,(double)dt * configs.angleVel / 15);
	}
	if (!(Graphics::GetScreenRect<int>().Contains((Vei2)configs.pos)))
	{
		configs.killMe = true;
	}
}
GraphicObjects::Object::Object(PartConf& configs) : configs(configs){}

void GraphicObjects::AddVoc(Object* obj,const PartConf* configsTemplate, int size, int spreadSpeed)
{
	PartConf configs= PartConf(*configsTemplate);
	configs.size = (float)rr.Calc(size) + 1;
	configs.ScaleBody((float)rr.Calc(size) + 1);
	configs.vel=Vec2((float)rr.Calc(spreadSpeed) - (spreadSpeed / 2), (float)rr.Calc(spreadSpeed) - (spreadSpeed / 2));
	configs.angleVel = (float)(rr.Calc(200)) / 10 - 10;
	obj->SetConfigs(configs);
	Add(obj);
}

/*
//######################## BLUEPRINTS
void GraphicObjects::AddVolcano(Vec2 p0, int size, int spreadSpeed, Vec2_<Color> colors)
{
	PartConf configs = PartConf();
	configs.pos = p0;
	configs.size = size;
	configs.colors = colors;
	configs.vel = colors;
	Add(&GraphicObjects::Particle(PartConf()));
	//Add(&GraphicObjects::Particle(p0, (float)rr.Calc(size / 3) + 1, Vec2((float)rr.Calc(spreadSpeed) - (spreadSpeed / 2), (float)rr.Calc(spreadSpeed) - (spreadSpeed / 2)), Vec2(0, 3), colors));
}
void GraphicObjects::AddSpark(Vec2 p0, int size, int spreadSpeed, Vec2_<Color> colors)
{
	Add(&GraphicObjects::Particle(p0, (float)rr.Calc(size / 3) + 1, Vec2((float)rr.Calc(spreadSpeed) - (spreadSpeed / 2), (float)rr.Calc(spreadSpeed) - (spreadSpeed / 2)), Vec2(0, 0), colors));
}
void GraphicObjects::AddShot(Vec2 p0, Vec2 pGoal, float speed, Vec2_<Color> colors)
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
	Add(&GraphicObjects::Shot(p0, Vec2((float)where, m).GetNormalized() * speed, Vec2(0.0f, 0.0f), 20, 3, colors));
}
*/