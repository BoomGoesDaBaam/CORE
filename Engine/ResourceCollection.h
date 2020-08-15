#pragma once
#include <vector>
#include "Surface.h"
#include "Font.h"
#include "Animation.h"
#include "Settings.h"
#include "RandyRandom.h"
#include "Settings.h"
#include "Matrix.h"
class TexturesCollection
{
	RandyRandom rng;
public:	

	TexturesCollection(Graphics& gfx);
	Surface spriteSHEEP = { "Textures/Spritesheet.bmp" };
	//Surface s_Items[1];
	std::vector<Animation>	Fields;			//All needed Textures for a Fieldtype
	//std::vector<Animation>	FieldsC;	//Field connections
	std::vector<Surface>	Frames;
	std::vector<Font>		fonts;
	void Update(float dt);
};

class FramesizeCollection
{

public:
	FramesizeCollection();
	std::vector<RectI> FieldCon;
	void Update(Vei2 cellSize);
};

class ResourceCollection
{
public:
	ResourceCollection(Graphics& gfx);
	TexturesCollection tC;		//Texturenansammlung
	FramesizeCollection fsC;	//Gives Framesizes for Frames for Drawing, wich need to be prescaled for performance
};

