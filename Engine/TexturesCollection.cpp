#include "TexturesCollection.h"

TexturesCollection::TexturesCollection(Graphics& gfx)
{
	//Frames
	s_Frames.push_back(spriteSHEEP.GetSupSurface(RectI(Vei2(0, 51), 70, 70)));
	//Items

	//Fields
	s_Fields.push_back(spriteSHEEP.GetSupSurface(RectI(Vei2(0 ,0), 50, 50)));
	s_Fields.push_back(spriteSHEEP.GetSupSurface(RectI(Vei2(51, 0), 50, 50)));
	s_Fields.push_back(spriteSHEEP.GetSupSurface(RectI(Vei2(102, 0), 50, 50)));
	//Fonts
	fonts.push_back(Font("Spritesheet.bmp", 9, 11, 9, 13, '!', '~', gfx));


}
