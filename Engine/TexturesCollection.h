#pragma once
#include "Surface.h"
#include "Font.h"
#include <vector>
class TexturesCollection
{
public:
	Surface spriteSHEEP = { "Textures/Spritesheet.bmp" };
	//Surface s_Frames[1];
	//Surface s_Items[1];
	std::vector<Surface>	s_Fields;
	std::vector<Font>		fonts;
	TexturesCollection(Graphics& gfx);
};

