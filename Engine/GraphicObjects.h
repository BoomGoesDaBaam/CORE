#pragma once
#include "Rect.h"
#include <vector>
#include "Graphics.h"
#include <memory>
#include "RandyRandom.h"
#include "Matrix.h"
#include "SpriteEffect.h"
#include "GigaMath.h"
#include "ResourceCollection.h"
#include "World.h"

class GraphicObjects				
{
	
public:
	struct PartConf	//Particle Configs
	{
		PartConf(std::shared_ptr<ResourceCollection> resC) :resC(std::move(resC)) {}
		std::shared_ptr<ResourceCollection> resC;
		float size=5.0f;
		float angleVel = 0;
		bool killMe = false;
		int style = 0;
		Vec2_<Color> colors = { Colors::Yellow, Colors::Red };
		Vec2 pos = Vec2(0.0f, 0.0f);
		Vec2 vel = Vec2(0.0f, 0.0f);
		Vec2 gravity = Vec2(0.0f, 0.0f);
		std::vector<Vec2> body = { {-2,-2},{2,-2},{2,2},{-2,2},{3,5},{1,-7},{3,-5} };
		void ScaleBody(float size)
		{
			for (int i = 0; i < (int)(body.size()); i++)
			{
				body[i] *= size;
			}
		}
		RectF GetRect()
		{
			return RectF(Vec2(pos.x, pos.y), size, size);
		}
	};
	class Object
	{
	public:
		PartConf configs;
	public:
		Object() = default;
		Object(PartConf& configs);

		virtual void Update(float dt);
		virtual void Draw(Graphics& gfx) {};
		bool ChoosenToDie() { return configs.killMe; }
		void SetConfigs(PartConf configs) { this->configs = configs; }
		Color GetColor() { return configs.colors.x; }
		void SetPos(Vec2 newP)
		{
			configs.pos = newP;
		}
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
			for (int i = 0; i < (int)(configs.body.size())-1; i++)
			{
				gfx.DrawLine(configs.body[i] + configs.pos, configs.body[(__int64)i+1] + configs.pos,SpriteEffect::Rainbow());
			}
			gfx.DrawLine(configs.body[configs.body.size()-1] + configs.pos, configs.body[0] + configs.pos, SpriteEffect::Rainbow());
		}
	};
	
	class TileFrame : public Object
	{
		Matrix<int> outline, matrix;
		std::vector<RectI> offset;
		
	public:
		TileFrame(PartConf& configs, Matrix<int> matrix) 
			: Object(configs),
			matrix(matrix),
			offset(configs.resC->GetFrameSize().GetConOffset(Vei2((int)configs.size, (int)configs.size))),
			outline(matrix.GetMatPlusZeroOutline())
		{
			
		}
		
		void Draw(Graphics& gfx)override
		{
			for (int y = 0; y < outline.GetRows(); y++)
			{
				for (int x = 0; x < outline.GetColums(); x++)
				{
					if (outline[x][y] == 1)
					{
						//gfx.DrawSurface((RectI)configs.GetRect() + Vei2((int)configs.size * (x-1), (int)configs.size * (y-1)), RectI(Vei2(0, 0), 50, 50), configs.resC->tC.windows.at(0).GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						//gfx.DrawSurface(configs.pos, RectI(Vei2(0, 0), 50, 50), configs.resC->tC.windows[0].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
						//gfx.DrawSurface(RectI((Vei2)configs.pos, configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y), RectI(Vei2(0, 0), configs.pics->tfSize[configs.style].x, configs.pics->tfSize[configs.style].y),Graphics::GetScreenRect<int>(), configs.pics->tileFramePics[configs.style], SpriteEffect::Chroma(Colors::Magenta));
					}
					else
					{
						auto a = outline.GetAroundMatrix(Vei2(x, y));
						a.MirrowVertical();
						//gfx.DrawConnections(1,(Vei2) configs.pos + Vei2((int)configs.size * (x-1), (int)configs.size * (y-1)), a, offset, configs.resC->tC.windows[0].GetCurSurface(), SpriteEffect::Chroma(Colors::Magenta));
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
			gfx.DrawLine(configs.pos, configs.pos + configs.vel * configs.size * 0.05f, configs.colors.x, 4);
		}
	};

private:
	Graphics& gfx;
	RandyRandom rr;
	std::shared_ptr<ResourceCollection> resC;
	const TexturesCollection* tC;
	const FramesizeCollection* fsC;
public:
	std::vector<std::unique_ptr<Object>> objects;
	GraphicObjects(Graphics& gfx, std::shared_ptr<ResourceCollection> resC);
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
			objects.push_back(std::move(std::make_unique<TileFrame>(*v)));
		}
		if (Polygon* v = dynamic_cast<Polygon*>(object))
		{
			objects.push_back(std::make_unique<Polygon>(*v));
		}
		
	}

	//Particle Blueprints			
	//void AddVolcano(Vec2 p0, int size, int spreadSpeed, Vec2_<Color> colors = { Colors::Yellow,Colors::Red });
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