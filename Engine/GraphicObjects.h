#pragma once
#include "Rect.h"
#include <vector>
#include "Graphics.h"
#include <memory>
#include "RandyRandom.h"
#include "Matrix.h"
#include "SpriteEffect.h"
class GraphicObjects
{
	class ParticleConfigs
	{

	};
	class Pictures
	{
	public:
		std::vector<Surface> tileFramePics;
		std::vector<Vei2> tfSize;
		Pictures()
		{
			tileFramePics.push_back(Surface("Textures/6.bmp"));
			tfSize.push_back(Vei2(30,30));

		}
	};
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
		Object(Vec2 pos, Vec2 vel, Vec2 gravity, Vec2_<Color> colors);
		virtual void Update(float dt);
		virtual void Draw(Graphics& gfx) {};
		bool ChoosenToDie() { return killMe; }
		void SetPrimaryColor(Color c) { primC = c; }
		void SetSecondaryColor(Color c) { secC = c; }
		void SetColor(Vec2_<Color> colors) {primC = colors.x;secC = colors.y;}
		Color GetColor() { return primC; }
	};

	class Particle : public Object
	{
		float radius;
	public:
		Particle(Vec2 pos, float radius, Vec2 vel, Vec2 gravity, Vec2_<Color> colors) : Object(pos, vel, gravity, colors), radius(radius) {}
		void Draw(Graphics& gfx)override {
			gfx.DrawCircle((int)pos.x, (int)pos.y, radius, radius - (radius / 3), primC, secC);
		}
		void RealNice()
		{
			radius += 0.1f;
		}
	};
	class Polynom : public Object
	{
		std::vector<Vec2> body;

	public:
		Polynom(Vec2 pos, std::vector<Vec2> body, Vec2 vel, Vec2 gravity, Vec2_<Color> colors) : Object(pos, vel, gravity, colors), body(body){}
		void Draw(Graphics& gfx)override {
			for (int i = 0; i < body.size()-1; i++)
			{
				gfx.DrawLine(body[i] + pos, body[i+1] + pos, primC,1);
			}
			gfx.DrawLine(body[body.size()-1] + pos, body[0] + pos, primC, 3);
		}
	};
	class TileFrame : public Object
	{
		Matrix<int> matrix;
		
		int style = 0;
		const Pictures& pic;
	public:
		TileFrame(Vec2 pos, Matrix<int> size,const Pictures& pic, Vec2_<Color> colors, int style = 0) : Object(pos, Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f),colors), matrix(size), style(style),pic(pic) {}
		void Draw(Graphics& gfx)override
		{
			Matrix<int> sur = Matrix<int>(3, 3, 0);
			for (int y = 0; y < matrix.GetRaws(); y++)
			{
				for (int x = 0; x < matrix.GetColums(); x++)
				{
					if (y == 0 && x == 0 && matrix[x][y])
					{
						gfx.DrawSurface(RectI((Vei2)pos, pic.tfSize[style].x, pic.tfSize[style].y), RectI(Vei2(0, 0), pic.tfSize[style].x, pic.tfSize[style].y),Graphics::GetScreenRect<int>(), pic.tileFramePics[style], SpriteEffect::Chroma(Colors::Magenta));
					}
				}
			}
		}
	};
	
	class Shot : public Object
	{
		float width=3, length;
	public:
		Shot(Vec2 pos, Vec2 vel, Vec2 gravity, float length, float, Vec2_<Color> colors) : Object(pos, vel, gravity, colors), length(length) {}
		void Draw(Graphics& gfx)override {
			gfx.DrawLine(pos, pos + vel * length * 0.05f, primC, (int)width);
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
		if (Polynom* v = dynamic_cast<Polynom*>(object))
		{
			objects.push_back(std::make_unique<Polynom>(*v));
		}
	}

	//Particle Blueprints			
	void AddVolcano(Vec2 p0, int size, int spreadSpeed, Vec2_<Color> colors = { Colors::Yellow,Colors::Red });
	void AddSpark(Vec2 p0, int size, int spreadSpeed, Vec2_<Color> colors = { Colors::Yellow,Colors::Red });
	void AddShot(Vec2 p0, Vec2 pGoal, float speed, Vec2_<Color> colors = { Colors::Yellow,Colors::Red });
	//void AddTileframe(Vec2 pos, Matrix<bool> size, int style);

	typedef GraphicObjects GRAPH_OBJ;
	typedef GraphicObjects::Particle PARTICLE;
};
