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
	windowsFrame.push_back(Animation(delay));					//standart design
	windowsFrame[0].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(510, 1397), 140, 280)));

	windowsFrame.push_back(Animation(delay));
	windowsFrame[1].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(651, 1397), 140, 14)));

	windowsFrame.push_back(Animation(delay));
	windowsFrame[2].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(651, 1412), 140, 14)));

	windowsFrame.push_back(Animation(delay));					//	next turn
	windowsFrame[3].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(284, 61), 120, 60)));

	windowsFrame.push_back(Animation(delay));					//	button to build screen
	windowsFrame[4].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(405, 1), 60, 60)));

	windowsFrame.push_back(Animation(delay));					//	build screen background
	windowsFrame[5].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(1150, 0), 800, 600)));

	windowsFrame.push_back(Animation(delay));					//	button back to game
	windowsFrame[6].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(466, 1), 60, 60)));

	windowsFrame.push_back(Animation(delay));					//	composition = 0 bg
	windowsFrame[7].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(527, 1), 180, 60)));
	//Obstacles



	obstacles.push_back(0.2f);//#0
	for (int i = 0; i < 1; i++)
	{
		obstacles[0].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(0, 2417 + i * 17), 16, 16)));
	}
	obstacles.push_back(0.2f);//#1
	obstacles[1].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(17, 2417), 8, 8)));

	multiObstacles.push_back(0.2f);//##1
	for (int i = 0; i < 5; i++)
	{
		multiObstacles[0].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(26, 2417 + i * 41), 40, 40)));
	}
	obstacles.push_back(0.2f);//#2
	for (int i = 0; i < 5; i++)
	{
		obstacles[2].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(67, 2417 + i * 9), 8, 8)));
	}
	obstacles.push_back(0.2f);//#3
	obstacles[3].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(76, 2417), 32, 32)));

	obstacles.push_back(0.5f);//#4
	obstacles[4].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(17, 2417), 8, 8)));

	multiObstacles.push_back(0.5f);//##4
	for (int i = 0; i < 5; i++)
	{
		multiObstacles[1].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(109, 2417 + i * 41), 40, 40)));
	}

	obstacles.push_back(0.2f);//#5
	for (int i = 0; i < 2; i++)
	{
		obstacles[5].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(150, 2417 + i * 9), 8, 8)));
	}

	obstacles.push_back(0.5f);//#6
	obstacles[6].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(159, 2417), 8, 8)));

	obstacles.push_back(0.5f);//#7
	obstacles[7].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(168, 2417), 8, 8)));

	obstacles.push_back(0.5f);//#8
	obstacles[8].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(177, 2417), 16, 16)));

	obstacles.push_back(0.5f);//#9
	obstacles[9].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(194, 2417), 32, 32)));

	obstacles.push_back(0.5f);//#10
	obstacles[10].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(227, 2417), 8, 8)));

	obstacles.push_back(0.5f);//#11
	obstacles[11].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(236, 2417), 8, 8)));
	obstacles.push_back(0.5f);//#12
	obstacles[12].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(245, 2417), 8, 8)));
	obstacles.push_back(0.5f);//#13
	obstacles[13].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(254, 2417), 8, 8)));
	obstacles.push_back(0.5f);//#14
	obstacles[14].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(263, 2417), 8, 8)));
	obstacles.push_back(0.5f);//#15
	obstacles[15].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(272, 2417), 8, 8)));
	obstacles.push_back(0.5f);//#16
	obstacles[16].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(281, 2417), 8, 8)));
	obstacles.push_back(0.5f);//#17
	obstacles[17].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(290, 2417), 8, 8)));
	obstacles.push_back(0.5f);//#18
	obstacles[18].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(299, 2417), 8, 8)));
	obstacles.push_back(0.5f);//#19
	obstacles[19].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(308, 2417), 8, 8)));
	obstacles.push_back(0.5f);//#20
	obstacles[20].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(317, 2417), 8, 8)));
	
	obstacles.push_back(0.5f);//#21
	obstacles[21].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(326, 2417), 24, 24)));
	obstacles.push_back(0.5f);//#22
	obstacles[22].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(351, 2417), 24, 24)));
	obstacles.push_back(0.5f);//#23
	obstacles[23].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(376, 2417), 32, 32)));
	obstacles.push_back(0.5f);//#24
	obstacles[24].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(409, 2417), 32, 32)));
	obstacles.push_back(0.5f);//#25
	obstacles[25].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(442, 2417), 32, 32)));
	obstacles.push_back(0.5f);//#26
	obstacles[26].Push(spriteSHEEP.GetSupSurface(RectI(Vei2(475, 2417), 40, 40)));

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
			fields[i].Push(spriteSHEEP.GetSupSurface(RectI(Vei2((int)((floor((float)i / 8)) * 255), 122 + f * 51 + (i % 8) * 255), 254, 50)));
			
			
			if (Settings::anyMaskedType(i))
			{
				maskedFields.push_back(Animation(delay));

				maskedFields[nMasked].Push(spriteSHEEP.GetSupSurface(RectI(Vei2((int)((floor((float)i / 8)) * 255), 122 + f * 51 + (i % 8) * 255), 254, 50)));
				
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
	fonts.push_back(Font("Textures/Font5.bmp", 112, '!', '~', Colors::FontDelimitor, Colors::FontNewLine, gfx));
}
void TexturesCollection::Update(float dt)
{
	for (int i = 0; i < (int)fields.size(); i++)
	{
		fields.at(i).Update(dt);
	}
	for (int i = 0; i < (int)maskedFields.size(); i++)
	{
		maskedFields.at(i).Update(dt);
	}
	for (int i = 0; i < (int)frames.size(); i++)
	{
		frames.at(i).Update(dt);
	}
	for (int i = 0; i < (int)windowsFrame.size(); i++)
	{
		windowsFrame.at(i).Update(dt);
	}
	for (int i = 0; i < (int)buttons.size(); i++)
	{
		buttons.at(i).Update(dt);
	}
	for (int i = 0; i < (int)obstacles.size(); i++)
	{
		obstacles.at(i).Update(dt);
	}
	for (int i = 0; i < (int)multiObstacles.size(); i++)
	{
		multiObstacles.at(i).Update(dt);
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
	cur.bottom -= (int)((25.f / 50.f) * cSize.y);
	cur.right -= (int)((25.f / 50.f) * cSize.y);
	v.push_back(cur);

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);				// topright
	cur.bottom -= (int)((25.f / 50.f) * cSize.y);
	cur.left += (int)((25.f / 50.f) * cSize.y);
	v.push_back(cur);

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);				// bottomleft
	cur.top += (int)((25.f / 50.f) * cSize.y);
	cur.right -= (int)((25.f / 50.f) * cSize.y);
	v.push_back(cur);

	cur = RectI(Vei2(0, 0), cSize.x, cSize.y);				// bottomright
	cur.top += (int)((25.f / 50.f) * cSize.y);
	cur.left += (int)((25.f / 50.f) * cSize.y);
	v.push_back(cur);
	//new elements need to be added in loop and in GetPositionsOfCon!!!!!!!!!!!!!
	return v;
}
void FramesizeCollection::Update(Vei2 cSize)
{
	FieldCon = GetConOffset(cSize);

}
std::vector<SubAnimation> FramesizeCollection::GetConnectionAnimationVec(int lookFor, bool masked, Matrix<int> aMat)const
{
	using namespace Settings;
	std::vector<SubAnimation> vec;
	std::vector<RectI> posInGrit = GetConOffset(Vei2(50, 50));
	assert(aMat.GetSize().x == 3 && aMat.GetSize().y == 3);
	Animation& v = tC->fields.at(lookFor);
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
bool FramesizeCollection::FIDF(int first, int second)const	//FIRST IS DRAWN FIRST
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