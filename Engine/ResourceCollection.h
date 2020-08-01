#pragma once
#include <vector>
#include "Surface.h"
#include "Font.h"
#include "Animation.h"
class TexturesCollection
{
public:
	TexturesCollection(Graphics& gfx);
	Surface spriteSHEEP = { "Textures/Spritesheet.bmp" };
	//Surface s_Items[1];
	std::vector<Animation>	s_Fields;
	std::vector<Animation>	s_FieldsC;		//Field connections
	std::vector<Surface>	s_Frames;
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
	TexturesCollection tC;
	FramesizeCollection fsC;
};

