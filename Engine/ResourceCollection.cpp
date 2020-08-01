#include "ResourceCollection.h"
//	##### MAIN #####

ResourceCollection::ResourceCollection(Graphics& gfx)
	:
	tC(gfx)
{
	
}

//  ##### TEXTUREN #####

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
	for (int i = 0; i < 3; i++)
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


//  ##### Framesize #####

FramesizeCollection::FramesizeCollection()
{
	for (int i = 0; i < 4; i++)
	{
		FieldCon.push_back(RectI(Vei2(0, 0), 0, 0));
	}
	Update(Vei2(100, 100));								// !!! Hardcoded
}

void FramesizeCollection::Update(Vei2 cSize)
{
	RectI cur = RectI(Vei2(0,0), cSize.x, cSize.y);
	cur.bottom -= (44.f / 50.f) * cSize.y;
	FieldCon[0] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.top += (44.f / 50.f) * cSize.y;
	FieldCon[1] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.right -= (44.f / 50.f) * cSize.y;
	FieldCon[2] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.left += (44.f / 50.f) * cSize.y;
	FieldCon[3] = cur;
}