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
	Surface spriteSHEEP = { "Textures/Spritesheet.bmp" };
	//Surface s_Items[1];
	std::vector<Animation>	Fields;			//all needed Textures for a Fieldtype
	std::vector<Animation>	maskedFields;	//types that need speacial cyan masking
	//std::vector<Animation>	FieldsC;	//field connections
	std::vector<Animation>	Frames;
	std::vector<Animation>	windows;
	std::vector<Font>		fonts;
	void Update(float dt);
};

class FramesizeCollection						
{
	TexturesCollection* tC;
public:
	std::vector<RectI> FieldCon;					//precalculations fpr better performance

	FramesizeCollection(TexturesCollection* tC);
	void Update(Vei2 cellSize);
	std::vector<RectI> GetConOffset(Vei2 cSize)const;	//gets the rects for every connection from the conSpritesheet
	std::vector<SubAnimation> GetConnectionAnimationVec(int lookFor, Vei2 pos, bool masked = false, Matrix<int> aMat = Matrix<int>(3,3,0))const;
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
