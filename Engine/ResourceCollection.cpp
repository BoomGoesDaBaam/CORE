#include "ResourceCollection.h"
#include "Settings.h"
//	##### RESOURCE COLLECTION #####

ResourceCollection::ResourceCollection(Graphics& gfx)
	:
	tC(gfx),
	fsC(&tC)
{
	
}

//  ##### TEXTUREN #####

TexturesCollection::TexturesCollection(Graphics& gfx)
{
	//	Frames
	Frames.push_back(Animation(0.5f));
	for (int i = 0; i < 2; i++)
	{
		Frames[0].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(i * 71, 51), 70, 70)));
	}
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
	fonts.push_back(Font("Textures/Font1.bmp", 9, 11, 9, 13, '!', '~', gfx));

}
void TexturesCollection::Update(float dt)
{
	for (int i = 0; i < Fields.size(); i++)
	{
		Fields.at(i).Update(dt);
	}
	for (int i = 0; i < Frames.size(); i++)
	{
		Frames.at(i).Update(dt);
	}
}
//  ##### Framesize #####

FramesizeCollection::FramesizeCollection(TexturesCollection* tC):tC(tC)
{
	for (int i = 0; i < 4; i++)
	{
		FieldCon.push_back(RectI(Vei2(0, 0), 0, 0));
	}
	Update(Vei2(100, 100));								// !!! Hardcoded
}
std::vector<RectI> FramesizeCollection::GetConOffset(Vei2 cSize)const
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
std::vector<SubAnimation> FramesizeCollection::GetConnectionAnimationVec(int lookFor, Vei2 pos, bool masked, Matrix<int> aMat)const
{
	using namespace Settings;
	std::vector<SubAnimation> vec;
	std::vector<RectI> posInGrit = GetConOffset(Vei2(50, 50));
	assert(aMat.GetSize().x == 3 && aMat.GetSize().y == 3);
	auto v = tC->Fields.at(lookFor);
	if (masked)
	{
		v = tC->maskedFields.at(translateIntoMaskedType(lookFor));
	}

	if (FIDF(aMat[1][1], lookFor))
	{
		if (aMat[1][2] == lookFor)
		{
			if (aMat[0][1] == lookFor)	// 1
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(51, 0), 25, 25), posInGrit[0]));
			}
			if (aMat[0][1] != lookFor)	// 13
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(204, 0), 25, 25), posInGrit[0]));
			}
			if (aMat[2][1] == lookFor)	// 2
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(76, 0), 25, 25), posInGrit[1]));
			}
			if (aMat[2][1] != lookFor)	// 14
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(229, 0), 25, 25), posInGrit[1]));
			}
		}
		else if (aMat[1][2] != lookFor)
		{
			if (aMat[0][1] == lookFor)	//9
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(153, 0), 25, 25), posInGrit[0]));
			}
			if (FIDF(aMat[0][1], lookFor) && aMat[0][2] == lookFor)	// 5
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(102, 0), 25, 25), posInGrit[0]));
			}
			if (aMat[2][1] == lookFor)	//10
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(178, 0), 25, 25), posInGrit[1]));
			}
			if (FIDF(aMat[2][1], lookFor) && aMat[2][2] == lookFor)	// 6
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(127, 0), 25, 25), posInGrit[1]));
			}
		}
		if (aMat[1][0] == lookFor)
		{
			if (aMat[0][1] == lookFor)	// 3
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(51, 25), 25, 25), posInGrit[2]));
			}
			if (aMat[0][1] != lookFor)	// 15
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(204, 25), 25, 25), posInGrit[2]));
			}
			if (aMat[2][1] == lookFor)	// 4
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(76, 25), 25, 25), posInGrit[3]));
			}
			if (aMat[2][1] != lookFor)	// 16
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(229, 25), 25, 25), posInGrit[3]));
			}
		}
		else if (aMat[1][0] != lookFor)
		{
			if (FIDF(aMat[0][1], lookFor) && aMat[0][0] == lookFor)	// 7
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(102, 25), 25, 25), posInGrit[2]));
			}
			if (aMat[0][1] == lookFor)	// 11
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(153, 25), 25, 25), posInGrit[2]));
			}
			if (FIDF(aMat[2][1], lookFor) && aMat[2][0] == lookFor)	// 8
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(127, 25), 25, 25), posInGrit[3]));
			}
			if (aMat[2][1] == lookFor)	// 12
			{
				vec.push_back(SubAnimation(v, RectI(Vei2(178, 25), 25, 25), posInGrit[3]));
			}
		}
	}
	return vec;
}
bool FramesizeCollection::FIDF(int first, int second)const
{
	if (first != second)
	{
		for (int i = 0; i < Settings::nDiffFieldTypes; i++)
		{
			if (Settings::typeLayer[i] == first || Settings::typeLayer[i] == second)
			{
				return Settings::typeLayer[i] == first;
			}
		}
	}
	return false;
}