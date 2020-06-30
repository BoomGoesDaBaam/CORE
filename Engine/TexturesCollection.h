#pragma once
#include "Surface.h"
#include "Font.h"
#include <vector>
class TexturesCollection
{
public:
	//Surface s_Frames[1];
	//Surface s_Items[1];
	std::vector<Surface>	s_Fields[2];
	std::vector<Font>		fonts[1];
	TexturesCollection(Graphics& gfx);
};

