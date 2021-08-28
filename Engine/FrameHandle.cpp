#pragma once
#include "FrameHandle.h"
//		### Componente ###
Text* Frame::AddText(std::string text, RectF pos, int size,const Font* f, Color c, std::string key, std::vector<int> activInStates, int textLoc)
{
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<Text>(text, pos, size, f, c, activInStates, this, textLoc, buffer);
	return static_cast<Text*>(comps[key].get());
}
TextBox* Frame::AddTextBox(std::string text, RectF pos, int size, const Font* f, Color c, std::string key, std::vector<int> activInStates, int textLoc)
{
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<TextBox>(text, pos, size, f, c, activInStates, this, textLoc, buffer);
	return static_cast<TextBox*>(comps[key].get());
}
Button* Frame::AddButton(RectF pos,const Animation* a,const Animation* aHover, std::string key,const Font* f, std::vector<int> activInStates)
{
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<Button>(pos, a, aHover, activInStates, f, this, buffer);
	return static_cast<Button*>(comps[key].get());
}
CheckBox* Frame::AddCheckBox(RectF pos, std::queue<FrameEvent>* buffer, sharedResC resC, std::string key, std::vector<int> activInStates)
{
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<CheckBox>(pos, this, buffer, resC, activInStates);
	return static_cast<CheckBox*>(comps[key].get());
}
Image* Frame::AddImage(RectF pos, const Animation* a, const Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates)
{
	//RectF pos, Animation* a, Animation* aHover, Component* parentC, std::queue<FrameEvent>* buffer, std::vector<int> activInStates
	activInStates = FillWith1WhenSize0(activInStates, nStates);
	//assert(activInStates.size() == nStates);
	comps[key] = std::make_unique<Image>(pos, a, aHover, this, buffer, activInStates);
	return static_cast<Image*>(comps[key].get());
}
Frame* Frame::AddFrame(RectF pos, int type, sharedResC resC, Component* parentC, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates)
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
	float charDist = font->GetCharDist();
	lines.push_back("");
	bool firstWorldInLine = true;

	for (int i = 0; i < (int)text.length(); i++)
	{
		if (text[i] >= first && text[i] <= last)
		{
			wordWidthInChar++;
			float ratio = (float)cRects[(__int64)text[i] - first].GetWidth() / cRects[(__int64)text[i] - first].GetHeight();
			Vec2 charSize(ratio * size, (float)size);
			if (wordWidthInPixel + ((float)size * charDist * ratio) >= width && !firstWorldInLine)
			{
				lines.push_back("");
				wordWidthInPixel = 0;
				firstWorldInLine = true;
			}
			wordWidthInPixel += ((float)size * charDist * ratio);
		}
		if (text[i] == ' ')
		{
			firstWorldInLine = false;
			wordWidthInChar++;
			lines[(int)lines.size() - 1] += text.substr(charStart, wordWidthInChar);
			charStart += wordWidthInChar;
			wordWidthInChar = 0;
			if (wordWidthInPixel + (size * 0.7f) >= width)
			{
				lines.push_back("");
				wordWidthInPixel = 0;
				firstWorldInLine = true;
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
	Image(pos,nullptr,nullptr, parentC, buffer, {}),
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

Hint::Hint(Hint& hint)
{
	*this = hint;
}
Hint& Hint::operator=(const Hint& hint)
{
	newCompAtY = hint.newCompAtY;
	resC = hint.resC;
	lastMP = hint.lastMP;
	visible = hint.visible;
	size = hint.size;
	xStart = hint.xStart;

	for (int i = 0; i < hint.comps.size(); i++)
	{
		comps.push_back(hint.comps[i]->Clone());
	}
	return *this;
}
void Hint::AddHintTextBox(std::string text,const Font* font, Color c)
{
	int textSize = (int)((float)(11) * resC->GetFrameSize().GetGuiScale());
	int maxWidth = Settings::hintCellSize * Settings::hintMaxWidth;
	comps.push_back(std::make_unique<TextBox>(text, RectF(Vec2(xStart, newCompAtY), size.x * 25, maxWidth), textSize, font, c, std::vector<int>(), nullptr, 0, nullptr));
	TextBox* tB = static_cast<TextBox*>(comps[comps.size() - 1].get());
	newCompAtY += textSize * (tB)->GetLines().size() + 2;
	int lineLength = tB->GetLineLength(0);
	if (size.x * Settings::hintCellSize < lineLength)
	{
		size.x = lineLength / Settings::hintCellSize;
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
	std::map<std::string, std::unique_ptr<ComponentWithoutHint>>::iterator it = comps.find(key);
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
		Vec2 mpRel = (Vec2)(mP - GetPos().GetTopLeft<float>());
		Vec2 scale;
		switch (type)
		{
		case 0:
			Vec2 scale = Vec2(resC->GetSurf().windowsFrame[0].GetSize()) / Vec2(pos.GetSize());
			mpRel *= scale;
			switch (curState)
			{
			case 0:
				return resC->GetSurf().windowsFrame[1].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel);
				break;
			case 1:
				return resC->GetSurf().windowsFrame[0].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel) || resC->GetSurf().windowsFrame[2].GetCurSurface().TestIfHitOnScreen((Vec2)mpRel);
				break;
			}
			break;
		case 1:
			if (s != nullptr)
			{
				scale = Vec2(s->GetSize()) / Vec2(pos.GetSize());
				mpRel *= scale;

				return s->TestIfHitOnScreen((Vec2)mpRel);
			}
			return GetPos().Contains(mP);
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
ComponentWithoutHint::ComponentWithoutHint(const ComponentWithoutHint& compWH)
{
	*this = compWH;
}
ComponentWithoutHint& ComponentWithoutHint::operator=(const ComponentWithoutHint& compWH)
{
	pos = compWH.pos;
	curState = compWH.curState;
	extra1 = compWH.extra1;
	extra2 = compWH.extra2;
	nStates = compWH.nStates;
	extraB1 = compWH.extraB1;
	extraS1 = compWH.extraS1;
	c = compWH.c;
	activInStates = compWH.activInStates;
	text = compWH.text;
	mouseHovers = compWH.mouseHovers;
	hitable = compWH.hitable;
	visible = compWH.visible;
	prio = compWH.prio;
	buffer = compWH.buffer;
	parentC = compWH.parentC;
	//comps = compWH.comps;
	return *this;
}


std::vector<int> ComponentWithoutHint::FillWith1WhenSize0(std::vector<int> activInStates, int nStages)
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
std::queue<FrameEvent>* ComponentWithoutHint::GetBuffer()const
{
	return buffer;
}
std::unique_ptr<ComponentWithoutHint>&& ComponentWithoutHint::Clone()const
{
	return std::make_unique<ComponentWithoutHint>(ComponentWithoutHint(*this));
}
void Component::AddHint(Hint& hint)
{
	this->hint = std::make_unique<Hint>(hint);
}
void Component::DrawHintWhenVisible(Graphics& gfx)
{
	if (hint != nullptr && mouseHovers)
	{
		hint->Draw(gfx);
	}
}
void Component::UpdateHintPos(Vei2 mP)
{
	if (hint != nullptr && mouseHovers)
	{
		hint->SetLastMP(mP);
	}
}
//  ### Framehandle::PageFrame ###


//	### Framehandle::MultiFrame ###
Frame* MultiFrame::AddFrame(std::string key, RectF pos, int type, sharedResC resC, Component* parentC)
{
	frames[key] = std::make_unique<Frame>(pos,type,resC,parentC, buffer);
	frameOrder[(int)frameOrder.size()] = key;
	return frames[key].get();
}
PageFrame* MultiFrame::AddPageFrame(std::string key, RectF pos, int type, sharedResC resC, Component* parentC, int nPages)
{
	frames[key] = std::make_unique<PageFrame>(pos, type, resC, parentC, nPages, buffer);
	frameOrder[(int)frameOrder.size()] = key;
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
			Frame* f = static_cast<Frame*>(parentC->GetComp(key));	
			f->SetAnimationOfBouth(&resC->GetSurf().items[inv->GetItem(i)->get()->GetId()]);
			
			Hint h = Hint(resC, Vei2(2,2));
			h.AddHintTextBox("wfhuwfhwuhuwf wwji jijw fjiwifj jiwf ijfw ijfw jiwjiwf  fwfi jwjiwjifw ijff wijwifjijfw ijfwijwf wj wfijjifwijf ", &resC->GetSurf().fonts[0], Colors::Green);
			f->AddHint(h);

			//gI->AddHint(h);

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
int FrameHandle::GetHitInventorySlot(std::string frameString, Vec2 mP)
{
	if (comps.count(frameString) <= 0)
	{
		return -1;
	}
	int nSlots = 0;
	Frame* frame = nullptr;

	frame = static_cast<Frame*>(comps[frameString].get());
	if (frameString == "fInventory")
	{
		nSlots = 8;
	}
	if (frameString == "fInventoryBox")
	{
		nSlots = 9; 
	}
	if (frameString == "fInventoryStorage")
	{
		nSlots = 25;
	}
	if (frameString == "fInventoryWrought")
	{
		nSlots = 6;
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
			
			if (static_cast<Frame*>(frame->GetComp(key))->GetPos().Contains(mP))
			{
				return frame->GetComp(key)->extra1;
			}
		}
	}
	return -1;
}
	// 270, 90		//180 60
Frame* FrameHandle::CreateBuildOption(RectF pos, int obstacleType, PageFrame* parentC, std::vector<int> activOnPages, World* world)
{
	std::string key = Settings::GetObstacleString(obstacleType);
	Frame* frame = parentC->AddFramePF(pos, key, { 1 }, activOnPages);
	frame->s = &resC->GetSurf().windowsFrame[7].GetCurSurface();
	frame->extra1 = obstacleType;
	frame->bFunc = BBuildMode;
	frame->hitable = true;
	frame->AddText(Settings::GetObstacleString(obstacleType), RectF(Vec2(pos.GetWidth() / 3, pos.GetHeight() / 10), pos.GetWidth() / 3, 11), 11, &resC->GetSurf().fonts[0], Colors::Black, key + "H", { 1 });
	std::map<std::string, float> neededRes = Settings::obstacleStats[obstacleType].neededResToBuild;
	std::map<std::string, float>::iterator res;
	int i = 0;
	for (res = neededRes.begin(); res != neededRes.end(); res++)
	{
		//std::map<st::string,float> check ={{}}
		std::string amount = GigaMath::float2StringWithPrecision(2, res->second);

		if (world->GetPlayer()->GetMaterials().Has({ { res->first,res->second } }))
			frame->AddText("-" + res->first + ": x" + amount + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(pos.GetWidth() / 2, (pos.GetHeight() / 3) + i * (pos.GetHeight() / 6)), 50.f, 11.f), 11, &resC->GetSurf().fonts[0], Colors::Green, key + "res" + std::to_string(i), { 1 });
		else
		{
			frame->AddText("-" + res->first + ": x" + amount + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(pos.GetWidth() / 2, (pos.GetHeight() / 3) + i * (pos.GetHeight() / 6)), 50.f, 11.f), 11, &resC->GetSurf().fonts[0], Colors::Red, key + "res" + std::to_string(i), { 1 });
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
	frame->AddText(Settings::GetItemString(itemType), RectF(Vec2(pos.GetWidth() / 3, pos.GetHeight() / 10), pos.GetWidth() / 3, 11), 11, &resC->GetSurf().fonts[0], Colors::Black, key + "H", { 1 });
	std::map<std::string, float> neededRes = Settings::itemStats[itemType].neededResToCraft;
	std::map<std::string, float>::iterator res;
	int i = 0;
	for (res = neededRes.begin(); res != neededRes.end(); res++)
	{
		//std::map<st::string,float> check ={{}}
		if (world->GetPlayer()->GetMaterials().Has({ { res->first,res->second } }))
			frame->AddText("-" + res->first + ": x" + std::to_string(res->second) + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(pos.GetWidth() / 2, (pos.GetHeight() / 3) + i * (pos.GetHeight() / 6)), 50.f, 11.f), 11, &resC->GetSurf().fonts[0], Colors::Green, key + "res" + std::to_string(i), { 1 });
		else
		{
			frame->AddText("-" + res->first + ": x" + std::to_string(res->second) + " " + Settings::lang_kilogram[Settings::lang], RectF(Vec2(pos.GetWidth() / 2, (pos.GetHeight() / 3) + i * (pos.GetHeight() / 6)), 50.f, 11.f), 11, &resC->GetSurf().fonts[0], Colors::Red, key + "res" + std::to_string(i), { 1 });
		}
		i++;
	}
	//Frame* frame = AddFrame(key, pos, 0);
	return nullptr;// static_cast<Frame*>(comps[key].get());
}
Frame* FrameHandle::CreateDragFrameWithHpBar(Frame* parentC, RectF pos, const Animation* a, const Animation* aHover, std::queue<FrameEvent>* buffer, std::string key, std::vector<int> activInStates)
{
	Frame* f = parentC->AddFrame(pos, 1, resC, parentC, buffer, key, activInStates);
	f->SetVisible(true);
	f->SetDragable(true);
	//parentC->AddImage(RectF(Vec2(0,0),0,0), a, aHover, buffer, key+"Img", activInStates);
	//GrabImage* gi = parentC->AddGrabImage(pos, a, aHover, buffer, key, activInStates);
	//gi->SetVisible(false);

	Image* image = f->AddImage(RectF(Vec2(0, pos.GetHeight() / 5 * 4), pos.GetWidth(), pos.GetHeight() / 5), &resC->GetSurf().frames[1], &resC->GetSurf().frames[1], buffer, key + "Hp");
	image = f->AddImage(RectF(Vec2(0, pos.GetHeight() / 5 * 4), pos.GetWidth(), pos.GetHeight() / 5), &resC->GetSurf().frames[2], &resC->GetSurf().frames[2], buffer, key + "HpIs");
	return f;
}
void FrameHandle::AddHeadline(Frame* parentC, std::string text, const Font* f, Color c)
{
	parentC->AddText(text, (RectF)resC->GetFrameSize().GetFramePos("frameHeadline"), 11*resC->GetFrameSize().GetGuiScale(),f, Colors::Black, "tHeadline");
}
int FrameHandle::AddInfo(Frame* parentC, int top, const Font* f, Color c, std::string key, std::string infoText)
{
	if (infoText == "###")
	{
		infoText = Settings::lang_noInformation[Settings::lang];
	}
	RectF parentRect = parentC->GetPos();
	int xStart = (int)((float)(parentRect.GetWidth()) / 40);
	int halfParentWidth = parentRect.GetWidth() / 2;
	int textSize = (int)((float)(11) * resC->GetFrameSize().GetGuiScale());
	std::string infoString = GetInfoString(key);
	//parentC->AddTextBox()
	TextBox* tb1 = parentC->AddTextBox(infoText, RectF(Vec2(halfParentWidth, top), halfParentWidth, 14), textSize, &resC->GetSurf().fonts[0], c, key+"Is", {0,1}, 1);
	TextBox* tb2 = parentC->AddTextBox(infoString, RectF(Vec2(xStart, top), halfParentWidth, 14), textSize, &resC->GetSurf().fonts[0], Colors::Black, key, { 0,1 }, 1);
	int nLines = tb1->GetLines().size();
	if (tb2->GetLines().size() > nLines)
	{
		nLines = tb2->GetLines().size();
	}
	return top + textSize*nLines + 10;
}
int FrameHandle::AddInfoTextBox(Frame* parentC,std::string text, int top, const Font* f, Color c, std::string key)
{
	RectF parentRect = parentC->GetPos();
	int xStart = (int)((float)(parentRect.GetWidth()) / 40);
	int textSize = (int)((float)(11) * resC->GetFrameSize().GetGuiScale());
	TextBox* tB_townHall = parentC->AddTextBox(text, RectF(Vec2(xStart, top), parentRect.GetWidth() - xStart*2, 40), textSize, &resC->GetSurf().fonts[0], Colors::Black, key, { 0,1 });
	return top + textSize * (tB_townHall->GetLines().size() + 2);
}
int FrameHandle::AddObstacleAttackButton(Frame* parentC, int top, const Font* f, Color c)
{
	RectF parentRect = parentC->GetPos();
	int xStart = (int)((float)(parentRect.GetWidth()) / 40);
	int halfParentWidth = parentRect.GetWidth() / 2;
	int buttonBoxSize = (int)((float)(120) * resC->GetFrameSize().GetGuiScale());

	Button* b_setAttack = parentC->AddButton(RectF(Vec2(xStart, top), buttonBoxSize, buttonBoxSize / 2), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], "bSetAttack", &resC->GetSurf().fonts[0], { 0,1 });
	b_setAttack->bFunc = BSetAttackMode;
	b_setAttack->text = Settings::lang_attack[Settings::lang] + " (A)";
	b_setAttack->size = (int)((float)(11) * resC->GetFrameSize().GetGuiScale());
	b_setAttack->c = c;
	b_setAttack->hitable = true;
	return top + halfParentWidth / 2 + 10;
}
int FrameHandle::AddObstacleCheckBox(Frame* parentC, int top, const Font* f, Color c,std::string text, std::string key)
{
	RectF parentRect = parentC->GetPos();
	int xStart = (int)((float)(parentRect.GetWidth()) / 40);
	int halfParentWidth = parentRect.GetWidth() / 2;
	int textSize = (int)((float)(11) * resC->GetFrameSize().GetGuiScale());
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
	else if (key == "tFlora")
	{
		return Settings::lang_flora[Settings::lang] + ":";
	}
	else if (key == "tObstacle")
	{
		return Settings::lang_Obstacle[Settings::lang] + ":";
	}
	else if (key == "tObstacleHP")
	{
		return Settings::lang_ObstacleHP[Settings::lang] + ":";
	}
	else if (key == "tUnits")
	{
		return Settings::lang_units[Settings::lang] + ":";
	}
	else if (key == "tMaxUnits")
	{
		return Settings::lang_maxUnits[Settings::lang] + ":";
	}
	std::string ressource = key.substr(1, key.length() - 1);
	ressource[0] += 32;
	if (Settings::TranslateRessource(ressource, Settings::lang).find("ERROR") == std::string::npos)
	{
		return ressource;
	}

	return "ERROR: key not found(method GetInfoString()):" + key;
}
bool FrameHandle::FrameIsEnabled(Obstacle* obstacle, std::string key)
{
	bool obstacleFrame = std::any_of(Settings::frameKeysObstacle.begin(), Settings::frameKeysObstacle.end(), [&](const std::string val) {
		return val == key;
	});
	if (curScene == 0)
	{
		if (obstacleFrame && obstacle != nullptr)
		{
			if (key == "fUnit")
			{
				return Settings::anyOfCreature(obstacle->GetType());
			}
			if (key == "fTownhall")
			{
				return obstacle->GetType() == 3;
			}
			if (key == "fLumberjackHut")
			{
				return obstacle->GetType() == 27;
			}
			if (key == "fInventory")
			{
				return Settings::anyOfCreature(obstacle->GetType());
			}
			if (key == "fInventoryBox")
			{
				return obstacle->GetType() == 6;
			}
			if (key == "fInventoryStorage")
			{
				return obstacle->GetType() == 50;
			}
			if (key == "fInventoryWrought")
			{
				return obstacle->GetType() == 30;
			}
			return false;
		}
		else if (obstacleFrame)
		{
			return false;
		}
		return true;
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

		start = AddInfoTextBox(fTownhall, Settings::lang_TownhallInfo[Settings::lang], start, &resC->GetSurf().fonts[0], Colors::Black, "tbObstacleInfo");

		start = AddInfo(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitHp");
		start = AddInfo(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitAttacksLeft");
		start = AddInfo(fTownhall, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitTeam");

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
		
		start = AddInfoTextBox(fLumberjackHut, Settings::lang_LumberjackHutInfo[Settings::lang], start, &resC->GetSurf().fonts[0], Colors::Black, "tbObstacleInfo");
		
		start = AddInfo(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitHp");
		start = AddInfo(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitAttacksLeft");
		start = AddInfo(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitTeam");

		start = AddObstacleCheckBox(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black, Settings::lang_automatic[Settings::lang] + ":", "automaticChop");
		start = AddObstacleAttackButton(fLumberjackHut, start, &resC->GetSurf().fonts[0], Colors::Black);
		static_cast<Button*>(comps[key]->GetComp("bSetAttack"))->text = Settings::lang_chop[Settings::lang];
	}
	else if (key == "fInventory")
	{
		Frame* fInventory = AddFrame("fInventory", RectF(Vec2(Graphics::ScreenWidth / 2 - 125, Graphics::ScreenHeight / 12), 250, 190), 1);
		fInventory->s = &resC->GetSurf().windowsFrame[8].GetCurSurface();
		
		Frame* hand1 = CreateDragFrameWithHpBar(fInventory, RectF(Vec2(70, 10), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item0", { 1,1 });
		hand1->extra1 = 0;
		hand1->extraS1 = "inventory swap fInventory";

		Frame* bonus2 = CreateDragFrameWithHpBar(fInventory, RectF(Vec2(130, 10), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item1", { 1,1 });
		bonus2->extra1 = 1;
		bonus2->extraS1 = "inventory swap fInventory";

		Frame* armor = CreateDragFrameWithHpBar(fInventory, RectF(Vec2(70, 70), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item2", { 1,1 });
		armor->SetVisible(false);
		armor->extra1 = 2;
		armor->extraS1 = "inventory swap fInventory";

		Frame* bonus1 = CreateDragFrameWithHpBar(fInventory, RectF(Vec2(130, 70), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item3", { 1,1 });
		bonus1->SetVisible(false);
		bonus1->extra1 = 3;
		bonus1->extraS1 = "inventory swap fInventory";

		Frame* item1 = CreateDragFrameWithHpBar(fInventory, RectF(Vec2(10, 130), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item4", { 1,1 });
		item1->SetVisible(false);
		item1->extra1 = 4;
		item1->extraS1 = "inventory swap fInventory";

		Frame* item2 = CreateDragFrameWithHpBar(fInventory, RectF(Vec2(70, 130), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item5", { 1,1 });
		item2->SetVisible(false);
		item2->extra1 = 5;
		item2->extraS1 = "inventory swap fInventory";

		Frame* item3 = CreateDragFrameWithHpBar(fInventory, RectF(Vec2(130, 130), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item6", { 1,1 });
		item3->SetVisible(false);
		item3->extra1 = 6;
		item3->extraS1 = "inventory swap fInventory";

		Frame* item4 = CreateDragFrameWithHpBar(fInventory, RectF(Vec2(190, 130), 50, 50), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item7", { 1,1 });
		item4->SetVisible(false);
		item4->extra1 = 7;
		item4->extraS1 = "inventory swap fInventory";
	}
	else if (key == "fInventoryBox")
	{
		Frame* fInventoryBox = AddFrame("fInventoryBox", RectF(Vec2(Graphics::ScreenWidth / 8 * 5, Graphics::ScreenHeight / 12), 190, 190), 1);
		fInventoryBox->s = &resC->GetSurf().windowsFrame[9].GetCurSurface();

		for (int i = 0; i < 9; i++)
		{
			Frame* image = CreateDragFrameWithHpBar(fInventoryBox, RectF(Vec2(10.f + (int)(i % 3) * 60.f, 10.f + (int)(i / 3) * 60.f), 50.f, 50.f), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item" + std::to_string(i), { 1,1 });
			image->SetVisible(false);
			image->extra1 = i;
			image->extraS1 = "inventory swap fInventoryBox";
		}
	}
	else if (key == "fInventoryStorage")
	{
		Frame* fInventoryStorage = AddFrame("fInventoryStorage", RectF(Vec2(Graphics::ScreenWidth / 8 * 5, Graphics::ScreenHeight / 12), 310, 310), 1);
		fInventoryStorage->s = &resC->GetSurf().windowsFrame[10].GetCurSurface();

		for (int i = 0; i < 25; i++)
		{
			Frame* image = CreateDragFrameWithHpBar(fInventoryStorage, RectF(Vec2(10.f + (int)(i % 5) * 60, 10.f + (int)(i / 5) * 60), 50.f, 50.f), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item" + std::to_string(i), { 1,1 });
			image->SetVisible(false);
			image->extra1 = i;
			image->extraS1 = "inventory swap fInventoryStorage";
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
			Frame* image = CreateDragFrameWithHpBar(fInventoryWrought, RectF(Vec2(10.f + (int)(i % 3) * 60.f, 40.f + (int)(i / 3) * 90.f), 50.f, 50.f), &resC->GetSurf().items[0], &resC->GetSurf().items[0], &buffer, "gI_item" + std::to_string(i), { 1,1 });
			image->SetVisible(false);
			image->extra1 = i;
			image->extraS1 = "inventory swap fInventoryWrought";
		}
	}
}
void FrameHandle::UpdateFrame(const World* world, int process, std::string key, Component* comp)
{
	Obstacle* obstacle = nullptr;
	if (process == 0)
	{
		obstacle = world->GetFocusedObstacle();
	}
	else if (process == 1)
	{
		obstacle = world->GetStorageObstacle();
	}
	//
	if (comp->GetComp("tFloraIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tFloraIs"))->Settext(Settings::GetTypeString(world->GetfCellType()));
	}
	if (comp->GetComp("tObstacleIs") != nullptr)
	{
		if(world->GetFocusedObstacle() != nullptr)
			static_cast<TextBox*>(comp->GetComp("tObstacleIs"))->Settext(Settings::GetObstacleString(world->GetFocusedObstacle()->GetType()));
		else
			static_cast<TextBox*>(comp->GetComp("tObstacleIs"))->Settext("...");
	}
	if (comp->GetComp("tObstacleHPIs") != nullptr)
	{
		if (world->GetFocusedObstacle() != nullptr)
			static_cast<TextBox*>(comp->GetComp("tObstacleHPIs"))->Settext(std::to_string(world->GetFocusedObstacle()->GetHP()) + " / " + std::to_string(Settings::obstacleStats[world->GetFocusedObstacle()->GetType()].baseHp));
		else
			static_cast<TextBox*>(comp->GetComp("tObstacleHPIs"))->Settext("...");
	}
	const std::map<std::string, float>* playerM = world->GetPlayer()->GetMaterials().GetRawData();
	std::map<std::string, float>::const_iterator i;
	for (i = playerM->begin(); i != playerM->end(); i++)
	{
		std::string key = i->first;
		key[0] -= 32;
		key = "t" + key + "Is";
		if (comp->GetComp(key) != nullptr)
		{ 
			if (key != "tMaxUnitsIs" && key != "tUnitsIs")
			{
				static_cast<TextBox*>(comp->GetComp(key))->Settext(GigaMath::float2StringWithPrecision(2, i->second) + " kg");
			}
			else
			{
				static_cast<TextBox*>(comp->GetComp(key))->Settext(GigaMath::float2StringWithPrecision(2, i->second));
			}
		}
	}
	/*
	if (comp->GetComp("tWoodIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tWoodIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	if (comp->GetComp("tIronIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tIronIs"))->Settext(std::to_string(playerM->values["iron"]));
	}
	if (comp->GetComp("tSandIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tSandIs"))->Settext(std::to_string(playerM->values["sand"]));
	}
	if (comp->GetComp("tStoneIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tStoneIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	if (comp->GetComp("tCopperIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tCopperIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	if (comp->GetComp("tGoldIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tGoldIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	if (comp->GetComp("tAluminumIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tAluminumIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	if (comp->GetComp("tEmeraldIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tEmeraldIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	if (comp->GetComp("tSapphireIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tSapphireIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	if (comp->GetComp("tRobinIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tRobinIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	if (comp->GetComp("tDimondIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tDimondIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	if (comp->GetComp("tAmberIs") != nullptr)
	{
		static_cast<TextBox*>(comp->GetComp("tAmberIs"))->Settext(std::to_string(playerM->values["wood"]));
	}
	*/
	//
	if (obstacle != nullptr)
	{
		if (comp->GetComp("tUnitNameIs")!= nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitNameIs"))->Settext(Settings::GetObstacleString(obstacle->GetType()));
		}
		if (comp->GetComp("tUnitHpIs") != nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitHpIs"))->Settext(std::to_string(obstacle->GetHP()) + " / " + std::to_string(Settings::obstacleStats[obstacle->GetType()].baseHp));
		}
		if (comp->GetComp("tUnitTeamIs") != nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitTeamIs"))->Settext(obstacle->GetTeam()->GetTeamName());
		}
		if (comp->GetComp("tUnitStepsLeftIs") != nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitStepsLeftIs"))->Settext(std::to_string(obstacle->GetStepsLeft()));
		}
		if (comp->GetComp("tUnitAttacksLeftIs") != nullptr)
		{
			static_cast<TextBox*>(comps[key]->GetComp("tUnitAttacksLeftIs"))->Settext(std::to_string(obstacle->GetAttackTrait()->GetAttacksLeft()));
		}
		if (key == "fUnit")
		{
			std::map<std::string, std::string> map2Parse;
			auto compsOfOld = comp->GetComps();
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
			Frame* f_townhall = static_cast<Frame*>(comp);
			if (obstacle->GetEducation()->Educates())
				static_cast<CheckBox*>(f_townhall->GetComp("cBeducate"))->Check();
			else
				static_cast<CheckBox*>(f_townhall->GetComp("cBeducate"))->Uncheck();
			if (obstacle->GetHealTrait()->Isenabled())
				static_cast<CheckBox*>(f_townhall->GetComp("cBheal"))->Check();
			else
				static_cast<CheckBox*>(f_townhall->GetComp("cBheal"))->Uncheck();
			if (obstacle->GetAttackTrait()->GetReloadNextTurn())
				static_cast<CheckBox*>(f_townhall->GetComp("cBattack"))->Check();
			else
				static_cast<CheckBox*>(f_townhall->GetComp("cBattack"))->Uncheck();
		}
		if (key == "fLumberjackHut")
		{
			Frame* fLumberjackHut = static_cast<Frame*>(comp);
			if (obstacle->GetAttackTrait()->GetAutomaticMode() == CtPos(Vei2(-1, -1), Vei2(-1, -1)))
				static_cast<CheckBox*>(fLumberjackHut->GetComp("cBautomaticChop"))->Uncheck();
			else
				static_cast<CheckBox*>(fLumberjackHut->GetComp("cBautomaticChop"))->Check();
		}
		if (key.find("Inventory") != std::string::npos && FrameIsEnabled(obstacle,key))		//handles all needed Inventoryupdates
		{
			UpdateInventoryComps(obstacle->GetInventory(), comp);
		}
		if (key == "fInventoryWrought")
		{
			Frame* fInventoryWrought = static_cast<Frame*>(comp);
			if (obstacle->GetCraftTrait()->IsCrafting())
			{
				fInventoryWrought->GetComp("t_craftStatus")->text = Settings::lang_status[Settings::lang] + ": " + Settings::GetItemString(obstacle->GetCraftTrait()->GetItemID()) + " " + Settings::lang_finishedIn[Settings::lang] + " " + std::to_string(obstacle->GetCraftTrait()->TurnsLeft(obstacle->GetProductivity())) + " " + Settings::lang_rounds[Settings::lang];
				fInventoryWrought->GetComp("t_craftStatus")->c = Colors::Green;
			}
			else if (obstacle->GetCraftTrait()->GetItemID() != -1 && !obstacle->GetTeam()->GetMaterials().Has(Settings::itemStats[obstacle->GetCraftTrait()->GetItemID()].neededResToCraft))
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
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitName");
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitHp");
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitTeam");
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitStepsLeft");
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitAttacksLeft");
		start = AddObstacleAttackButton(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black);
	}
	else
	{
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitName", parsedText->find("tUnitNameIs")->second);
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitHp", parsedText->find("tUnitHpIs")->second	);
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitTeam", parsedText->find("tUnitTeamIs")->second);
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitStepsLeft", parsedText->find("tUnitStepsLeftIs")->second);
		start = AddInfo(fUnity, start, &resC->GetSurf().fonts[0], Colors::Black, "tUnitAttacksLeft", parsedText->find("tUnitAttacksLeftIs")->second);
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
	bool hit = false;
	if (Settings::framesOn)
	{
		for (int i = 0; i < handleOrder.size(); i++)
		{
			if (comps.count("fInventory") > 0 &&  handleOrder[i] == comps["fInventory"].get())
			{
				int k = 23;//remove me
			}
			hit = handleOrder[i]->HandleMouseInput(e,true) || hit;
		}
		UpdateHandleOrder();
	}
	return hit;
}

void FrameHandle::Draw(Graphics& gfx)
{
	if (Settings::framesOn)
	{
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
		RectI screenRect = Graphics::GetScreenRect<int>();
		int screenHeight = screenRect.GetHeight();
		int screenWidth = screenRect.GetWidth();

		if (scene == 0)
		{
			using namespace Settings;
			std::vector<int> a = { 0,1 };

					
			//Frame* fUnity = AddFrame("fUnit", (RectF)resC->GetFrameSize().GetFramePos("framePos"), 0); frameResDisPos

			//MultiFrame* m = AddMultiFrame("f_resD", RectF(Vec2(1040, 110), 140, 280), 0, 1);
			RectF resDisRect = (RectF)resC->GetFrameSize().GetFramePos("frameResDisPos");
			MultiFrame* m = AddMultiFrame("f_resD", resDisRect, 0, 1);

			Frame* f1 = m->AddFrame("fresD_f1", RectF(Vec2(0, 0), resDisRect.GetWidth(), resDisRect.GetHeight()), 0, resC, m);//
			AddHeadline(f1, Settings::lang_fieldInformation[Settings::lang], &resC->GetSurf().fonts[0], Colors::Black);
			int f1Top = Settings::percentForGrab * resDisRect.GetHeight()+10;
			f1Top = AddInfo(f1, f1Top, &resC->GetSurf().fonts[0], Colors::Black, "tFlora", "...");
			f1Top = AddInfo(f1, f1Top, &resC->GetSurf().fonts[0], Colors::Black, "tObstacle","...");
			f1Top = AddInfo(f1, f1Top, &resC->GetSurf().fonts[0], Colors::Black, "tObstacleHP", "...");
			f1Top = AddInfo(f1, f1Top, &resC->GetSurf().fonts[0], Colors::Black, "tUnits", "...");
			f1Top = AddInfo(f1, f1Top, &resC->GetSurf().fonts[0], Colors::Black, "tMaxUnits", "...");

			Frame* f2 = m->AddFrame("fresD_res", RectF(Vec2(0, Settings::percentForGrab * resDisRect.GetHeight()), resDisRect.GetWidth(), resDisRect.GetHeight()), 0, resC, m);//
			AddHeadline(f2, Settings::lang_resources[Settings::lang], &resC->GetSurf().fonts[0], Colors::Black);
			int f2Top = Settings::percentForGrab * resDisRect.GetHeight() *2* 0.75f;
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tWood", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tIron", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tSand", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tStone", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tCopper", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tGold", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tAluminum", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tEmerald", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tSapphire", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tRobin", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tDiamond", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tAmber", "..."); 
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tLithium", "...");
			f2Top = AddInfo(f2, f2Top, &resC->GetSurf().fonts[0], Colors::Black, "tGunpowder", "...");


			Frame* f3 = m->AddFrame("fresD_mat", RectF(Vec2(0, Settings::percentForGrab * resDisRect.GetHeight() * 2), resDisRect.GetWidth(), resDisRect.GetHeight()), 0, resC, m);//
			AddHeadline(f3, Settings::lang_materials[Settings::lang], &resC->GetSurf().fonts[0], Colors::Black);
			int f3Top = Settings::percentForGrab * resDisRect.GetHeight() * 3 * 0.50f;
			
			f3Top = AddInfo(f3, f3Top, &resC->GetSurf().fonts[0], Colors::Black, "tSteel", "...");
			f3Top = AddInfo(f3, f3Top, &resC->GetSurf().fonts[0], Colors::Black, "tPlastic", "...");
			f3Top = AddInfo(f3, f3Top, &resC->GetSurf().fonts[0], Colors::Black, "tConcrete", "...");
			f3Top = AddInfo(f3, f3Top, &resC->GetSurf().fonts[0], Colors::Black, "tGlass", "...");
			f3Top = AddInfo(f3, f3Top, &resC->GetSurf().fonts[0], Colors::Black, "tCeramics", "...");
			f3Top = AddInfo(f3, f3Top, &resC->GetSurf().fonts[0], Colors::Black, "tSnow", "...");
			f3Top = AddInfo(f3, f3Top, &resC->GetSurf().fonts[0], Colors::Black, "tBricks", "...");
			f3Top = AddInfo(f3, f3Top, &resC->GetSurf().fonts[0], Colors::Black, "tSlate", "...");


			Frame* f4 = m->AddFrame("fresD_org", RectF(Vec2(0, Settings::percentForGrab * resDisRect.GetHeight()*3), resDisRect.GetWidth(), resDisRect.GetHeight()), 0, resC, m);//
			AddHeadline(f4, Settings::lang_organic[Settings::lang], &resC->GetSurf().fonts[0], Colors::Black);
			int f4Top = Settings::percentForGrab * resDisRect.GetHeight() * 4 * 0.3f;

			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tCorals", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tSticks", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tLeaves", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tWool", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tLeather", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tFur", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tMeat", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tFish", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tBerrys", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tApples", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tCactus", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tSilicon", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tSapling", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tFeather", "...");
			f4Top = AddInfo(f4, f4Top, &resC->GetSurf().fonts[0], Colors::Black, "tSilk", "...");
			
			Frame* f5 = m->AddFrame("fresD_science", RectF(Vec2(0, Settings::percentForGrab * resDisRect.GetHeight() * 4), resDisRect.GetWidth(), resDisRect.GetHeight()), 0, resC, m);//
			AddHeadline(f5, Settings::lang_science[Settings::lang], &resC->GetSurf().fonts[0], Colors::Black);
			int f5Top = Settings::percentForGrab * resDisRect.GetHeight() * 5 * 0.3f;
			
			f5Top = AddInfo(f5, f5Top, &resC->GetSurf().fonts[0], Colors::Black, "tComputer chips", "...");


			comps["b_NextTurn"] = std::make_unique<Button>(Button(RectF(Vec2(Graphics::ScreenWidth/20*17, Graphics::ScreenHeight *13/16), Graphics::ScreenWidth / 10, Graphics::ScreenHeight / 12), &resC->GetSurf().windowsFrame[3], &resC->GetSurf().windowsFrame[3], { 0,0 }, &resC->GetSurf().fonts[0], nullptr, &buffer));
			//comps["b_NextTurn"] = std::make_unique<Button>(Button(RectF(Vec2(650, 450), 120, 60), &resC->GetSurf().windowsFrame[3], &resC->GetSurf().windowsFrame[3], { 0,0 }, &resC->GetSurf().fonts[0], nullptr, &buffer));
			static_cast<Button*>(comps["b_NextTurn"].get())->bFunc = BNextTurn;

			comps["b_buildScene"] = std::make_unique<Button>(Button(RectF(Vec2(Graphics::ScreenWidth/42, Graphics::ScreenHeight/12), Graphics::ScreenWidth / 10, Graphics::ScreenHeight / 12), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], { 0,0 }, &resC->GetSurf().fonts[0], nullptr, &buffer));
			Button* button = static_cast<Button*>(comps["b_buildScene"].get());
			button->extra1 = 1;
			button->size = 14;
			button->bFunc = BLoadScene;
			button->text = Settings::lang_build[Settings::lang];

			comps["b_wroughScene"] = std::make_unique<Button>(Button(RectF(Vec2(Graphics::ScreenWidth / 42, Graphics::ScreenHeight / 24*5), Graphics::ScreenWidth / 10, Graphics::ScreenHeight / 12), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], { 0,0 }, &resC->GetSurf().fonts[0], nullptr, &buffer));
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
			fBuildSelection->AddTextPF(Settings::lang_housing[Settings::lang], RectF(Vec2(screenWidth / 8 * 3, screenHeight / 24), screenHeight / 2, screenWidth / 21), 50, &resC->GetSurf().fonts[0], Colors::Black, "HousingH", { 1 }, { 1,0,0,0 });

			Button* b_back = fBuildSelection->AddButtonPF(RectF(Vec2(30, 60), 60, 30), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], "b_buildback", &resC->GetSurf().fonts[0], { 1 }, { 1,1,1,1 });
			b_back->extra1 = 0;
			b_back->bFunc = BLoadScene;
			b_back->text = Settings::lang_back[Settings::lang];
			

			CreateBuildOption(RectF(Vec2(screenWidth/25, screenHeight / 6), screenWidth/5, screenHeight / 8), 2, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 25, screenHeight / 6 + 1 * screenHeight / 6), screenWidth/5, screenHeight / 8), 6, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 25, screenHeight / 6 + 2 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 3, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 25, screenHeight / 6 + 3 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 21, fBuildSelection, { 1,0,0,0 }, world);
			
			CreateBuildOption(RectF(Vec2(screenWidth / 12.5f + screenWidth / 5, screenHeight / 6), screenWidth / 5, screenHeight / 8), 0, fBuildSelection, { 1,0,0,0 }, world);

			CreateBuildOption(RectF(Vec2(screenWidth / 8.3333f + screenWidth / 5 * 2, screenHeight / 6), screenWidth / 5, 90), 22, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 8.3333f + screenWidth / 5 * 2, screenHeight / 6 + 1 * screenHeight / 6), 270, 90), 23, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 8.3333f + screenWidth / 5 * 2, screenHeight / 6 + 2 * screenHeight / 6), 270, 90), 24, fBuildSelection, { 1,0,0,0 }, world);

			CreateBuildOption(RectF(Vec2(screenWidth / 6.25f + screenWidth / 5 * 3, screenHeight / 6), screenWidth / 5, 90), 25, fBuildSelection, { 1,0,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 6.25f + screenWidth / 5 * 3, screenHeight / 6 + 1 * screenHeight / 6), screenWidth / 5, 90), 26, fBuildSelection, { 1,0,0,0 }, world);


			fBuildSelection->AddTextPF(Settings::lang_productions[Settings::lang], RectF(Vec2(screenWidth / 8 * 3, screenHeight / 24), screenHeight / 2, screenWidth / 21), 50, &resC->GetSurf().fonts[0], Colors::Black, "roductionH", { 1 }, { 0,1,0,0 });
			
			CreateBuildOption(RectF(Vec2(screenWidth / 25, screenHeight / 6), screenWidth / 5, screenHeight / 8), 27, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 25, screenHeight / 6 + 1 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 28, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 25, screenHeight / 6 + 2 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 33, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 25, screenHeight / 6 + 3 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 31, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 25, screenHeight / 6 + 4 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 34, fBuildSelection, { 0,1,0,0 }, world);

			CreateBuildOption(RectF(Vec2(screenWidth / 12.5f + screenWidth / 5, screenHeight / 6), screenWidth / 5, screenHeight / 8), 30, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 12.5f + screenWidth / 5, screenHeight / 6 + 1 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 32, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 12.5f + screenWidth / 5, screenHeight / 6 + 2 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 29, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 12.5f + screenWidth / 5, screenHeight / 6 + 3 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 36, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 12.5f + screenWidth / 5, screenHeight / 6 + 4 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 38, fBuildSelection, { 0,1,0,0 }, world);

			CreateBuildOption(RectF(Vec2(screenWidth / 8.3333f + screenWidth / 5 * 2, screenHeight / 6), screenWidth / 5, 90), 37, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 8.3333f + screenWidth / 5 * 2, screenHeight / 6 + 1 * screenHeight / 6), 270, 90), 35, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 8.3333f + screenWidth / 5 * 2, screenHeight / 6 + 2 * screenHeight / 6), 270, 90), 42, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 8.3333f + screenWidth / 5 * 2, screenHeight / 6 + 3 * screenHeight / 6), 270, 90), 49, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 8.3333f + screenWidth / 5 * 2, screenHeight / 6 + 4 * screenHeight / 6), 270, 90), 50, fBuildSelection, { 0,1,0,0 }, world);

			CreateBuildOption(RectF(Vec2(screenWidth / 6.25f + screenWidth / 5 * 3, screenHeight / 6), screenWidth / 5, 90), 39, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 6.25f + screenWidth / 5 * 3, screenHeight / 6 + 1 * screenHeight / 6), screenWidth / 5, 90), 40, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 6.25f + screenWidth / 5 * 3, screenHeight / 6 + 2 * screenHeight / 6), screenWidth / 5, 90), 41, fBuildSelection, { 0,1,0,0 }, world);
			CreateBuildOption(RectF(Vec2(screenWidth / 6.25f + screenWidth / 5 * 3, screenHeight / 6 + 3 * screenHeight / 6), screenWidth / 5, 90), 48, fBuildSelection, { 0,1,0,0 }, world);

			
		}
		else if (scene == 2)
		{
			std::vector<int> a = { 0,1 };
			Frame* fCraftSelection = AddFrame("f_craftMenu", Graphics::GetScreenRect<float>(), 1);//AddPageFrame("f_bg", RectF(Vec2(0, 0), Graphics::ScreenWidth, Graphics::ScreenHeight), 1, 4);															//build selection menu
			fCraftSelection->s = &resC->GetSurf().windowsFrame[5].GetCurSurface();
			fCraftSelection->SetMoveable(false);
			fCraftSelection->AddText(Settings::lang_forge[Settings::lang], RectF(Vec2(screenWidth / 8 * 3, screenHeight/24), screenHeight/2, screenWidth / 21), 50, &resC->GetSurf().fonts[0], Colors::Black, "ForgeH", { 1 });

			Button* b_back = fCraftSelection->AddButton(RectF(Vec2(30, 60), 60, 30), &resC->GetSurf().windowsFrame[6], &resC->GetSurf().windowsFrame[6], "b_buildback", &resC->GetSurf().fonts[0], { 1 });
			b_back->extra1 = 0;
			b_back->bFunc = BLoadScene;
			b_back->text = Settings::lang_back[Settings::lang];

			CreateCraftOption(RectF(Vec2(screenWidth / 25, screenHeight / 6), screenWidth / 5, screenHeight / 8), 0, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(screenWidth / 25, screenHeight / 6 + 1 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 1, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(screenWidth / 25, screenHeight / 6 + 2 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 2, fCraftSelection, { 1,0,0,0 }, world);

			CreateCraftOption(RectF(Vec2(screenWidth / 12.5f + screenWidth / 5, screenHeight / 6), screenWidth / 5, screenHeight / 8), 9, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(screenWidth / 12.5f + screenWidth / 5, screenHeight / 6 + 1 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 10, fCraftSelection, { 1,0,0,0 }, world);
			CreateCraftOption(RectF(Vec2(screenWidth / 12.5f + screenWidth / 5, screenHeight / 6 + 2 * screenHeight / 6), screenWidth / 5, screenHeight / 8), 11, fCraftSelection, { 1,0,0,0 }, world);
		}
		UpdateHandleOrder();
		UpdateFrames(world);
	}
}
void FrameHandle::UpdateFrames(const World* world)
{
	Obstacle* obst = world->GetFocusedObstacle();
	Obstacle* storage = world->GetStorageObstacle();
	if (Settings::framesOn)
	{
		for (int i = 0; i < (int)Settings::GetFrameKeys().size(); i++)
		{
			std::string key = Settings::GetFrameKeys()[i];
			if (comps.count(key) > 0)
			{
				DeterminateUpdateProc(world, comps[key].get(), key);
			}
			else if (FrameIsEnabled(obst, key))
			{
				LoadFrame(key);
				UpdateFrame(world, 0, key, comps[key].get());
			}
			else if (FrameIsEnabled(storage, key))
			{
				LoadFrame(key);
				UpdateFrame(world, 1, key, comps[key].get());
			}
		}
		UpdateHandleOrder();
	}
}
void FrameHandle::DeterminateUpdateProc(const World* world, Component* comp, std::string key)
{
	Obstacle* obst = world->GetFocusedObstacle();
	Obstacle* storage = world->GetStorageObstacle();


	if (MultiFrame* mf = dynamic_cast<MultiFrame*>(comp))
	{
		for (int i = 0; i < mf->GetFrameAmount(); i++)
		{
			DeterminateUpdateProc(world, mf->GetFrame(i), mf->GetKey(i));
		}
	}
	if (FrameIsEnabled(obst, key))
	{
		UpdateFrame(world, 0, key, comp);
	}
	else if (FrameIsEnabled(storage, key))
	{
		UpdateFrame(world, 1, key, comp);
	}
	else if (FrameIsEnabled(nullptr, key))
	{

	}
	else 
	{
		DeleteFrame(key);
	}
}

