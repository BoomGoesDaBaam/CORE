#pragma once
#include "Surface.h"
#include "Font.h"
#include <vector>
#include "Animation.h"
class TexturesCollection
{
public:
	Surface spriteSHEEP = { "Textures/Spritesheet.bmp" };
	//Surface s_Items[1];
	std::vector<Animation>	s_Fields;
	std::vector<Animation>	s_FieldsC;		//Field connections
	std::vector<Surface>	s_Frames;
	std::vector<Font>		fonts;
	TexturesCollection(Graphics& gfx);
	void Update(float dt);
};

