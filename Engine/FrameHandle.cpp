#pragma once
#include "FrameHandle.h"
//		### Componente ###
Text* Component::AddText(std::string text, RectF pos, int size,const Font* f, Color c, std::string key, std::vector<int> activInStates, int textLoc)
{
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<Text>(text, pos, size, f, c, activInStates, this, textLoc, buffer);
	return static_cast<Text*>(comps[key].get());
}
TextBox* Component::AddTextBox(std::string text, RectF pos, int size, const Font* f, Color c, std::string key, std::vector<int> activInStates, int textLoc)
{
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<TextBox>(text, pos, size, f, c, activInStates, this, textLoc, buffer);
	return static_cast<TextBox*>(comps[key].get());
}
Button* Component::AddButton(RectF pos,const Animation* a,const Animation* aHover, std::string key,const Font* f, std::vector<int> activInStates)
{
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<Button>(pos, a, aHover, activInStates, f, this, buffer);
	return static_cast<Button*>(comps[key].get());
}
CheckBox* Component::AddCheckBox(RectF pos, std::queue<FrameEvent>* buffer, sharedResC resC, std::string key, std::vector<int> activInStates)
{
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<CheckBox>(pos, this, buffer, resC, activInStates);
	return static_cast<CheckBox*>(comps[key].get());
}
Image* Component::AddImage(RectF pos, const Animation* a, const Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates)
{
	//RectF pos, Animation* a, Animation* aHover, Component* parentC, std::queue<FrameEvent>* buffer, std::vector<int> activInStates
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<Image>(pos, a, aHover, this, buffer, activInStates);
	return static_cast<Image*>(comps[key].get());
}
GrabImage* Component::AddGrabImage(RectF pos, const Animation* a, const Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates)
{
	//(RectF pos, Animation * a, Animation * aHover, Component * parentC, std::queue<FrameEvent>*buffer, std::vector<int> activInStates);
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<GrabImage>(pos, a, aHover, this, buffer, activInStates);
	return static_cast<GrabImage*>(comps[key].get());
}
Frame* Component::AddFrame(RectF pos, int type, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates)
{
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<Frame>(pos, type, resC, this, buffer);
	comps[key]->activInStates = activInStates;
	return static_cast<Frame*>(comps[key].get());
}
//
Text::Text(std::string text, RectF pos, int size,const Font* f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc, std::queue<FrameEvent>* buffer)
	:
	Component(pos, parentC, buffer),
	f(f),
	size(size),
	textLoc(textLoc)
{
	this->activInStates = activInStates;
	this->text = text;
	this->c = c;
}
TextBox::TextBox(std::string text, RectF pos, int size, const Font* f, Color c, std::vector<int> activInStates, Component* parentC, int textLoc, std::queue<FrameEvent>* buffer)
	:
	Text("text should be shown",pos,size,f,c,activInStates, parentC,0, buffer),
	lines(SplitTextToLines(f,text, size, (int)pos.GetWidth()))
{

}
std::vector<std::string> TextBox::SplitTextToLines(const Font* font, std::string text, int size, int width)
{
	std::vector<std::string> lines;
	const std::vector<RectI>& cRects = font->GetCharRects();
	int wordWidthInChar = 0;
	float wordWidthInPixel = 0;
	int charStart = 0;
	int first = font->GetFirst();
	int last = font->GetLast();

	lines.push_back("");
	for (int i = 0; i < (int)text.length(); i++)
	{
		if (text[i] >= first && text[i] <= last)
		{
			wordWidthInChar++;
			float ratio = (float)cRects[(__int64)text[i] - first].GetWidth() / cRects[(__int64)text[i] - first].GetHeight();
			Vec2 charSize(ratio * size, (float)size);
			if (wordWidthInPixel + ((float)size * 0.9f * ratio) >= width)
			{
				lines.push_back("");
				wordWidthInPixel = 0;
			}
			wordWidthInPixel += ((float)size * 0.9f * ratio);
		}
		if (text[i] == ' ')
		{
			wordWidthInChar++;
			lines[(int)lines.size() - 1] += text.substr(charStart, wordWidthInChar);
			charStart += wordWidthInChar;
			wordWidthInChar = 0;
			if (wordWidthInPixel + (size * 0.7f) >= width)
			{
				lines.push_back("");
				wordWidthInPixel = 0;
			}
			wordWidthInPixel += ((float)(size * 0.7f));
		}
	}
	if (wordWidthInChar > 0)
	{
		lines[(int)lines.size() - 1] += text.substr(charStart, wordWidthInChar);
	}
	return lines;
}
const std::vector<std::string>& TextBox::GetLines()const
{
	return lines;
}
Button::Button(RectF pos, const Animation* a, const Animation* aHover, std::vector<int> activInStates, const Font* f, Component* parentC, std::queue<FrameEvent>* buffer)
	:
	Text("",pos, 10,f,Colors::Black,activInStates, parentC,0, buffer)
{
	this->a = a;
	this->aHover = aHover;
	hitable = true;
	//if (Frame* p = dynamic_cast<Frame*>(parentC))
	//{
		//pF = p;
	//}
	//else 
	if (PageFrame* p = dynamic_cast<PageFrame*>(parentC))
	{
		ppF = p;
	}
	this->activInStates = activInStates;
}
Image::Image(RectF pos, const Animation* a, const Animation* aHover, Component* parentC, std::queue<FrameEvent>* buffer, std::vector<int> activInStates)
	:
	Component(pos, parentC, buffer),
	a(a),
	aHover(aHover)
{
	this->activInStates = activInStates;
	this->hitable = true;
}
GrabImage::GrabImage(RectF pos, const Animation* a, const Animation* aHover, Component* parentC, std::queue<FrameEvent>* buffer, std::vector<int> activInStates)
	:
	Image(pos, a, aHover, parentC, buffer, activInStates)
{

}
/*
Composition::Composition(RectF pos, sharedResC resC, std::vector<int> activInStates, Component* parentC, std::queue<FrameEvent>* buffer, int type)
	:
	Frame(pos, 0, resC, parentC, buffer)
{
	/*
	if (type == 0)
	{
		a = &this->resC->GetSurf().windowsFrame[7];
		aHover = &this->resC->GetSurf().windowsFrame[7];
		this->activInStates = activInStates;
	}
}
*/

CheckBox::CheckBox(RectF pos, Component* parentC, std::queue<FrameEvent>* buffer, sharedResC resC, std::vector<int> activInStates)
	:
	Component(pos, parentC, buffer),
	resC(std::move(resC))
{
	this->activInStates = activInStates;
	this->hitable = true;
}
//		### Framehandle::Frame ###

Frame::Frame(RectF pos, int type, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer)
	:
	Component(pos, parentC,buffer),
	resC(std::move(resC))
{
	this->type = type;
	this->nStates = 1;
	assert(type == 0 || type == 1 || type == -1);
	if (type == 0)
	{
		nStates = 2;
		activInStates = { 1, 0 };
	}

}

PageFrame::PageFrame(RectF pos, int type, sharedResC resC, Component* parentC, int nPages, std::queue<FrameEvent>* buffer)
	:
	Frame(pos, type, resC, parentC,buffer),
	nPages(nPages)
{
	std::vector<int> a = {0, 1};
	if (type == 1)
	{
		a = { 1 };
	}
	std::vector<int> activOnPagesL;
	activOnPagesL.push_back(0);
	for (int i = 0; i < nPages - 1; i++)
	{
		activOnPagesL.push_back(1);
	}
	Button* b1 = AddButtonPF(RectF(Vec2(5, 18), 34, 9), &resC->GetSurf().buttons[0], &resC->GetSurf().buttons[1], "b_left", &resC->GetSurf().fonts[0],a, activOnPagesL);
	b1->bFunc = B1;
	std::vector<int> activOnPagesR;
	for (int i = 0; i < nPages - 1; i++)
	{
		activOnPagesR.push_back(1);
	}
	activOnPagesR.push_back(0);
	Button* b2 = AddButtonPF(RectF(Vec2(99, 18), 34, 9), &resC->GetSurf().buttons[2], &resC->GetSurf().buttons[3], "b_right", &resC->GetSurf().fonts[0],a, activOnPagesR);
	b2->bFunc = B2;
}

void Frame::SetText(std::string text, std::string key)
{
	std::map<std::string, std::unique_ptr<Component>>::iterator it = comps.find(key);
	if (it != comps.end())
	{
		it->second->text = text;
	}
}
bool Frame::Hit(Vec2 mP)
{
	if (IsVisible())
	{
		assert(type == 0 || type == 1 || type == -1);
		Vei2 mpRelTop = (Vei2)(mP - GetPos().GetTopLeft<float>());
		Vei2 mpRelBottom = (Vei2)(mP - GetPos().GetTopLeft<float>());
		Vec2 scaleHead;
		Vec2 scaleBottom;
		switch (type)
		{
		case 0:
			scaleHead = Vec2(resC->GetSurf().windowsFrame[1].GetSize()) / Vec2(pos.GetSize());
			scaleBottom = Vec2(resC->GetSurf().windowsFrame[0].GetSize()) / Vec2(pos.GetSize());
			mpRelTop.x = (int)(mpRelTop.x * scaleHead.x);
			mpRelTop.y = (int)(mpRelTop.y * scaleHead.y);
			mpRelBottom.x = (int)(mpRelBottom.x * scaleBottom.x);
			mpRelBottom.y = (int)(mpRelBottom.y * scaleBottom.y);
			switch (curState)
			{
			case 0:
				return resC->GetSurf().windowsFrame[1].GetCurSurface().TestIfHitOnScreen((Vec2)mpRelBottom);
				break;
			case 1:
				return resC->GetSurf().windowsFrame[0].GetCurSurface().TestIfHitOnScreen((Vec2)mpRelTop) || resC->GetSurf().windowsFrame[2].GetCurSurface().TestIfHitOnScreen((Vec2)mpRelBottom);
				break;
			}
			break;
		case 1:
			scaleHead = Vec2(s->GetSize()) / Vec2(pos.GetSize());
			mpRelTop.x = (int)(mpRelTop.x * scaleHead.x);
			mpRelTop.y = (int)(mpRelTop.y * scaleHead.y);

			return s->TestIfHitOnScreen((Vec2)mpRelTop);
			break;
		case -1:
			return false;//resC->GetSurf().windowsFrame[1].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel);
			break;
		}
	}
	return false;
}
bool Frame::IsExtended() 
{ 
	assert(type == 0  || type == 1 || type == -1);
	switch (type)
	{
	case 0:
		return curState == 1;
		break;
	}
	return false;
}
int Frame::GetExtendedHeight()
{
	assert(type == 0 || type == 1 || type == -1);
	switch (type)
	{
	case 0:
		return ((int)pos.GetHeight() / 20) * 19;
		break;
	}
	return 0;
}
void Frame::Grab(Vec2 mP)
{
	Vec2 delta = mP - pos.GetTopLeft<float>();
	if (delta.y < pos.GetHeight() * Settings::percentForGrab && delta.y >= 0 && delta.x >= 0 && delta.x < pos.GetWidth())
	{
		grabbed = true;
		lastMouseP = mP;
	}
}
bool B1(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::Type::ButtonPressed, "priv page", -1, caller));
	//pF->PriviousPage();
	return true;
}
bool B2(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::Type::ButtonPressed, "next page", -1, caller));
	//pF->NextPage();
	return true;
}
bool BBuildMode(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::Type::ButtonPressed, "enable buildmode", caller->extra1, caller));
	//curW.SetBuildMode(0);
	return true;
}
bool BCraftingQueue(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::Type::ButtonPressed, "set crafting queue", caller->extra1, caller));
	//curW.SetBuildMode(0);
	return true;
}
bool BNextTurn(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::Type::ButtonPressed, "next turn", -1, caller));
	//curW.NextTurn();
	return true;
}
bool BLoadScene(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::Type::ButtonPressed, "load scene", caller->extra1, caller));
	//curW.LoadBuildMenu();
	return true;
}
bool BSetAttackMode(std::queue<FrameEvent>* buffer, Component* caller)
{
	buffer->push(FrameEvent(FrameEvent::Type::ButtonPressed, "set attackMode", -1, caller));
	return true;
}
bool BSetObstacleState(std::queue<FrameEvent>* buffer, Component* caller)
{
	if(caller->extraB1)
		buffer->push(FrameEvent(FrameEvent::Type::ButtonPressed, "set obstacle state "+ caller->extraS1, caller->extra1, caller));
	else
		buffer->push(FrameEvent(FrameEvent::Type::ButtonPressed, "set obstacle state "+ caller->extraS1, caller->extra2, caller));
	return true;
}
void Frame::Move(Vec2 mP)
{
	Vec2 deltaMove = lastMouseP - mP;
	lastMouseP = mP;
	pos -= deltaMove;
}
void Frame::Release()
{
	grabbed = false;
}

std::vector<int> Component::FillWith1WhenSize0(std::vector<int> activInStates, int nStages)
{
	if (activInStates.size() == 0)
	{
		std::vector<int> activInStates(nStages, 1);
		return std::vector<int>(nStages, 1);
	}
	else
	{
		for (int i = 0; i < nStates - (int)activInStates.size(); i++)
		{
			activInStates.push_back(1);
		}
	}
	return activInStates;
}

//  ### Framehandle::PageFrame ###


//	### Framehandle::MultiFrame ###
Frame* MultiFrame::AddFrame(std::string key, RectF pos, int type, sharedResC resC, Component* parentC)
{
	frames[key] = std::make_unique<Frame>(pos,type,resC,parentC, buffer);
	return frames[key].get();
}
PageFrame* MultiFrame::AddPageFrame(std::string key, RectF pos, int type, sharedResC resC, Component* parentC, int nPages)
{
	frames[key] = std::make_unique<PageFrame>(pos, type, resC, parentC, nPages, buffer);
	return static_cast<PageFrame*>(frames[key].get());
}

FrameEvent FrameHandle::Read()
{
	if (buffer.size() > 0u)
	{
		FrameEvent e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return FrameEvent();
	}
}
Frame* FrameHandle::AddFrame(std::string key, RectF pos, int type)
{
	if (Settings::framesOn)
	{
		comps[key] = std::make_unique<Frame>(pos, type, resC, nullptr, &buffer);
		return static_cast<Frame*>(comps[key].get());
	}
	return nullptr;
}
PageFrame* FrameHandle::AddPageFrame(std::string key, RectF pos, int type, int nPages)
{
	if (Settings::framesOn)
	{
		comps[key] = std::make_unique<PageFrame>(pos, type, resC, nullptr, nPages, &buffer);
		return static_cast<PageFrame*>(comps[key].get());
	}
	return nullptr;
}
MultiFrame* FrameHandle::AddMultiFrame(std::string key, RectF pos, int type, int nStates)
{
	if (Settings::framesOn)
	{
		std::vector<int> activInStates;
		for (int i = 0; i < nStates; i++)
		{
			activInStates.push_back(1);
		}
		comps[key] = std::make_unique<MultiFrame>(pos, resC, nullptr, &buffer);
		return static_cast<MultiFrame*>(comps[key].get());
	}
	return nullptr;
}
//	### Framehandle ###
void FrameHandle::UpdateInventoryComps(Inventory* inv, Component* parentC)
{
	for (int i = 0; i < inv->GetSize(); i++)
	{
		std::string key = "gI_item" + std::to_string(i);
		if (inv->GetItem(i)->get() != nullptr)
		{
			parentC->GetComp(key)->SetVisible(true);
			static_cast<GrabImage*>(parentC->GetComp(key))->SetAnimationOfBouth(&resC->GetSurf().items[inv->GetItem(i)->get()->GetId()]);
			if (inv->GetItem(i)->get()->GetDurability() != -1 && inv->GetItem(i)->get()->GetDurability() != Settings::itemStats[inv->GetItem(i)->get()->GetId()].durability)
			{
				static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "HpIs"))->SetVisible(true);
				static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "Hp"))->SetVisible(true);
				float percentage = (float)inv->GetItem(i)->get()->GetDurability() / Settings::itemStats[inv->GetItem(i)->get()->GetId()].durability;
				static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "HpIs"))->SetDrawPercent(Vec2(percentage, 1.f));
			}
			else
			{
				static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "HpIs"))->SetVisible(false);
				static_cast<Image*>(parentC->GetComp(key)->GetComp(key + "Hp"))->SetVisible(false);
			}
		}
		else
		{
			parentC->GetComp(key)->SetVisible(false);
		}
	}
}
int FrameHandle::GetHitInventorySlot(std::string who, Vec2 mP)
{
	int nSlots = 0;
	Frame* frame = nullptr;
	if (who == "unit")
	{
		nSlots = 8;
		frame = static_cast<Frame*>(comps["fInventory"].get());
	}
	if (who == "box")
	{
		nSlots = 9;
		frame = static_cast<Frame*>(comps["fInventoryBox"].get());
	}
	if (who == "storage")
	{
		nSlots = 25;
		frame = static_cast<Frame*>(comps["fInventoryStorage"].get());
	}
	if (who == "wrought")
	{
		nSlots = 6;
		frame = static_cast<Frame*>(comps["fInventoryWrought"].get());
	}
	if (frame == nullptr)
	{
		return -1;
	}
	if (frame->IsVisible())
	{
		for (int i = 0; i < nSlots; i++)
		{
			std::string key = "gI_item" + std::to_string(i);
			if (static_cast<GrabImage*>(frame->GetComp(key))->GetPos().Contains(mP))
			{
				return frame->GetComp(key)->extra1;
			}
		}
	}
	return -1;
}
Frame* FrameHandle::CreateBuildOption(RectF pos, int obstacleType, PageFrame* parentC, std::vector<int> activOnPages, World* world)
{
	std::string key = Settings::GetObstacleString(obstacleType);
	Frame* frame = parentC->AddFramePF(pos, key, { 1 }, activOnPages);
	frame->s = &resC->GetSurf().windowsFrame[7].GetCurSurface();
	frame->extra1 = obstacleType;
	frame->bFunc = BBuildMode;
	frame->hitable = true;
	frame->AddText(Settings::GetObstacleString(obstacleType), RectF(Vec2(60, 5), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, key + "H", { 1 });
	std::map<std::string, float> neededRes = Settings::obstacleStats[obstacleType].neededResToBuild;
	std::map<std::string, float>::iterator res;
	int i = 0;
	for (res = neededRes.begin(); res != neededRes.end(); res++)
	{
		//std::map<st::string,float> check ={{}}
		if (world->GetPlayer()->GetMaterials().Has({ { res->first,res->second } }))
			frame->AddText("-" + res->first + ": x" + std::to_string(res->second) + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(70.f, 20.f + i * 10.f), 50.f, 8.f), 7, &resC->GetSurf().fonts[0], Colors::Green, key + "res" + std::to_string(i), { 1 });
		else
		{
			frame->AddText("-" + res->first + ": x" + std::to_string(res->second) + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(70.f, 20.f + i * 10.f), 50.f, 8.f), 7, &resC->GetSurf().fonts[0], Colors::Red, key + "res" + std::to_string(i), { 1 });
		}
		i++;
	}
	//Frame* frame = AddFrame(key, pos, 0);
	return nullptr;// static_cast<Frame*>(comps[key].get());
}
Frame* FrameHandle::CreateCraftOption(RectF pos, int itemType, Frame* parentC, std::vector<int> activOnPages, World* world)
{
	std::string key = Settings::GetItemString(itemType);
	//Frame* frame = parentC->AddFramePF(pos, key, { 1 }, activOnPages);
	Frame* frame = parentC->AddFrame(pos, 1, resC, parentC, &buffer, key, { 1 });
	frame->s = &resC->GetSurf().windowsFrame[7].GetCurSurface();
	frame->extra1 = itemType;
	frame->bFunc = BCraftingQueue;
	frame->hitable = true;
	frame->AddText(Settings::GetItemString(itemType), RectF(Vec2(60, 5), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, key + "H", { 1 });
	std::map<std::string, float> neededRes = Settings::itemStats[itemType].neededResToCraft;
	std::map<std::string, float>::iterator res;
	int i = 0;
	for (res = neededRes.begin(); res != neededRes.end(); res++)
	{
		//std::map<st::string,float> check ={{}}
		if (world->GetPlayer()->GetMaterials().Has({ { res->first,res->second } }))
			frame->AddText("-" + res->first + ": x" + std::to_string(res->second) + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(70.f, 20.f + i * 10.f), 50.f, 8.f), 7, &resC->GetSurf().fonts[0], Colors::Green, key + "res" + std::to_string(i), { 1 });
		else
		{
			frame->AddText("-" + res->first + ": x" + std::to_string(res->second) + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(70.f, 20.f + i * 10.f), 50.f, 8.f), 7, &resC->GetSurf().fonts[0], Colors::Red, key + "res" + std::to_string(i), { 1 });
		}
		i++;
	}
	//Frame* frame = AddFrame(key, pos, 0);
	return nullptr;// static_cast<Frame*>(comps[key].get());
}
GrabImage* FrameHandle::CreateGIWithHpBar(Component* parentC, RectF pos, const Animation* a, const Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates)
{
	GrabImage* gi = parentC->AddGrabImage(pos, a, aHover, buffer, key, activInStates);
	gi->SetVisible(false);

	Image* image = gi->AddImage(RectF(Vec2(0, pos.GetHeight() / 5 * 4), pos.GetWidth(), pos.GetHeight() / 5), &resC->GetSurf().frames[1], &resC->GetSurf().frames[1], buffer, key + "Hp");
	image = gi->AddImage(RectF(Vec2(0, pos.GetHeight() / 5 * 4), pos.GetWidth(), pos.GetHeight() / 5), &resC->GetSurf().frames[2], &resC->GetSurf().frames[2], buffer, key + "HpIs");
	return gi;
}
void FrameHandle::AddHeadline(Component* parentC, std::string text, const Font* f, Color c)
{
	parentC->AddText(text, (RectF)resC->GetFrameSize().GetFramePos("frameHeadline"), 14*resC->GetFrameSize().GetGuiScale(),f, Colors::Black, "tHeadline");

}
int FrameHandle::AddObstacleInfo(Component* parentC, int top, const Font* f, Color c, std::string key, std::string infoText)
{
	if (infoText == "###")
	{
		infoText = Settings::lang_noInformation[Settings::lang];
	}
	RectF parentRect = parentC->GetPos();
	int xStart = (int)((float)(parentRect.GetWidth()) / 40);
	int halfParentWidth = parentRect.GetWidth() / 2;
	int textSize = (int)((float)(14) * resC->GetFrameSize().GetGuiScale());
	std::string infoString = GetInfoString(key);
	//parentC->AddTextBox()
	TextBox* tb1 = parentC->AddTextBox(infoText, RectF(Vec2(halfParentWidth, top), halfParentWidth, 14), textSize, &resC->GetSurf().fonts[0], c, key+"Is", {0,1}, 1);
	TextBox* tb2 = parentC->AddTextBox(infoString, RectF(Vec2(xStart, top), halfParentWidth, 14), textSize, &resC->GetSurf().fonts[0], Colors::Black, key, { 0,1 }, 1);
	int nLines = tb1->GetLines().size();
	if (tb1->GetLines().size() > nLines)
	{
		nLines = tb1->GetLines().size();
	}
	return top + textSize*nLines + 10;
}
int FrameHandle::AddObstacleInfoTextBox(Component* parentC,std::string text, int top, const Font* f, Color c, std::string key)
{

	RectF parentRect = parentC->GetPos();
	int xStart = (int)((float)(parentRect.GetWidth()) / 40);
	int textSize = (int)((float)(14) * resC->GetFrameSize().GetGuiScale());
	TextBox* tB_townHall = parentC->AddTextBox(Settings::lang_TownhallInfo[Settings::lang], RectF(Vec2(xStart, top), parentRect.GetWidth() - xStart*2, 40), textSize, &resC->GetSurf().fonts[0], Colors::Black, "tB_townhallInfo", { 0,1 });
	return top + textSize * (tB_townHall->GetLines().size() + 2);
}
int FrameHandle::AddObstacleAttackButton(Component* parentC, int top, const Font* f, Color c)
{
	RectF parentRect = parentC->GetPos();
	int xStart = (int)((float)(parentRect.GetWidth()) / 40);
	int halfParentWidth = parentRect.GetWidth() / 2;
	int buttonBoxSize = (int)((float)(120) * resC->GetFrameSize().GetGuiScale());

	Button* b_setAttack = parentC->AddButton(RectF(Vec2(xStart, top), buttonBoxSize, buttonBoxSize / 2), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], "bSetAttack", &resC->GetSurf().fonts[0], { 0,1 });
	b_setAttack->bFunc = BSetAttackMode;
	b_setAttack->text = Settings::lang_attack[Settings::lang] + " (A)";
	b_setAttack->size = (int)((float)(14) * resC->GetFrameSize().GetGuiScale());
	b_setAttack->c = c;
	b_setAttack->hitable = true;
	return top + halfParentWidth / 2 + 10;
}
int FrameHandle::AddObstacleCheckBox(Component* parentC, int top, const Font* f, Color c,std::string text, std::string key)
{
	RectF parentRect = parentC->GetPos();
	int xStart = (int)((float)(parentRect.GetWidth()) / 40);
	int halfParentWidth = parentRect.GetWidth() / 2;
	int textSize = (int)((float)(14) * resC->GetFrameSize().GetGuiScale());
	int checkBoxSize = (int)((float)(15) * resC->GetFrameSize().GetGuiScale());

	parentC->AddText(text, RectF(Vec2(xStart, top), halfParentWidth, textSize), textSize, f, c, "t"+key, { 0,1 }, 1);

	CheckBox* cb = parentC->AddCheckBox(RectF(Vec2(halfParentWidth, top), checkBoxSize, checkBoxSize), &buffer, resC, "cB"+key, { 0,1 });
	if(key == "educate" || key == "heal" || key == "attack")
		cb->extraS1 = "townhall";
	if(key == "automaticChop")
		cb->extraS1 = "lumberjackHut";
	cb->SetBFunc(BSetObstacleState);
	if (key == "educate" || key == "automaticChop")
	{
		cb->extra1 = 0;
		cb->extra2 = 1;
	}
	if (key == "heal")
	{
		cb->extra1 = 2;
		cb->extra2 = 3;
	}
	if (key == "attack")
	{
		cb->extra1 = 4;
		cb->extra2 = 5;
	}
	return top + textSize + 10;
}
std::string FrameHandle::GetInfoString(std::string key)
{
	if (key == "tUnitName")
	{
		return Settings::lang_unitName[Settings::lang] + ":";
	}else if (key == "tUnitHp")
	{
		return Settings::lang_hp[Settings::lang] + ":";
	}
	else if(key == "tUnitTeam")
	{
		return Settings::lang_team[Settings::lang] + ":";
	}
	else if(key == "tUnitStepsLeft")
	{
		return Settings::lang_stepsLeft[Settings::lang] + ":";
	}
	else if (key == "tUnitAttacksLeft")
	{
		return Settings::lang_attacksLeft[Settings::lang] + ":";
	}
	return "ERROR: key not found:" + key;
}
bool FrameHandle::FrameEnabledForObstacle(Obstacle* obstacle, std::string key)
{
	if (key == "fUnit")
	{
		return Settings::anyOfCreature(obstacle->type);
	}
	if (key == "fTownhall")
	{
		return obstacle->type == 3;
	}
	if (key == "fLumberjackHut")
	{
		return obstacle->type == 27;
	}
	if (key == "fInventory")
	{
		return Settings::anyOfCreature(obstacle->type);
	}
	if (key == "fInventoryBox")
	{
		return obstacle->type == 6;
	}
	if (key == "fInventoryStorage")
	{
		return obstacle->type == 50;
	}
	if (key == "fInventoryWrought")
	{
		return obstacle->type == 30;
	}
	return false;
}
void FrameHandle::SetCheckboxDisabling(std::vector<CheckBox*> checkboxes)
{
	for (int i = 0; i < (int)checkboxes.size(); i++)
	{
		for (int n = 0; n < (int)checkboxes.size(); n++)
		{
			if (i != n)
			{
				checkboxes[i]->AddCheckBox2Disable(checkboxes[n]);
			}
		}
	}
}
void FrameHandle::LoadFrame(std::string key)
{
	if (key == "fUnit")
	{
		CreateFrameUnit();
	}
	else if (key == "fTownhall")
	{
		Frame* fTownhall = AddFrame("fTownhall", (RectF)(resC->GetFrameSize().GetFramePos("framePos")), 0);																			//Townhall Frame
		fTownhall->SetState(1);
		AddHeadline(fTownhall, Settings::lang_townhall[Settings::lang], &resC->GetSurf().fonts[0], Colors::Black);

		int start = fTownhall->GetPos().GetHeight() * Settings::percentForGrab + 5;

		start = AddObstacleInfoTextBox(fTownhall, Settings::lang_TownhallInfo[Settings::lang], start, &resC->GetSurf().fonts[0], Colors::Black, "tbObstacleInfo");

		start = AddObstacleInfo(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitHp");
		start = AddObstacleInfo(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitAttacksLeft");
		start = AddObstacleInfo(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitTeam");

		start = AddObstacleCheckBox(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black, Settings::lang_educate[Settings::lang]+":", "educate");
		start = AddObstacleCheckBox(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black, Settings::lang_heal[Settings::lang] + ":", "heal");
		start = AddObstacleCheckBox(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black, Settings::lang_recharge[Settings::lang] + ":", "attack");
		start = AddObstacleAttackButton(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black);

		SetCheckboxDisabling({ static_cast<CheckBox*>(comps["fTownhall"]->GetComp("cBeducate")),
							   static_cast<CheckBox*>(comps["fTownhall"]->GetComp("cBheal")),
							   static_cast<CheckBox*>(comps["fTownhall"]->GetComp("cBattack"))});
	}
	else if (key == "fLumberjackHut")
	{																		//Lumberjack Frame
		Frame* fLumberjackHut = AddFrame("fLumberjackHut", (RectF)(resC->GetFrameSize().GetFramePos("framePos")), 0);																			//Townhall Frame
		fLumberjackHut->SetState(1);

		AddHeadline(fLumberjackHut, Settings::lang_lumberjackHut[Settings::lang], &resC->GetSurf().fonts[0], Colors::Black);

		int start = fLumberjackHut->GetPos().GetHeight() * Settings::percentForGrab + 5;
		
		start = AddObstacleInfoTextBox(fLumberjackHut, Settings::lang_LumberjackHutInfo[Settings::lang], start, &resC->GetSurf().fonts[0], Colors::Black, "tbObstacleInfo");
		
		start = AddObstacleInfo(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitHp");
		start = AddObstacleInfo(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitAttacksLeft");
		start = AddObstacleInfo(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitTeam");

		start = AddObstacleCheckBox(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black, Settings::lang_automatic[Settings::lang] + ":", "automaticChop");
		start = AddObstacleAttackButton(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black);
		static_cast<Button*>(comps[key]->GetComp("bSetAttack"))->text = Settings::lang_chop[Settings::lang];
	}
	else if (key == "fInventory")
	{
		Frame* fInventory = AddFrame("fInventory", RectF(Vec2(Graphics::ScreenWidth / 2 - 125, Graphics::ScreenHeight / 12), 250, 190), 1);
		fInventory->s = &resC->GetSurf().windowsFrame[8].GetCurSurface();
		
		GrabImage* hand1 = CreateGIWithHpBar(fInventory, RectF(Vec2(70, 10), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item0", { 1,1 });
		hand1->extra1 = 0;
		hand1->extraS1 = "inventory swap unit";

		GrabImage* bonus2 = CreateGIWithHpBar(fInventory, RectF(Vec2(130, 10), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item1", { 1,1 });
		bonus2->extra1 = 1;
		bonus2->extraS1 = "inventory swap unit";

		GrabImage* armor = CreateGIWithHpBar(fInventory, RectF(Vec2(70, 70), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item2", { 1,1 });
		armor->SetVisible(false);
		armor->extra1 = 2;
		armor->extraS1 = "inventory swap unit";

		GrabImage* bonus1 = CreateGIWithHpBar(fInventory, RectF(Vec2(130, 70), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item3", { 1,1 });
		bonus1->SetVisible(false);
		bonus1->extra1 = 3;
		bonus1->extraS1 = "inventory swap unit";

		GrabImage* item1 = CreateGIWithHpBar(fInventory, RectF(Vec2(10, 130), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item4", { 1,1 });
		item1->SetVisible(false);
		item1->extra1 = 4;
		item1->extraS1 = "inventory swap unit";

		GrabImage* item2 = CreateGIWithHpBar(fInventory, RectF(Vec2(70, 130), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item5", { 1,1 });
		item2->SetVisible(false);
		item2->extra1 = 5;
		item2->extraS1 = "inventory swap unit";

		GrabImage* item3 = CreateGIWithHpBar(fInventory, RectF(Vec2(130, 130), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item6", { 1,1 });
		item3->SetVisible(false);
		item3->extra1 = 6;
		item3->extraS1 = "inventory swap unit";

		GrabImage* item4 = CreateGIWithHpBar(fInventory, RectF(Vec2(190, 130), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item7", { 1,1 });
		item4->SetVisible(false);
		item4->extra1 = 7;
		item4->extraS1 = "inventory swap unit";
	}
	else if (key == "fInventoryBox")
	{
		Frame* fInventoryBox = AddFrame("fInventoryBox", RectF(Vec2(Graphics::ScreenWidth / 8 * 5, Graphics::ScreenHeight / 12), 190, 190), 1);
		fInventoryBox->s = &resC->GetSurf().windowsFrame[9].GetCurSurface();

		for (int i = 0; i < 9; i++)
		{
			GrabImage* image = CreateGIWithHpBar(fInventoryBox, RectF(Vec2(10.f + (int)(i % 3) * 60.f, 10.f + (int)(i / 3) * 60.f), 50.f, 50.f), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item" + std::to_string(i), { 1,1 });
			image->SetVisible(false);
			image->extra1 = i;
			image->extraS1 = "inventory swap box";
		}
	}
	else if (key == "fInventoryStorage")
	{
		Frame* fInventoryStorage = AddFrame("fInventoryStorage", RectF(Vec2(Graphics::ScreenWidth / 8 * 5, Graphics::ScreenHeight / 12), 310, 310), 1);
		fInventoryStorage->s = &resC->GetSurf().windowsFrame[10].GetCurSurface();

		for (int i = 0; i < 25; i++)
		{
			GrabImage* image = CreateGIWithHpBar(fInventoryStorage, RectF(Vec2(10.f + (int)(i % 5) * 60, 10.f + (int)(i / 5) * 60), 50.f, 50.f), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item" + std::to_string(i), { 1,1 });
			image->SetVisible(false);
			image->extra1 = i;
			image->extraS1 = "inventory swap storage";
		}
	}
	else if (key == "fInventoryWrought")
	{
		Frame* fInventoryWrought = AddFrame("fInventoryWrought", RectF(Vec2(Graphics::ScreenWidth / 8 * 5, Graphics::ScreenHeight / 12), 190, 190), 1);
		fInventoryWrought->s = &resC->GetSurf().windowsFrame[11].GetCurSurface();
		Text* text;
		text = fInventoryWrought->AddText(Settings::lang_craftedItem[Settings::lang] + ":", RectF(Vec2(10, 10), 50, 8), 14, &resC->GetSurf().fonts[0], Colors::Black, "t_craft", { 1,1 }, 1);
		text->SetVisible(true);
		text = fInventoryWrought->AddText(Settings::lang_status[Settings::lang] + ":", RectF(Vec2(10, 28), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_craftStatus", { 1,1 }, 1);

		text = fInventoryWrought->AddText(Settings::lang_reparation[Settings::lang] + ":", RectF(Vec2(10, 110), 50, 8), 14, &resC->GetSurf().fonts[0], Colors::Black, "t_rep", { 1,1 }, 1);
		text->SetVisible(true);

		for (int i = 0; i < 6; i++)
		{
			GrabImage* image = CreateGIWithHpBar(fInventoryWrought, RectF(Vec2(10.f + (int)(i % 3) * 60.f, 40.f + (int)(i / 3) * 90.f), 50.f, 50.f), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item" + std::to_string(i), { 1,1 });
			image->SetVisible(false);
			image->extra1 = i;
			image->extraS1 = "inventory swap wrought";
		}
	}
}
void FrameHandle::UpdateFrame(Obstacle* obstacle, std::string key)
{
	if (obstacle != nullptr)
	{
		if (comps[key]->GetComp("tUnitNameIs")!= nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitNameIs"))->Settext(Settings::GetObstacleString(obstacle->type));
		}
		if (comps[key]->GetComp("tUnitHpIs") != nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitHpIs"))->Settext(std::to_string(obstacle->hp) + " / " + std::to_string(Settings::obstacleStats[obstacle->type].baseHp));
		}
		if (comps[key]->GetComp("tUnitTeamIs") != nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitTeamIs"))->Settext(obstacle->team->GetTeamName());
		}
		if (comps[key]->GetComp("tUnitStepsLeftIs") != nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitStepsLeftIs"))->Settext(std::to_string(obstacle->stepsLeft));
		}
		if (comps[key]->GetComp("tUnitAttacksLeftIs") != nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitAttacksLeftIs"))->Settext(std::to_string(obstacle->attack->GetAttacksLeft()));
		}
		if (key == "fUnit")
		{
			std::map<std::string, std::string> map2Parse;
			auto compsOfOld = comps[key]->GetComps();
			//const std::map<std::string, std::unique_ptr<Component>>::iterator i;
			for (auto i = compsOfOld->begin(); i != compsOfOld->end(); i++)
			{
				if (const TextBox* tb = dynamic_cast<const TextBox*>(i->second.get()))
				{
					map2Parse[i->first] = tb->GetText();
				}
			}
			DeleteFrame(key);
			CreateFrameUnit(&map2Parse);
		}
		if (key == "fTownhall")
		{
			Frame* f_townhall = static_cast<Frame*>(comps[key].get());
			if (obstacle->education->Educates())
				static_cast<CheckBox*>(f_townhall->GetComp("cBeducate"))->Check();
			else
				static_cast<CheckBox*>(f_townhall->GetComp("cBeducate"))->Uncheck();
			if (obstacle->heal->Isenabled())
				static_cast<CheckBox*>(f_townhall->GetComp("cBheal"))->Check();
			else
				static_cast<CheckBox*>(f_townhall->GetComp("cBheal"))->Uncheck();
			if (obstacle->attack->GetReloadNextTurn())
				static_cast<CheckBox*>(f_townhall->GetComp("cBattack"))->Check();
			else
				static_cast<CheckBox*>(f_townhall->GetComp("cBattack"))->Uncheck();
		}
		if (key == "fLumberjackHut")
		{
			Frame* fLumberjackHut = static_cast<Frame*>(comps[key].get());
			if (obstacle->attack->GetAutomaticMode() == CtPos(Vei2(-1, -1), Vei2(-1, -1)))
				static_cast<CheckBox*>(fLumberjackHut->GetComp("cBautomaticChop"))->Uncheck();
			else
				static_cast<CheckBox*>(fLumberjackHut->GetComp("cBautomaticChop"))->Check();
		}
		if (key.find("Inventory") != std::string::npos && FrameEnabledForObstacle(obstacle,key))		//handles all needed Inventoryupdates
		{
			UpdateInventoryComps(obstacle->inv.get(), comps[key].get());
		}
		if (key == "fInventoryWrought")
		{
			Frame* fInventoryWrought = static_cast<Frame*>(comps[key].get());
			if (obstacle->craft->IsCrafting())
			{
				fInventoryWrought->GetComp("t_craftStatus")->text = Settings::lang_status[Settings::lang] + ": " + Settings::GetItemString(obstacle->craft->GetItemID()) + " " + Settings::lang_finishedIn[Settings::lang] + " " + std::to_string(obstacle->craft->TurnsLeft(obstacle->productivity)) + " " + Settings::lang_rounds[Settings::lang];
				fInventoryWrought->GetComp("t_craftStatus")->c = Colors::Green;
			}
			else if (obstacle->craft->GetItemID() != -1 && !obstacle->team->GetMaterials().Has(Settings::itemStats[obstacle->craft->GetItemID()].neededResToCraft))
			{
				fInventoryWrought->GetComp("t_craftStatus")->text = Settings::lang_status[Settings::lang] + ": " + Settings::lang_ressourcesAreMissing[Settings::lang];
				fInventoryWrought->GetComp("t_craftStatus")->c = Colors::Red;
			}
			else
			{
				fInventoryWrought->GetComp("t_craftStatus")->text = Settings::lang_status[Settings::lang] + ": " + Settings::lang_nothingSelected[Settings::lang];
				fInventoryWrought->GetComp("t_craftStatus")->c = Colors::Red;
			}
			comps["b_wroughScene"]->SetVisible(true);
		}
	}
}
void FrameHandle::DeleteFrame(std::string key)
{
	if (key == "fInventoryWrought")
	{
		comps["b_wroughScene"]->SetVisible(false);
	}
	comps.erase(key);
}
void FrameHandle::CreateFrameUnit(const std::map<std::string,std::string>* parsedText)
{
	std::vector<int> a = { 0,1 };
	Frame* fUnity = AddFrame("fUnit", (RectF)resC->GetFrameSize().GetFramePos("framePos"), 0);
	fUnity->SetState(1);																		//Unit Frame

	AddHeadline(fUnity, Settings::lang_unitInfo[Settings::lang], &resC->GetSurf().fonts[0], Colors::Black);

	int start = fUnity->GetPos().GetHeight() * Settings::percentForGrab + 5;
	if (parsedText == nullptr)
	{
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitName");
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitHp");
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitTeam");
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitStepsLeft");
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitAttacksLeft");
		start = AddObstacleAttackButton(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black);
	}
	else
	{
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitName", parsedText->find("tUnitNameIs")->second);
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitHp", parsedText->find("tUnitHpIs")->second	);
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitTeam", parsedText->find("tUnitTeamIs")->second);
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitStepsLeft", parsedText->find("tUnitStepsLeftIs")->second);
		start = AddObstacleInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitAttacksLeft", parsedText->find("tUnitAttacksLeftIs")->second);
		start = AddObstacleAttackButton(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black);
	}
}
FrameHandle::FrameHandle(sharedResC resC)
	:resC(std::move(resC))
{
	UpdateHandleOrder();
}

bool FrameHandle::HandleMouseInput(Mouse::Event& e)
{

	if (Settings::framesOn)
	{
		for (int prio = 0; prio <= 10; prio++)
		{
			std::map<std::string, std::unique_ptr<Component>>::iterator i;
			for (i = comps.begin(); i != comps.end(); i++)
			{
				/*
				if (e.GetType() == Mouse::Event::Type::LRelease && i->first == "f_Unit" && i->second->GetPrio() == prio)
				{
					int k = 23;
				}
				*/
				if (i->second->GetPrio() == prio && i->second->HandleMouseInput(e, true))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void FrameHandle::Draw(Graphics& gfx)
{
	if (Settings::framesOn)
	{
		UpdateHandleOrder();
		for (int i = 0; i < handleOrder.size(); i++)
		{
			handleOrder[i]->Draw(gfx);
		}
	}
}
void FrameHandle::UpdateHandleOrder()
{
	handleOrder.clear();
	std::map<std::string, std::unique_ptr<Component>>::iterator i;
	for (i = comps.begin(); i != comps.end(); i++)
	{
		if(i->second->IsVisible())
			handleOrder.push_back(i->second.get());
	}
	std::sort(handleOrder.begin(), handleOrder.end(), [&](Component* obst1, Component* obst2)
	{
		return obst1->GetPrio() > obst2->GetPrio();
	});
}

void FrameHandle::LoadScene(int scene, World* world)
{
	if (Settings::framesOn)
	{
		comps.clear();
		curScene = scene;
		if (scene == 0)
		{
			using namespace Settings;
			std::vector<int> a = { 0,1 };

			MultiFrame* m = AddMultiFrame("f_resD", RectF(Vec2(1040, 110), 140, 280), 0, 1);

			Frame* f1 = m->AddFrame("fresD_f1", RectF(Vec2(0, 0), 140, 280), 0, resC, m);
			PageFrame* p3 = m->AddPageFrame("fresD_f2", RectF(Vec2(0, 12), 140, 280), 0, resC, m, 3);

			// #1
			f1->AddText(Settings::lang_fieldInformation[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, "h_f1");

			f1->AddText(Settings::lang_noInformation[Settings::lang], RectF(Vec2(60, 19), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_cellType", a, 1);
			f1->AddText(Settings::lang_flora[Settings::lang] + ":", RectF(Vec2(2, 19), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_flora", a, 1);
			f1->AddText(Settings::lang_Obstacle[Settings::lang] + ":", RectF(Vec2(2, 35), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_obstacle", a, 1);
			f1->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(60, 35), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_obstacleInfo", a, 1);
			f1->AddText(Settings::lang_hp[Settings::lang] + ":", RectF(Vec2(2, 46), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_obstacleHp", a, 1);
			f1->AddText(Settings::lang_noInformation[Settings::lang] + ":", RectF(Vec2(60, 46), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_obstacleHpIs", a, 1);

			// #2
			// #3
			// Page 1
			p3->AddTextPF(Settings::lang_constructionMaterials[Settings::lang], RectF(Vec2(46, 2), 50, 8), 7, &resC->GetSurf().fonts[0], Colors::Black, "h_f3");

			p3->AddTextPF(Settings::lang_resources[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "sh_resourcen", { 0,1 }, { 1, 0, 0 });
			p3->AddTextPF(Settings::lang_materials[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "sh_materials", { 0,1 }, { 0, 1, 0 });
			p3->AddTextPF(Settings::lang_organic[Settings::lang], RectF(Vec2(44, 16), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "sh_organic", { 0,1 }, { 0, 0, 1 });

			p3->AddTextPF(Settings::lang_wood[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_wood", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_iron[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_iron", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_sand[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_sand", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_stone[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_stone", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_copper[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_copper", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_gold[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_gold", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_aluminum[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_aluminum", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_emerald[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_emerald", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_sapphire[Settings::lang] + ":", RectF(Vec2(5, 130), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_sapphire", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_robin[Settings::lang] + ":", RectF(Vec2(5, 140), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_robin", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_diamond[Settings::lang] + ":", RectF(Vec2(5, 150), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_dimond", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF(Settings::lang_amber[Settings::lang] + ":", RectF(Vec2(5, 160), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_amber", { 0,1 }, { 1, 0, 0 }, 1);

			p3->AddTextPF(Settings::lang_kilogram[Settings::lang], RectF(Vec2(100, 40), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_kilogram", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("11", RectF(Vec2(100, 50), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nWood", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 60), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nIron", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 70), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nSand", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("44", RectF(Vec2(100, 80), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nStone", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 90), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nCopper", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 100), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nGold", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("3", RectF(Vec2(100, 110), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nAluminum", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("4", RectF(Vec2(100, 120), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nEmerald", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 130), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nSapphire", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("4", RectF(Vec2(100, 140), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nRobin", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 150), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nDimond", { 0,1 }, { 1, 0, 0 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 160), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nAmber", { 0,1 }, { 1, 0, 0 }, 1);

			//Page 2
			p3->AddTextPF(Settings::lang_steel[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_steel", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_plastic[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_plastic", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_concrete[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_concrete", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_glass[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_glass", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_ceramics[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_ceramics", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_snow[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_snow", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_bricks[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_bricks", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF(Settings::lang_slate[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_slate", { 0,1 }, { 0, 1, 0 }, 1);

			p3->AddTextPF("11", RectF(Vec2(100, 50), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nSteel", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 60), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nPlastic", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 70), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nConcrete", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("44", RectF(Vec2(100, 80), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nGlass", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 90), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nCeramics", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 100), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nSnow", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 110), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nBricks", { 0,1 }, { 0, 1, 0 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 120), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nSlate", { 0,1 }, { 0, 1, 0 }, 1);

			//Page 3
			p3->AddTextPF(Settings::lang_corals[Settings::lang] + ":", RectF(Vec2(5, 50), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_corals", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_sticks[Settings::lang] + ":", RectF(Vec2(5, 60), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_sticks", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_leaves[Settings::lang] + ":", RectF(Vec2(5, 70), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_leaves", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_wool[Settings::lang] + ":", RectF(Vec2(5, 80), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_wool", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_leather[Settings::lang] + ":", RectF(Vec2(5, 90), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_leather", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_fur[Settings::lang] + ":", RectF(Vec2(5, 100), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_fur", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_meat[Settings::lang] + ":", RectF(Vec2(5, 110), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_meat", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_fish[Settings::lang] + ":", RectF(Vec2(5, 120), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_fish", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_berrys[Settings::lang] + ":", RectF(Vec2(5, 130), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_berrys", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_apples[Settings::lang] + ":", RectF(Vec2(5, 140), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_apples", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_cactus[Settings::lang] + ":", RectF(Vec2(5, 150), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_cactus", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_units[Settings::lang] + ":", RectF(Vec2(5, 160), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_units", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF(Settings::lang_maxUnits[Settings::lang] + ":", RectF(Vec2(5, 170), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_maxUnits", { 0,1 }, { 0, 0, 1 }, 1);

			p3->AddTextPF("11", RectF(Vec2(100, 50), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nCorals", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 60), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nSticks", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 70), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nLeaves", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("44", RectF(Vec2(100, 80), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nWool", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 90), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nLeather", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 100), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nFur", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("3", RectF(Vec2(100, 110), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nMeat", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("4", RectF(Vec2(100, 120), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nFish", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("2", RectF(Vec2(100, 130), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nBerrys", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("4", RectF(Vec2(100, 140), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nApples", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 150), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nCactus", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 160), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nUnits", { 0,1 }, { 0, 0, 1 }, 1);
			p3->AddTextPF("24", RectF(Vec2(100, 170), 50, 10), 7, &resC->GetSurf().fonts[0], Colors::Black, "t_nMaxUnits", { 0,1 }, { 0, 0, 1 }, 1);


			comps["b_NextTurn"] = std::make_unique<Button>(Button(RectF(Vec2(1120, 600), 120, 60), &resC->GetSurf().windowsFrame[3], &resC->GetSurf().windowsFrame[3], { 0,0 }, &resC->GetSurf().fonts[0], nullptr, &buffer));
			//comps["b_NextTurn"] = std::make_unique<Button>(Button(RectF(Vec2(650, 450), 120, 60), &resC->GetSurf().windowsFrame[3], &resC->GetSurf().windowsFrame[3], { 0,0 }, &resC->GetSurf().fonts[0], nullptr, &buffer));
			static_cast<Button*>(comps["b_NextTurn"].get())->bFunc = BNextTurn;

			comps["b_buildScene"] = std::make_unique<Button>(Button(RectF(Vec2(30, 60), 120, 60), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], { 0,0 }, &resC->GetSurf().fonts[0], nullptr, &buffer));
			Button* button = static_cast<Button*>(comps["b_buildScene"].get());
			button->extra1 = 1;
			button->size = 14;
			button->bFunc = BLoadScene;
			button->text = Settings::lang_build[Settings::lang];

			comps["b_wroughScene"] = std::make_unique<Button>(Button(RectF(Vec2(30, 100), 60, 30), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], { 0,0 }, &resC->GetSurf().fonts[0], nullptr, &buffer));
			button = static_cast<Button*>(comps["b_wroughScene"].get());
			button->extra1 = 2;
			button->size = 14;
			button->SetVisible(false);
			button->bFunc = BLoadScene;
			button->text = Settings::lang_forge[Settings::lang];
			/*
			Frame* fButtonBuild = AddFrame(RectF(Vec2(30, 30), 60, 30), 1);																			//BUILD MENU BUTTON
			fButtonBuild->s = &resC->GetSurf().windowsFrame[4].GetCurSurface();
			fButtonBuild->bFunc = BBuildMenu;
			*/
			
		}
		else if (scene == 1)
		{
			std::vector<int> a = { 0,1 };
			PageFrame* fBuildSelection = AddPageFrame("f_bg", RectF(Vec2(0, 0), Graphics::ScreenWidth, Graphics::ScreenHeight), 1, 4);															//build selection menu
			fBuildSelection->s = &resC->GetSurf().windowsFrame[5].GetCurSurface();
			fBuildSelection->SetMoveable(false);
			fBuildSelection->GetComp("b_left")->pos = RectF(Vec2(25, 25), 100, 25);
			fBuildSelection->GetComp("b_right")->pos = RectF(Vec2(Graphics::ScreenWidth - 125, 25), 100, 25);
			fBuildSelection->AddTextPF(Settings::lang_housing[Settings::lang], RectF(Vec2(470, 30), 300, 60), 50, &resC->GetSurf().fonts[0], Colors::Black, "HousingH", { 1 }, { 1,0,0,0 });

			Button* b_back = fBuildSelection->AddButtonPF(RectF(Vec2(30, 60), 60, 30), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], "b_buildback", &resC->GetSurf().fonts[0], { 1 }, { 1,1,1,1 });
			b_back->extra1 = 0;
			b_back->bFunc = BLoadScene;
			b_back->text = Settings::lang_back[Settings::lang];

			CreateBuildOption(RectF(Vec2(60, 120), 180, 60), 0, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 190), 180, 60), 2, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 260), 180, 60), 3, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 330), 180, 60), 21, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 400), 180, 60), 22, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 470), 180, 60), 23, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 540), 180, 60), 24, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 610), 180, 60), 25, fBuildSelection, { 1,0,0,0 }, world);

			CreateBuildOption(RectF(Vec2(250, 190), 180, 60), 26, fBuildSelection, { 1,0,0,0 }, world);

			fBuildSelection->AddTextPF(Settings::lang_productions[Settings::lang], RectF(Vec2(470, 30), 300, 60), 50, &resC->GetSurf().fonts[0], Colors::Black, "roductionH", { 1 }, { 0,1,0,0 });

			CreateBuildOption(RectF(Vec2(60, 120), 180, 60), 27, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 190), 180, 60), 28, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 260), 180, 60), 29, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 330), 180, 60), 30, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 400), 180, 60), 31, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 470), 180, 60), 32, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 540), 180, 60), 33, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(60, 610), 180, 60), 34, fBuildSelection, { 0,1,0,0 }, world);

			CreateBuildOption(RectF(Vec2(250, 120), 180, 60), 35, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 190), 180, 60), 36, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 260), 180, 60), 37, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 330), 180, 60), 38, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 400), 180, 60), 39, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 470), 180, 60), 40, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 540), 180, 60), 41, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(250, 610), 180, 60), 42, fBuildSelection, { 0,1,0,0 }, world);

			CreateBuildOption(RectF(Vec2(440, 120), 180, 60), 48, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(440, 190), 180, 60), 49, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(440, 260), 180, 60), 50, fBuildSelection, { 0,1,0,0 }, world);

		}
		else if (scene == 2)
		{
			std::vector<int> a = { 0,1 };
			Frame* fCraftSelection = AddFrame("f_craftMenu", RectF(Vec2(0, 0), Graphics::ScreenWidth, Graphics::ScreenHeight), 1);//AddPageFrame("f_bg", RectF(Vec2(0, 0), Graphics::ScreenWidth, Graphics::ScreenHeight), 1, 4);															//build selection menu
			fCraftSelection->s = &resC->GetSurf().windowsFrame[5].GetCurSurface();
			fCraftSelection->SetMoveable(false);
			fCraftSelection->AddText(Settings::lang_forge[Settings::lang], RectF(Vec2(470, 30), 300, 60), 50, &resC->GetSurf().fonts[0], Colors::Black, "ForgeH", { 1 });

			Button* b_back = fCraftSelection->AddButton(RectF(Vec2(30, 100), 60, 30), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], "b_buildback", &resC->GetSurf().fonts[0], { 1 });
			b_back->extra1 = 0;
			b_back->bFunc = BLoadScene;
			b_back->text = Settings::lang_back[Settings::lang];

			CreateCraftOption(RectF(Vec2(60, 190), 180, 60), 0, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 260), 180, 60), 1, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 330), 180, 60), 2, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 400), 180, 60), 9, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 470), 180, 60), 10, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(60, 540), 180, 60), 11, fCraftSelection, { 1,0,0,0 }, world);
		}
		UpdateHandleOrder();
	}
}
void FrameHandle::UpdateFieldinformation(World& curW, Team* player)
{
	if (Settings::framesOn)
	{

		MultiFrame* m = static_cast<MultiFrame*>(comps["f_resD"].get());
		Materials& playerM = player->GetMaterials();
		//#1
		Frame* f1 = static_cast<Frame*>(m->GetFrame("fresD_f1"));
		f1->SetText(Settings::GetTypeString(curW.GetfCellType()), "t_cellType");
		if (curW.GetFocusedObstacle() != nullptr)
		{
			f1->SetText(Settings::GetObstacleString(curW.GetFocusedObstacle()->type), "t_obstacleInfo");
			f1->SetText(std::to_string(curW.GetFocusedObstacle()->hp) + " / " + std::to_string(Settings::obstacleStats[curW.GetFocusedObstacle()->type].baseHp), "t_obstacleHpIs");
		}

		//#2

		//#3
		PageFrame* p3 = static_cast<PageFrame*>(m->GetFrame("fresD_f2"));

		// Page 1
		p3->SetText(std::to_string(playerM.values["wood"]), "t_nWood");
		p3->SetText(std::to_string(playerM.values["iron"]), "t_nIron");
		p3->SetText(std::to_string(playerM.values["sand"]), "t_nSand");
		p3->SetText(std::to_string(playerM.values["stone"]), "t_nStone");
		p3->SetText(std::to_string(playerM.values["copper"]), "t_nCopper");
		p3->SetText(std::to_string(playerM.values["gold"]), "t_nGold");
		p3->SetText(std::to_string(playerM.values["aluminum"]), "t_nAluminum");
		p3->SetText(std::to_string(playerM.values["emerald"]), "t_nEmerald");
		p3->SetText(std::to_string(playerM.values["sapphire"]), "t_nSapphire");
		p3->SetText(std::to_string(playerM.values["robin"]), "t_nRobin");
		p3->SetText(std::to_string(playerM.values["diamond"]), "t_nDimond");
		p3->SetText(std::to_string(playerM.values["amber"]), "t_nAmber");

		//Page 2
		p3->SetText(std::to_string(playerM.values["steel"]), "t_nSteel");
		p3->SetText(std::to_string(playerM.values["plastic"]), "t_nPlastic");
		p3->SetText(std::to_string(playerM.values["concrete"]), "t_nConcrete");
		p3->SetText(std::to_string(playerM.values["glass"]), "t_nGlass");
		p3->SetText(std::to_string(playerM.values["ceramics"]), "t_nCeramics");
		p3->SetText(std::to_string(playerM.values["snow"]), "t_nSnow");
		p3->SetText(std::to_string(playerM.values["bricks"]), "t_nBricks");
		p3->SetText(std::to_string(playerM.values["slate"]), "t_nSlate");

		//Page 3
		p3->SetText(std::to_string(playerM.values["corals"]), "t_nCorals");
		p3->SetText(std::to_string(playerM.values["sticks"]), "t_nSticks");
		p3->SetText(std::to_string(playerM.values["leafes"]), "t_nLeaves");
		p3->SetText(std::to_string(playerM.values["wool"]), "t_nWool");
		p3->SetText(std::to_string(playerM.values["leather"]), "t_nLeather");
		p3->SetText(std::to_string(playerM.values["fur"]), "t_nFur");
		p3->SetText(std::to_string(playerM.values["meat"]), "t_nMeat");
		p3->SetText(std::to_string(playerM.values["fish"]), "t_nFish");
		p3->SetText(std::to_string(playerM.values["berrys"]), "t_nBerrys");
		p3->SetText(std::to_string(playerM.values["apples"]), "t_nApples");
		p3->SetText(std::to_string(playerM.values["cactus"]), "t_nCactus");
		p3->SetText(std::to_string(playerM.values["units"]), "t_nUnits");
		p3->SetText(std::to_string(playerM.values["maxUnits"]), "t_nMaxUnits");

	}
}
void FrameHandle::UpdateFrames(Obstacle* obst, Obstacle* storage)
{
	if (Settings::framesOn)
	{
		for (int i = 0; i < (int)Settings::frameKeys.size(); i++)
		{
			std::string key = Settings::frameKeys[i];
			if (comps.count(key) > 0)
			{
				if (obst != nullptr && FrameEnabledForObstacle(obst, key))
				{
					UpdateFrame(obst, key);
				}
				else if (storage != nullptr && FrameEnabledForObstacle(storage, key))
				{
					UpdateFrame(storage, key);
				}
				else
				{
					DeleteFrame(key);
				}
			}
			else if (obst != nullptr && FrameEnabledForObstacle(obst,key))
			{
				LoadFrame(key);
				UpdateFrame(obst, key);
			}
			else if (storage != nullptr && FrameEnabledForObstacle(storage,key))
			{
				LoadFrame(key);
				UpdateFrame(storage, key);
			}
		}
	}
}

