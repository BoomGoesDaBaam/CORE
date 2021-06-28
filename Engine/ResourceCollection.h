#pragma once
#include <vector>
#include "Surface.h"
#include "Font.h"
#include "Animation.h"
#include "Settings.h"
#include "RandyRandom.h"
#include "Matrix.h"
#include "SpriteEffect.h"
class TexturesCollection
{
	RandyRandom rng;
public:	

	TexturesCollection(Graphics& gfx);
	void TexturesCollection::IdkCallOnce();
	Surface spriteSHEEP = { "Textures/Spritesheet.bmp" };
	//Surface s_Items[1];
	std::vector<Animation>	fields;			//new vectors need to be added in update function
	std::vector<Animation>	newFields;			//new vectors need to be added in update function
	std::vector<Animation>	maskedFields;	//types that need speacial masking (smurpf for example)
	std::vector<Animation>	frames;			//focused Cell 
	std::vector<Animation>	windowsFrame;	//Frames
	std::vector<Animation>	buttons;
	std::vector<Animation>	obstacles;
	std::vector<Animation>	multiObstacles;
	std::vector<Animation>	items;
	std::vector<Font>		fonts;		
	void Update(float dt);
};

class FramesizeCollection						
{
	TexturesCollection* tC;
public:
	std::vector<RectI> FieldCon;					//precalculations of connections

	FramesizeCollection(TexturesCollection* tC);
	void Update(Vei2 cellSize);
	std::vector<RectI> GetConOffset(Vei2 cSize)const;	//gets the rects for every connection from the conSpritesheet
	std::vector<SubAnimation> GetConnectionAnimationVec(int lookFor, bool masked = false, Matrix<int> aMat = Matrix<int>(3,3,0))const;
	bool FIDF(int first, int second)const;
};
class ResourceCollection
{
public:
	ResourceCollection(Graphics& gfx);
	TexturesCollection tC;		//Texturenansammlung
	FramesizeCollection fsC;	//Gives Framesizes for Frames for Drawing, wich need to be prescaled for performance
};
typedef std::shared_ptr<ResourceCollection> sharedResC;
