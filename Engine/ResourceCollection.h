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
	void FillMaskedSurfaceWithWater();
	Surface spriteSHEEP = { "Textures/Spritesheet.bmp" };
	//Surface s_Items[1];
	std::vector<Animation>	fields;			//new vectors need to be added in update function
	std::vector<Animation>	newFields;			//new vectors need to be added in update function
	std::vector<Animation>	maskedFields;	//types that need speacial masking (smurpf for example)
	std::vector<Animation>	newMaskedFields;	//types that need speacial masking (smurpf for example)
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
	std::map<std::string, RectI> framePos;
	std::vector<RectI> fieldCon;					//precalculations of connections
	float guiScale = 1.0f;
public:

	FramesizeCollection(TexturesCollection* tC);
	void UpdateFieldCon(Vei2 cellSize);
	void UpdateFramePos(Vec2 screenSize, float guiScale = 1.0f);

	std::vector<RectI> GetConOffset(Vei2 cSize)const;	//gets the rects for every connection from the conSpritesheet
	std::vector<SubAnimation> GetConnectionAnimationVec(int lookFor, bool masked = false, Matrix<int> aMat = Matrix<int>(3,3,0))const;
	std::vector<SubAnimation> GetConnectionAnimationVecNew(int lookFor, bool masked, Matrix<int> aMat)const;
	bool FIDF(int first, int second)const;

	//
	const std::vector<RectI>& GetFieldCon()const;
	const RectI& GetFramePos(std::string value)const;
	float GetGuiScale()const;
};
class ResourceCollection
{
	TexturesCollection tC;		//collection of pictures
	FramesizeCollection fsC;
public:
	ResourceCollection(Graphics& gfx);//scaling graphics

	const TexturesCollection& GetSurf()const;
	const FramesizeCollection& GetFrameSize()const;

	//Functionality
	// Textures
	void UpdateSurfs(float dt);
	// Framesize
	void UpdateFieldCon(Vei2 size);
};
typedef std::shared_ptr<ResourceCollection> sharedResC;
