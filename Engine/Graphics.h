/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "Surface.h"
#include <cassert>
#include <cmath>
#include "Matrix.h"
#include "Settings.h"
#include "Font.h"
class Graphics
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception(HRESULT hr, const std::wstring& note, const wchar_t* file, unsigned int line);
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x, y, z;		// position
		float u, v;			// texcoords
	};
public:
	Graphics(class HWNDKey& key);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void EndFrame();
	void BeginFrame();
	void PutPixel(int x, int y, int r, int g, int b)
	{
		PutPixel(x, y, { unsigned char(r),unsigned char(g),unsigned char(b) });
	}
	void PutPixel(int x, int y, Color c);
	Color GetPixel(int x, int y);
	void RainbowPutPixel(int x, int y);

	bool PixelInFrame(Vei2 check) { return screenRect.Contains(check); }
	void DrawCircle(int x, int y, float radius, Color c);
	void DrawCircle(int x, int y, float outerRadius,float innerRadius, Color innerC, Color outerC);
	void DrawLine(Vec2 p1, Vec2 p0, Color c, int thickness = 1);
	template<typename E>
	void DrawLine(Vec2 p1, Vec2 p0, E effect, int thickness = 1)		//
	{
		if (p0.x < 0 && p1.x < 0 || p0.x > ScreenWidth && p1.x > ScreenWidth || p0.y < 0 && p1.y < 0 || p0.y > ScreenHeight && p1.y > ScreenHeight)
		{
			return;
		}
		if (p1.x < 0)
		{
			p1.x = 0;
		}
		if (p1.y < 0)
		{
			p1.y = 0;
		}
		if (p1.x > ScreenWidth)
		{
			p1.x = ScreenWidth -1;
		}
		if (p1.y > ScreenHeight)
		{
			p1.y = ScreenHeight-1;
		}

		if (p0.x < 0)
		{
			p0.x = 0;
		}
		if (p0.y < 0)
		{
			p0.y = 0;
		}
		if (p0.x > ScreenWidth)
		{
			p0.x = ScreenWidth - 1;
		}
		if (p0.y > ScreenHeight)
		{
			p0.y = ScreenHeight - 1;
		}

		float m = 0.0f;
		if (p1.x != p0.x)
		{
			m = (p1.y - p0.y) / (p1.x - p0.x);
		}
		else
		{
			m = 2000000;
		}
		if (thickness > 1)
		{
			for (int i = -thickness / 2; i < thickness / 2; i++)
			{
				if (std::abs(m) <= 1.0f)
				{
					DrawLine(Vec2(p1.x, p1.y + i), Vec2(p0.x, p0.y + i), effect);
				}
				else
				{
					DrawLine(Vec2(p1.x + i, p1.y), Vec2(p0.x + i, p0.y), effect);
				}
			}
		}

		if (p1.x != p0.x && std::abs(m) <= 1.0f)
		{
			if (p0.x > p1.x)
			{
				std::swap(p0, p1);
			}

			const float b = p0.y - m * p0.x;

			for (int x = (int)p0.x; x < (int)p1.x; x++)
			{
				const float y = m * (float)x + b;

				const int yi = (int)y;
				if (x >= 0 && x < ScreenWidth && yi >= 0 && yi < ScreenHeight)
				{
					//PutPixel(x, yi, c);
					//RainbowPutPixel(x, yi);
					effect(x, yi, Colors::Magenta, *this);
				}
			}
		}
		else
		{
			if (p0.y > p1.y)
			{
				std::swap(p0, p1);
			}

			const float w = (p1.x - p0.x) / (p1.y - p0.y);
			const float p = p0.x - w * p0.y;

			for (int y = (int)p0.y; y < (int)p1.y; y++)
			{
				const float x = w * (float)y + p;

				const int xi = (int)x;
				if (xi >= 0 && xi < ScreenWidth && y >= 0 && y < ScreenHeight)
				{
					//PutPixel(xi, y, c);
					//RainbowPutPixel(xi, y);
					effect(xi, y, Colors::Magenta, *this);
				}
			}
		}
	}
	void DrawFilledRect(Vec2 pos, Vec2 size, Color c, float radiant = 0);
	template<typename E>
	void DrawFilledRect(RectF pos, Color c, E effect = SpriteEffect::Nothing())
	{
		for (int y = 0; y < pos.GetHeight(); y++)
		{
			for (int x = 0; x < pos.GetWidth(); x++)
			{
				if (PixelInFrame(Vei2((int)(x + pos.left), (int)(y + pos.top))))
				{
					effect((int)(x + pos.left), (int)(y + pos.top), c, *this);
				}
			}
		}
	}
	void DrawRect(RectF pos, Color c)
	{
		Vec2 p1(std::ceil(pos.left),pos.top);
		Vec2 p2(pos.right, pos.top);
		Vec2 p3(std::ceil(pos.left), pos.bottom);
		Vec2 p4(pos.right, pos.bottom);

		DrawLine(p1, p2, c);
		DrawLine(p1, p3, c);
		DrawLine(p4, p2, c);
		DrawLine(p4, p3, c);
	}
	bool FIDF(int first, int second)const;			//First is drawn first (for DrawCon)
	template <typename T>
	void DrawConnections(int lookFor, Vei2 topLeft, Matrix<int> aMat, std::vector<RectI> drawOffset, const Surface& surface, T effect)
	{
		using namespace Settings;
		assert(aMat.GetSize().x == 3 && aMat.GetSize().y == 3);

		if (FIDF(aMat[1][1], lookFor))
		{
			if (aMat[1][2] == lookFor)
			{
				if (aMat[0][1] == lookFor)	// 1
				{
					DrawSurface(drawOffset[0] + topLeft, RectI(Vei2(51, 0), 25, 25), surface, effect);
				}
				else if (aMat[0][1] != lookFor)	// 13
				{
					DrawSurface(drawOffset[0] + topLeft, RectI(Vei2(204, 0), 25, 25), surface, effect);
				}
				if (aMat[2][1] == lookFor)	// 2
				{
					DrawSurface(drawOffset[1] + topLeft, RectI(Vei2(76, 0), 25, 25), surface, effect);
				}
				else if (aMat[2][1] != lookFor)	// 14
				{
					DrawSurface(drawOffset[1] + topLeft, RectI(Vei2(229, 0), 25, 25), surface, effect);
				}
			}
			else if (aMat[1][2] != lookFor)
			{
				if (aMat[0][1] == lookFor)	//9
				{
					DrawSurface(drawOffset[0] + topLeft, RectI(Vei2(153, 0), 25, 25), surface, effect);
				}
				else if (FIDF(aMat[0][1], lookFor) && aMat[0][2] == lookFor)	// 5
				{
					DrawSurface(drawOffset[0] + topLeft, RectI(Vei2(102, 0), 25, 25), surface, effect);
				}
				if (aMat[2][1] == lookFor)	//10
				{
					DrawSurface(drawOffset[1] + topLeft, RectI(Vei2(178, 0), 25, 25), surface, effect);
				}
				else if (FIDF(aMat[2][1], lookFor) && aMat[2][2] == lookFor)	// 6
				{
					DrawSurface(drawOffset[1] + topLeft, RectI(Vei2(127, 0), 25, 25), surface, effect);
				}
			}
			if (aMat[1][0] == lookFor)
			{
				if (aMat[0][1] == lookFor)	// 3
				{
					DrawSurface(drawOffset[2] + topLeft, RectI(Vei2(51, 25), 25, 25), surface, effect);
				}
				else if (aMat[0][1] != lookFor)	// 15
				{
					DrawSurface(drawOffset[2] + topLeft, RectI(Vei2(204, 25), 25, 25), surface, effect);
				}
				if (aMat[2][1] == lookFor)	// 4
				{
					DrawSurface(drawOffset[3] + topLeft, RectI(Vei2(76, 25), 25, 25), surface, effect);
				}
				else if (aMat[2][1] != lookFor)	// 16
				{
					DrawSurface(drawOffset[3] + topLeft, RectI(Vei2(229, 25), 25, 25), surface, effect);
				}
			}
			else if (aMat[1][0] != lookFor)
			{
				if (FIDF(aMat[0][1], lookFor) && aMat[0][0] == lookFor)	// 7
				{
					DrawSurface(drawOffset[2] + topLeft, RectI(Vei2(102, 25), 25, 25), surface, effect);
				}
				else if (aMat[0][1] == lookFor)	// 11
				{
					DrawSurface(drawOffset[2] + topLeft, RectI(Vei2(153, 25), 25, 25), surface, effect);
				}
				if (FIDF(aMat[2][1], lookFor) && aMat[2][0] == lookFor)	// 8
				{
					DrawSurface(drawOffset[3] + topLeft, RectI(Vei2(127, 25), 25, 25), surface, effect);
				}
				else if (aMat[2][1] == lookFor)	// 12
				{
					DrawSurface(drawOffset[3] + topLeft, RectI(Vei2(178, 25), 25, 25), surface, effect);
				}
			}
		}
	}
	template <typename T>
	void DrawConnectionsNew(int lookFor, Vei2 topLeft, Matrix<int> aMat, std::vector<RectI> drawOffset, const Surface& surface, T effect)
	{
		using namespace Settings;
		assert(aMat.GetSize().x == 3 && aMat.GetSize().y == 3);

		if (FIDF(aMat[1][1], lookFor))
		{
			if (aMat[1][2] == lookFor)
			{
				if (aMat[0][1] == lookFor)	// 1
				{
					DrawSurface(drawOffset[0] + topLeft, RectI(Vei2(201, 0), 100, 100), surface, effect);
				}
				else if (aMat[0][1] != lookFor)	// 13
				{
					DrawSurface(drawOffset[0] + topLeft, RectI(Vei2(804, 0), 100, 100), surface, effect);
				}
				if (aMat[2][1] == lookFor)	// 2
				{
					DrawSurface(drawOffset[1] + topLeft, RectI(Vei2(301, 0), 100, 100), surface, effect);
				}
				else if (aMat[2][1] != lookFor)	// 14
				{
					DrawSurface(drawOffset[1] + topLeft, RectI(Vei2(904, 0), 100, 100), surface, effect);
				}
			}
			else if (aMat[1][2] != lookFor)
			{
				if (aMat[0][1] == lookFor)	//9
				{
					DrawSurface(drawOffset[0] + topLeft, RectI(Vei2(603, 0), 100, 100), surface, effect);
				}
				else if (/*FIDF(aMat[0][1], lookFor) &&*/ aMat[0][2] == lookFor)	// 5
				{
					DrawSurface(drawOffset[0] + topLeft, RectI(Vei2(402, 0), 100, 100), surface, effect);
				}
				if (aMat[2][1] == lookFor)	//10
				{
					DrawSurface(drawOffset[1] + topLeft, RectI(Vei2(703, 0), 100, 100), surface, effect);
				}
				else if (/*FIDF(aMat[2][1], lookFor) &&*/ aMat[2][2] == lookFor)	// 6
				{
					DrawSurface(drawOffset[1] + topLeft, RectI(Vei2(502, 0), 100, 100), surface, effect);
				}
			}
			if (aMat[1][0] == lookFor)
			{
				if (aMat[0][1] == lookFor)	// 3
				{
					DrawSurface(drawOffset[2] + topLeft, RectI(Vei2(201, 100), 100, 100), surface, effect);
				}
				else if (aMat[0][1] != lookFor)	// 15
				{
					DrawSurface(drawOffset[2] + topLeft, RectI(Vei2(804, 100), 100, 100), surface, effect);
				}
				if (aMat[2][1] == lookFor)	// 4
				{
					DrawSurface(drawOffset[3] + topLeft, RectI(Vei2(301, 100), 100, 100), surface, effect);
				}
				else if (aMat[2][1] != lookFor)	// 16
				{
					DrawSurface(drawOffset[3] + topLeft, RectI(Vei2(904, 100), 100, 100), surface, effect);
				}
			}
			else if (aMat[1][0] != lookFor)
			{
				if (aMat[0][1] == lookFor)	// 11
				{
					DrawSurface(drawOffset[2] + topLeft, RectI(Vei2(603, 100), 100, 100), surface, effect);
				}else if (/*FIDF(aMat[0][1], lookFor) && */aMat[0][0] == lookFor)	// 7
				{
					DrawSurface(drawOffset[2] + topLeft, RectI(Vei2(402, 100), 100, 100), surface, effect);
				}
				if (aMat[2][1] == lookFor)	// 12
				{
					DrawSurface(drawOffset[3] + topLeft, RectI(Vei2(703, 100), 100, 100), surface, effect);
				}else if (/*FIDF(aMat[2][1], lookFor) &&*/ aMat[2][0] == lookFor)	// 8
					{
						DrawSurface(drawOffset[3] + topLeft, RectI(Vei2(502, 100), 100, 100), surface, effect);
					}
			}
		}
	}
	/*
	template<typename E>
	void DrawSurface(RectI pos, RectI sourceR, RectI clip, const Surface& s, E effect)
	{
		for (int y = pos.top; y < pos.bottom; y++)
		{
			for (int x = pos.left; x < pos.right; x++)
			{
				if (PixelInFrame({ x,y }) && clip.top <= y && clip.bottom >= y && clip.left <= x && clip.right >= x)
				{
					int sPixelX = sourceR.left + ((float)(x - pos.left) / pos.GetWidth())  * sourceR.GetWidth();
					int sPixelY = sourceR.top  + ((float)(y - pos.top)  / pos.GetHeight()) * sourceR.GetHeight();
					Color sourceP = s.GetPixel(sPixelX, sPixelY);
					effect(x, y, sourceP, *this);
				}
			}
		}

	}
	*/
	template<typename E>
	void DrawSurface(RectI pos, RectI sourceR, float rad, const Surface& s, E effect)
	{

		float cosTheta = cos(rad);
		float sinTheta = sin(rad);
		RectI clip = GetScreenRect<int>();
		for (int y = 0; y < pos.GetHeight(); y++)
		{
			for (int x = 0; x < pos.GetWidth(); x++)
			{
				int rotY = y - pos.GetHeight() / 2;
				int rotX = x - pos.GetWidth() / 2;

				//if (PixelInFrame({ x + pos.left, y + pos.top }) && clip.top <= y && clip.bottom >= y && clip.left <= x && clip.right >= x)
				//{
				int sPixelX = (int)(sourceR.left + ((float)(x) / pos.GetWidth()) * sourceR.GetWidth());
				int sPixelY = (int)(sourceR.top + ((float)(y) / pos.GetHeight()) * sourceR.GetHeight());
				Color sourceP = s.GetPixel(sPixelX, sPixelY);

				int rotXmove = cosTheta * rotX - sinTheta * rotY;
				int rotYmove = sinTheta * rotX + cosTheta * rotY;

				effect(pos.left + rotXmove,pos.top + rotYmove, sourceP, *this);
				//}
			}
		}
	}
	template<typename E>
	void DrawSurface(RectI pos, const Surface& s, E effect, int n90rot = 0)
	{
		DrawSurface(pos, s.GetRect(), s, effect, n90rot);
	}
	template<typename E>
	void DrawSurface(RectI pos, RectI sourceR, const Surface& s, E effect, int n90rot=0)
	{
		if (pos.right < screenRect.left || pos.left > screenRect.right || pos.bottom < screenRect.top || pos.top > screenRect.bottom)
		{
			return;
		}
		pos.right--;
		pos.bottom--;

		RectI inFpos = pos;
		int skippedLeft = 0, skippedRight = 0, skippedTop = 0, skippedBottom = 0;
		if (inFpos.left < 0)
		{
			skippedLeft = std::abs(inFpos.left);
			inFpos.left = 0;
		}
		if (inFpos.right >= ScreenWidth)
		{
			skippedRight = inFpos.right - ScreenWidth;
			inFpos.right = ScreenWidth - 1;
		}
		if (inFpos.top < 0)
		{
			skippedTop = std::abs(inFpos.top);
			inFpos.top = 0;
		}
		if (inFpos.bottom >= ScreenHeight)
		{
			skippedBottom = inFpos.bottom - ScreenHeight;
			inFpos.bottom = ScreenHeight - 1;
		}

		n90rot %= 4;
		int drawWidth = inFpos.GetWidth();
		int drawHeight = inFpos.GetHeight();

		int sourceWidth = sourceR.GetWidth()-1;
		int sourceHeight = sourceR.GetHeight()-1;

		int posWidth = pos.GetWidth();
		int posHeight = pos.GetHeight();

		int sWidth = s.GetRect().GetWidth();
		int sHeight = s.GetRect().GetHeight();
		using namespace std;
		for (int y = 0; y < drawHeight + 1; y++)
		{
			int sPixelX = 0;
			int sPixelY = 0;
			if (n90rot == 0)
			{
				sPixelY = (int)(sourceR.top + ((float)(y + skippedTop) / (posHeight)) * sourceHeight + 0.5f);
			}
			else if (n90rot == 1)
			{
				sPixelX = (int)(sourceR.right - 1 - ((float)(y + skippedTop) / posWidth) * sourceWidth + 0.5f);
			}
			else if (n90rot == 2)
			{
				sPixelY = (int)(sourceR.bottom - 1 - ((float)(y + skippedTop) / posHeight) * sourceHeight + 0.5f);
			}
			else if (n90rot == 3)
			{
				sPixelX = (int)(sourceR.left + ((float)(y + skippedTop) / posWidth) * sourceWidth + 0.5f);
			}
			for (int x = 0; x < drawWidth + 1; x++)
			{
				assert(PixelInFrame({ x + inFpos.left, y + inFpos.top }));
				
				//if (PixelInFrame({ x + inFpos.left, y + inFpos.top }))
				//{

					if (n90rot == 0)
					{
						sPixelX = (int)(sourceR.left + ((float)(x + skippedLeft) / (posWidth)) * sourceWidth + 0.5f);
					}
					else if (n90rot == 1)
					{ 
						sPixelY = (int)(sourceR.top + ((float)(x + skippedLeft) / posHeight) * sourceHeight + 0.5f);
					} 
					else if (n90rot == 2)
					{
						sPixelX = (int)(sourceR.right - 1 - ((float)(x + skippedLeft) / posWidth) * sourceWidth + 0.5f);
					}
					else if (n90rot == 3)
					{
						sPixelY = (int)(sourceR.bottom - 1 -((float)(x + skippedLeft) / posHeight) * sourceHeight + 0.5f);
					}

					assert(sPixelX >= 0);
					assert(sPixelY >= 0);
					assert(sPixelX < s.GetRect().GetWidth());
					assert(sPixelY < s.GetRect().GetHeight());
					//if (sPixelX >= 0 && sPixelY >= 0 && sPixelX < sWidth && sPixelY < sHeight)
					//{
						Color sourceP = s.GetPixel(sPixelX, sPixelY);
						effect(x + inFpos.left, y + inFpos.top, sourceP, *this);
					//}
					//pixelPercX += percentX;
				//}
			}
		}
		for (int y = 0; y < 5; y++)
		{
			for (int x = 0; x < 5; x++)
			{
				if (PixelInFrame(Vei2(pos.left + x, pos.top + y)))
				{
					//PutPixel((int)pos.left + x, (int)pos.top + y, Colors::Black);
				}
			}
		}
	}
	template<typename E>
	void DrawText(std::string text,Vei2 pos, int size, const Font* font, E effect)
	{
		DrawText(text, pos.x, pos.y, size, font, effect);
	}
	template<typename E>
	void DrawText(std::string text, int x, int y, int size, const Font* font, E effect)
	{
		int xM = 0;
		int yM = 0;

		const std::vector<RectI>& cRects = font->GetCharRects();
		int first = font->GetFirst();
		int last = font->GetLast();
		const Surface& surface = font->GetSurface();

		//if (costumWidth)
		//{
		for (int i = 0; i < (int)text.length(); i++)
		{
			if (text[i] >= first && text[i] <= last)
			{
				float ratio = (float)cRects[(__int64)text[i] - first].GetWidth() / cRects[(__int64)text[i] - first].GetHeight();
				Vec2 charSize(ratio * size, (float)size);
				DrawSurface(RectI(Vei2(x + xM, y), (int)charSize.x, (int)charSize.y), cRects[(__int64)text[i] - first], surface, effect);
				xM += (int)((float)size * 0.9f * ratio);
			}
			if (text[i] == ' ')
			{
				xM += (int)(size * 0.7f);
			}
		}
	}
	template<typename E>
	void DrawTextCentered(std::string text, Vei2 pos, int size, const Font* font, E effect)
	{
		DrawTextCentered(text, pos.x, pos.y, size, font, effect);
	}
	template<typename E>
	void DrawTextCentered(std::string text, int x, int y, int size, const Font* font, E effect)
	{
		int xM = 0;
		int yM = 0;

		const std::vector<RectI>& cRects = font->GetCharRects();
		int first = font->GetFirst();
		int last = font->GetLast();
		const Surface& surface = font->GetSurface();

		int width = 0;
		for (int i = 0; i < (int)(text.length()); i++)
		{
			if (text[i] >= first && text[i] <= last)
			{
				float ratio = (float)cRects[(__int64)text[i] - first].GetWidth() / cRects[(__int64)text[i] - first].GetHeight();
				width += (__int64)(size * 0.9f * ratio);
			}
			if (text[i] == ' ')
			{
				width += (int)(size * 0.7f);
			}
		}
		x -= width / 2;
		y -= size / 2;
		for (int i = 0; i < (int)text.length(); i++)
		{
			if (text[i] >= first && text[i] <= last)
			{
				float ratio = (float)cRects[(__int64)text[i] - first].GetWidth() / cRects[(__int64)text[i] - first].GetHeight();
				Vec2 charSize(ratio * size, (float)size);
				DrawSurface(RectI(Vei2(x + xM, y), (int)charSize.x, (int)charSize.y), cRects[(__int64)text[i] - first], surface, effect);
				xM += (__int64)(size * 0.9f * ratio);
			}
			if (text[i] == ' ')
			{
				xM += (int)(size * 0.7f);
			}
		}
	}
	~Graphics();
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color* pSysBuffer = nullptr;

	std::vector<Color*> pSysBackBuffer;
	

public:
	static constexpr int ScreenWidth = 1280;
	static constexpr int ScreenHeight = 720;
	static Vei2 GetMidOfScreen() { return Vei2(ScreenWidth / 2, ScreenHeight / 2); };
	template<typename T>
	const static Rect_<T> GetScreenRect() { return Rect_<T>(Vec2_<T>(0, 0), ScreenWidth, ScreenHeight); };
	template<typename T>
	const static Vec2_<T> GetScreenVec2_() { return Vec2_<T>(ScreenWidth, ScreenHeight); };
	RectI screenRect = GetScreenRect<int>();

};