#include "TexturesCollection.h"

TexturesCollection::TexturesCollection(Graphics& gfx)
{
	//Frames
	s_Frames.push_back(spriteSHEEP.GetSupSurface(RectI(Vei2(0, 51), 70, 70)));
	//Items

	//Fields
	s_Fields.push_back(Animation(1.0f));
	s_Fields.push_back(Animation(3.0f));
	s_Fields.push_back(Animation(0.5f));

	s_FieldsC.push_back(Animation(1.0f));
	s_FieldsC.push_back(Animation(3.0f));
	s_FieldsC.push_back(Animation(0.5f));
	for (int i = 0; i < 3;i++)
	{
		for (int f = 0; f < 5; f++)
		{
			s_FieldsC[i].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(51, 122 + f * 51 + i * 255), 50, 50)));
			s_Fields[i].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(0, 122 + f * 51 + i * 255), 50, 50)));
		}
	}

	//Fonts
	fonts.push_back(Font("Spritesheet.bmp", 9, 11, 9, 13, '!', '~', gfx));


}
void TexturesCollection::Update(float dt)
{
	for (int i = 0; i < s_Fields.size(); i++)
	{
		s_Fields.at(i).Update(dt);
	}
}