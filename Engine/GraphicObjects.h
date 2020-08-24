#pragma once
#include "Rect.h"
#include <vector>
#include "Graphics.h"
#include <memory>
#include "RandyRandom.h"
#include "Matrix.h"
#include "SpriteEffect.h"
#include "GigaMath.h"
class GraphicObjects
{
	class Pictures
	{
	public:
		std::vector<Surface> tileFramePics;
		std::vector<Vei2> tfSize;
		Pictures()
		{
			tileFramePics.push_back(Surface("Textures/6.bmp"));
			tfSize.push_back(Vei2(30, 30));
		}
	};
public:
	struct PartConf	//Particle Configs
	{
		PartConf() = default;
		
		float size=5.0f;
		float angleVel = 0;
		bool killMe = false;
		int style = 0;
		int width=3;
		Vec2_<Color> colors = { Colors::Yellow, Colors::Red };
		Vec2 pos = Vec2(0.0f, 0.0f);
		Vec2 vel = Vec2(0.0f, 0.0f);
		Vec2 gravity = Vec2(0.0f, 2.0f);
		Matrix<int> matrix = Matrix<int>(1,1,1);
		const Pictures* pics=nullptr;
		std::vector<Vec2> body = { {-2,-2},{2,-2},{2,2},{-2,2},{3,5},{1,-7},{3,-5} };

		void ScaleBody(float size)
		{
			for (int i = 0; i < body.size(); i++)
			{
				body[i] *= size;
			}
		}
	};
	class Object
	{
	protected:
		PartConf configs;
	public:
		Object() = default;
		Object(PartConf& configs);
		virtual void Update(float dt);
		virtual void Draw(Graphics& gfx) {};
		bool ChoosenToDie() { return configs.killMe; }
		void SetConfigs(PartConf configs) { this->configs = configs; }
		Color GetColor() { return configs.colors.x; }
	};

	class Particle : public Object
	{
	public:
		Particle(PartConf& configs) : Object(configs) {}
		Particle() = default;
		void Draw(Graphics& gfx)override {
			gfx.DrawCircle((int)configs.pos.x, (int)configs.pos.y, configs.size, configs.size - (configs.size / 3), configs.colors.x, configs.colors.y);
		}
	};
	class Polygon : public Object
	{
	public:
		Polygon(PartConf& configs) : Object(configs){}
		Polygon() = default;
		void Draw(Graphics& gfx)override {
			for (int i = 0; i < configs.body.size()-1; i++)
			{
				gfx.DrawLine(configs.body[i] + configs.pos, configs.body[i+1] + configs.pos,SpriteEffect::Rainbow());
			}
			gfx.DrawLine(configs.body[configs.body.size()-1] + configs.pos, configs.body[0] + configs.pos, SpriteEffect::Rainbow());
		}
	};
	class TileFrame : public Object
	{

	public:
		TileFrame(PartConf& configs) : Object(configs) {}
		TileFrame() = default;
		void Draw(Graphics& gfx)override
		{
			Matrix<int> sur = Matrix<int>(3, 3, 0);
			for (int y = 0; y < configs.matrix.GetRows(); y++)
			{
				for (int x = 0; x < configs.matrix.GetColums(); x++)
				{
					if (y == 0 && x == 0 && configs.matrix[x][y])
					{
						//gfx.DrawSurface(RectI((Vei2)configs.pos, configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y), RectI(Vei2(0, 0), configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y),Graphics::GetScreenRect<int>(), configs.pics->tileFramePics[configs.style], SpriteEffect::Chroma(Colors::Magenta));
					}
				}
			}
		}
	};
	class Shot : public Object
	{
	public:
		Shot(PartConf& configs) : Object(configs){}
		Shot() = default;
		void Draw(Graphics& gfx)override {
			gfx.DrawLine(configs.pos, configs.pos + configs.vel * configs.size * 0.05f, configs.colors.x, (int)configs.width);
		}
	};
private:
	std::vector<std::unique_ptr<Object>> objects;
	Graphics& gfx;
	RandyRandom rr;
	Pictures pic;
public:
	GraphicObjects(Graphics& gfx);
	void Update(float dt);
	void Draw();
	void Add(Object* object)					//WHEN U ADD A NEW OBJECT U NEED TO ADD IT HERE ASS WELL
	{
		if (Particle* v = dynamic_cast<Particle*>(object))
		{
			objects.push_back(std::make_unique<Particle>(*v));
		}
		if (Shot* v = dynamic_cast<Shot*>(object))
		{
			objects.push_back(std::make_unique<Shot>(*v));
		}
		if (TileFrame* v = dynamic_cast<TileFrame*>(object))
		{
			objects.push_back(std::make_unique<TileFrame>(*v));
		}
		if (Polygon* v = dynamic_cast<Polygon*>(object))
		{
			objects.push_back(std::make_unique<Polygon>(*v));
		}
	}

	//Particle Blueprints			
	void AddVolcano(Vec2 p0, int size, int spreadSpeed, Vec2_<Color> colors = { Colors::Yellow,Colors::Red });
	//void AddSpark(Vec2 p0, int size, int spreadSpeed, Vec2_<Color> colors = { Colors::Yellow,Colors::Red });
	//void AddShot(Vec2 p0, Vec2 pGoal, float speed, Vec2_<Color> colors = { Colors::Yellow,Colors::Red });
	//void AddTileframe(Vec2 pos, Matrix<bool> size, int style);
	void AddVoc(Object* obj, const PartConf* configsTemplate, int size, int spreadSpeed);
};
typedef GraphicObjects GRAPH_OBJ;
typedef GraphicObjects::PartConf PARTCONF;
typedef GraphicObjects::Particle PARTICLE;
typedef GraphicObjects::Shot SHOT;
typedef GraphicObjects::TileFrame TILEFRAME;
typedef GraphicObjects::Polygon POLYGON;
/*
static class PartBlueprints
{
	static GraphicObjects::PartConf Volcano = GraphicObjects::PartConf(;
}
*/