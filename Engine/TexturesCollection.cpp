#include "TexturesCollection.h"

TexturesCollection::TexturesCollection(Graphics& gfx)
{
	//Frames

	//Items

	//Fields
	s_Fields->push_back(Surface("Textures/env_0.bmp"));
	s_Fields->push_back(Surface("Textures/env_1.bmp"));
	//Fonts
	fonts->push_back(Font("Spritesheet.bmp", 9, 11, 9, 13, '!', '~', gfx));


}
