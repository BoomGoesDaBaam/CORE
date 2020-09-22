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
	frames.push_back(Animation(0.5f));
	for (int i = 0; i < 2; i++)
	{
		frames[0].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(i * 71, 51), 70, 70)));
	}
	//	window - Frames
	float delay = rng.GetNormalDist() * 3 + 1;
	windowsFrame.push_back(Animation(delay));
	windowsFrame[0].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(510, 1397), 140, 280)));

	windowsFrame.push_back(Animation(delay));
	windowsFrame[1].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(651, 1397), 140, 14)));

	windowsFrame.push_back(Animation(delay));
	windowsFrame[2].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(651, 1412), 140, 14)));
	//Obstacles goldchair
	//obstacles.push_back(delay);
	//obstacles[0].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(0,0), 8, 17)));
	
	//Buttons buttons
	buttons.push_back(0.1f);
	buttons[0].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(0, 0), 34, 9)));
	buttons.push_back(0.1f);
	for (int i = 0; i < 5; i++)
	{
		buttons[1].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(35, i * 10), 34, 9)));
	}

	buttons.push_back(0.1f);
	buttons[2].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(105, 0), 34, 9)));
	buttons.push_back(0.1f);
	for (int i = 0; i < 5; i++)
	{
		buttons[3].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(70, i * 10), 34, 9)));
	}
	//	Items


	//	Fields
	int nMasked = 0;
	for (int i = 0; i < Settings::nDiffFieldTypes; i++)				//i diffrent Fields
	{
		float delay = rng.GetNormalDist() * 3 + 1;
		fields.push_back(Animation(delay));
		for (int f = 0; f < 5; f++)			//f diffent animations
		{
			fields[i].Push(spriteSHEEP.GetSupSurface(RectI(Vei2((floor((float)i / 8)) * 255, 122 + f * 51 + (i % 8) * 255), 254, 50)));
			
			
			if (Settings::anyMaskedType(i))
			{
				maskedFields.push_back(Animation(delay));
				maskedFields[nMasked].Push(spriteSHEEP.GetSupSurface(RectI(Vei2((floor((float)i / 8)) * 255, 122 + f * 51 + (i % 8) * 255), 254, 50)));
				
				fields[i].SetKeepTime(fields[0].GetKeepTime());
				for (int y = 0; y < 50; y++)
				{
					for (int x = 0; x < 210; x++)
					{
						if (fields[i].GetPixel(f, x, y) == Colors::Magenta)
						{
							fields[i].PutPixel(f, x, y, fields[0].GetPixel(f, x, y));
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
	fields[6].SetKeepTime(0.3f);
	fields[7].SetKeepTime(0.4f);
	fields[0].SetKeepTime(0.3f);
	fields[12].SetKeepTime(0.3f);
	//Fonts
	fonts.push_back(Font("Textures/Font4.bmp", 28, '!', '~', Colors::FontDelimitor, Colors::FontNewLine, gfx));
}
void TexturesCollection::Update(float dt)
{
	for (int i = 0; i < fields.size(); i++)
	{
		fields.at(i).Update(dt);
	}
	for (int i = 0; i < maskedFields.size(); i++)
	{
		maskedFields.at(i).Update(dt);
	}
	for (int i = 0; i < frames.size(); i++)
	{
		frames.at(i).Update(dt);
	}
	for (int i = 0; i < windowsFrame.size(); i++)
	{
		windowsFrame.at(i).Update(dt);
	}
	for (int i = 0; i < buttons.size(); i++)
	{
		buttons.at(i).Update(dt);
	}
	for (int i = 0; i < obstacles.size(); i++)
	{
		obstacles.at(i).Update(dt);
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
	auto v = tC->fields.at(lookFor);
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