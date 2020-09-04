#include "ResourceCollection.h"
#include "Settings.h"
//	##### RESOURCE COLLECTION #####

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
	//	windows
	for (int i = 0; i < Settings::nDiffWindows; i++)
	{
		float delay = rng.GetNormalDist() * 3 + 1;
		windows.push_back(Animation(delay));
		for (int f = 0; f < 5; f++)			//f diffent animations
		{
			windows[i].Push(spriteSHEEP.GetSupSurface(RectI(Vei2((floor((float)i / 8)) * 255, 2162 + f * 51 + (i % 8) * 255), 254, 50)));
		}
	}
	//	Items

	//	Fields
	int nMasked = 0;
	for (int i = 0; i < Settings::nDiffFieldTypes; i++)				//i diffrent Fields
	{
		float delay = rng.GetNormalDist() * 3 + 1;
		Fields.push_back(Animation(delay));
		for (int f = 0; f < 5; f++)			//f diffent animations
		{
			Fields[i].Push(spriteSHEEP.GetSupSurface(RectI(Vei2((floor((float)i / 8)) * 255, 122 + f * 51 + (i % 8) * 255), 254, 50)));
			
			
			if (Settings::anyMaskedType(i))
			{
				maskedFields.push_back(Animation(delay));
				maskedFields[nMasked].Push(spriteSHEEP.GetSupSurface(RectI(Vei2((floor((float)i / 8)) * 255, 122 + f * 51 + (i % 8) * 255), 254, 50)));
				
				Fields[i].SetKeepTime(Fields[0].GetKeepTime());
				for (int y = 0; y < 50; y++)
				{
					for (int x = 0; x < 210; x++)
					{
						if (Fields[i].GetPixel(f, x, y) == Colors::Magenta)
						{
							Fields[i].PutPixel(f, x, y, Fields[0].GetPixel(f, x, y));
							maskedFields[nMasked].PutPixel(f, x, y, Colors::Magenta);
						}
					}
				}
			}
			if (f == 4)
			{
				//nMasked++;
			}
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
	for (int i = 0; i < 4; i++)
	{
		FieldCon.push_back(RectI(Vei2(0, 0), 0, 0));
	}
	Update(Vei2(100, 100));								// !!! Hardcoded
}
std::vector<RectI> FramesizeCollection::GetConOffset(Vei2 cSize)
{
	std::vector<RectI> v;
	RectI cur = RectI(Vei2(0, 0), cSize.x, cSize.y);		// topleft
	cur.bottom -= (25.f / 50.f) * cSize.y;
	cur.right -= (25.f / 50.f) * cSize.y;
	v.push_back(cur);

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);				// topright
	cur.bottom -= (25.f / 50.f) * cSize.y;
	cur.left += (25.f / 50.f) * cSize.y;
	v.push_back(cur);

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);				// bottomleft
	cur.top += (25.f / 50.f) * cSize.y;
	cur.right -= (25.f / 50.f) * cSize.y;
	v.push_back(cur);

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);				// bottomright
	cur.top += (25.f / 50.f) * cSize.y;
	cur.left += (25.f / 50.f) * cSize.y;
	v.push_back(cur);
	//new elements need to be added in loop and in GetPositionsOfCon!!!!!!!!!!!!!
	return v;
}
void FramesizeCollection::Update(Vei2 cSize)
{
	FieldCon = GetConOffset(cSize);

}