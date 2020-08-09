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
	//Frames
	Frames.push_back(spriteSHEEP.GetSupSurface(RectI(Vei2(0, 51), 70, 70)));
	//Items

	//	Fields
	for (int i = 0; i < Settings::nDiffFieldTypes; i++)
	{
		float delay = rng.GetNormalDist() * 3  + 1;
		Fields.push_back(Animation(delay));
	}

	for (int i = 0; i < Settings::nDiffFieldTypes; i++)				//i diffrent Fields
	{
		for (int f = 0; f < 5; f++)			//f diffent animations
			{
						Fields[i].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(0, 122 + f * 51 + i * 255), 210, 50)));
					
						//FieldsC[(__int64)i + Settings::nDiffFieldTypes].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(102, 122 + f * 51 + i * 255), 27, 6)));	//B
				
				}
			}
	
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
	for (int i = 0; i < 12; i++)
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
	
	/*
	RectI cur = RectI(Vei2(0,0), cSize.x, cSize.y);		//Field connections - straight lines
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

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);			//inner corners
	cur.left += (6.f / 50.f) * cSize.x;
	cur.right -= (25.f / 50.f) * cSize.x;
	cur.top += (6.f / 50.f) * cSize.y;
	cur.bottom -= (25.f / 50.f) * cSize.y;
	FieldCon[4] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.left += (25.f / 50.f) * cSize.x;
	cur.right -= (6.f / 50.f) * cSize.x;
	cur.top += (6.f / 50.f) * cSize.y;
	cur.bottom -= (25.f / 50.f) * cSize.y;
	FieldCon[5] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.left += (6.f / 50.f) * cSize.x;
	cur.right -= (25.f / 50.f) * cSize.x;
	cur.top += (25.f / 50.f) * cSize.y;
	cur.bottom -= (6.f / 50.f) * cSize.y;
	FieldCon[6] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.left += (25.f / 50.f) * cSize.x;
	cur.right -= (6.f / 50.f) * cSize.x;
	cur.top += (25.f / 50.f) * cSize.y;
	cur.bottom -= (6.f / 50.f) * cSize.y;
	FieldCon[7] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);			//outer corners
	cur.right -= (44.f / 50.f) * cSize.x;
	cur.bottom -= (44.f / 50.f) * cSize.y;
	FieldCon[8] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);	
	cur.left += (44.f / 50.f) * cSize.x;
	cur.bottom -= (44.f / 50.f) * cSize.y; 
	FieldCon[9] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.right -= (44.f / 50.f) * cSize.x;
	cur.top += (44.f / 50.f) * cSize.y;
	FieldCon[10] = cur;

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);
	cur.left += (44.f / 50.f) * cSize.x;
	cur.top += (44.f / 50.f) * cSize.y;
	FieldCon[11] = cur;
	*/

	//new elements need to be added in loop
}