#include "ResourceCollection.h"
#include "Settings.h"
//	##### MAIN #####

ResourceCollection::ResourceCollection(Graphics& gfx)
	:
	tC(gfx)
{
	
}

//  ##### TEXTUREN #####

TexturesCollection::TexturesCollection(Graphics& gfx)
{
	//	Frames
	Frames.push_back(spriteSHEEP.GetSupSurface(RectI(Vei2(0, 51), 70, 70)));
	//	Items

	//	Fields
	for (int i = 0; i < Settings::nDiffFieldTypes; i++)				//i diffrent Fields
	{
		float delay = rng.GetNormalDist() * 3 + 1;
		Fields.push_back(Animation(delay));
		for (int f = 0; f < 5; f++)			//f diffent animations
		{
			Fields[i].Push(spriteSHEEP.GetSupSurface(RectI(Vei2((floor((float)i / 8)) * 211, 122 + f * 51 + (i % 8) * 255), 210, 50)));
		}
	}
	//Delay anpassen
	Fields[6].SetKeepTime(0.3f);
	Fields[7].SetKeepTime(0.4f);
	Fields[0].SetKeepTime(0.3f);
	Fields[12].SetKeepTime(0.3f);
	//Fonts
	fonts.push_back(Font("Spritesheet.bmp", 9, 11, 9, 13, '!', '~', gfx));


}
void TexturesCollection::Update(float dt)
{
	for (int i = 0; i < Fields.size(); i++)
	{
		Fields.at(i).Update(dt);
	}
}

//  ##### Framesize #####

FramesizeCollection::FramesizeCollection()
{
	for (int i = 0; i < 8; i++)
	{
		FieldCon.push_back(RectI(Vei2(0, 0), 0, 0));
	}
	Update(Vei2(100, 100));								// !!! Hardcoded
}

void FramesizeCollection::Update(Vei2 cSize)
{
	RectI cur = RectI(Vei2(0, 0), cSize.x, cSize.y);		//Field connections 1-4
	cur.bottom -= (25.f / 50.f) * cSize.y;
	cur.right -= (25.f / 50.f) * cSize.y;
	FieldCon[0] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);		
	cur.bottom -= (25.f / 50.f) * cSize.y;
	cur.left += (25.f / 50.f) * cSize.y;
	FieldCon[1] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);		
	cur.top += (25.f / 50.f) * cSize.y;
	cur.right -= (25.f / 50.f) * cSize.y;
	FieldCon[2] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);		
	cur.top += (25.f / 50.f) * cSize.y;
	cur.left += (25.f / 50.f) * cSize.y;
	FieldCon[3] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);			//outer corners
	cur.right -= (44.f / 50.f) * cSize.x;
	cur.bottom -= (44.f / 50.f) * cSize.y;
	FieldCon[4] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.left += (44.f / 50.f) * cSize.x;
	cur.bottom -= (44.f / 50.f) * cSize.y;
	FieldCon[5] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.right -= (44.f / 50.f) * cSize.x;
	cur.top += (44.f / 50.f) * cSize.y;
	FieldCon[6] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.left += (44.f / 50.f) * cSize.x;
	cur.top += (44.f / 50.f) * cSize.y;
	FieldCon[7] = cur;
	//new elements need to be added in loop and in GetPositionsOfCon!!!!!!!!!!!!!
}
std::vector<RectI> FramesizeCollection::GetPositionsOfCon(Vei2 cSize)
{
	std::vector<RectI> FieldConRect;
	for (int i = 0; i < 8; i++)
	{
		FieldConRect.push_back(RectI(Vei2(0, 0), 0, 0));
	}

	RectI cur = RectI(Vei2(0, 0), cSize.x, cSize.y);		//Field connections 1-4
	cur.bottom -= (25.f / 50.f) * cSize.y;
	cur.right -= (25.f / 50.f) * cSize.y;
	FieldConRect[0] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.bottom -= (25.f / 50.f) * cSize.y;
	cur.left += (25.f / 50.f) * cSize.y;
	FieldConRect[1] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.top += (25.f / 50.f) * cSize.y;
	cur.right -= (25.f / 50.f) * cSize.y;
	FieldConRect[2] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.top += (25.f / 50.f) * cSize.y;
	cur.left += (25.f / 50.f) * cSize.y;
	FieldConRect[3] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);			//outer corners
	cur.right -= (44.f / 50.f) * cSize.x;
	cur.bottom -= (44.f / 50.f) * cSize.y;
	FieldConRect[4] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.left += (44.f / 50.f) * cSize.x;
	cur.bottom -= (44.f / 50.f) * cSize.y;
	FieldConRect[5] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.right -= (44.f / 50.f) * cSize.x;
	cur.top += (44.f / 50.f) * cSize.y;
	FieldConRect[6] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.left += (44.f / 50.f) * cSize.x;
	cur.top += (44.f / 50.f) * cSize.y;
	FieldConRect[7] = cur;

	assert(FieldConRect.size() == FieldCon.size());
	return FieldConRect;
}