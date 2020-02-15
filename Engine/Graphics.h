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
	bool PixelInFrame(Vei2 check) { return GetScreenRect<int>().Contains(check); }
	template<typename E>
	void DrawSurface(Vei2 pos, const Surface& s, Color c, E effect)
	{
		DrawSurface(pos, Graphics::GetScreenRect<int>(), s, c,effect);
	}
	template<typename E>
	void DrawSurface(Vei2 pos, RectI sourceR, RectI clip, const Surface& s, Color c, E effect)
	{
		if (sourceR == RectI(Vei2(0, 0), 0, 0))
		{
			sourceR = s.GetRect();
		}

		if (sourceR.GetWidth() + pos.x > ScreenWidth)
		{
			sourceR.right -= sourceR.GetWidth() + pos.x - ScreenWidth;
		}
		if (sourceR.GetHeight() + pos.y > ScreenHeight)
		{
			sourceR.bottom -= sourceR.GetHeight() + pos.y - ScreenHeight;
		}

		if (pos.x < 0)
		{
			sourceR.left += std::abs(pos.x);
			pos.x += std::abs(pos.x);
		}
		if (pos.y < 0)
		{
			sourceR.top += std::abs(pos.y);
			pos.y += std::abs(pos.y);
		}
			for (int y = sourceR.top; y < sourceR.bottom; y++)
			{
				for (int x = sourceR.left; x < sourceR.right; x++)
				{
					int xOnFrame = x + pos.x - sourceR.left;
					int yOnFrame = y + pos.y - sourceR.top;
					assert(PixelInFrame({ xOnFrame,yOnFrame }));
						//Color c = s.GetPixel(((float)x / drawPos.GetWidth()) * s.GetWidth(), ((float)y / drawPos.GetHeight()) * s.GetHeight());
						Color c = s.GetPixel(x, y);
						effect(xOnFrame, yOnFrame, c, *this);
					//}
				}
			}
	}
	/*
		if (sourceR == RectI(Vei2(0, 0), 0, 0))
		{
			sourceR = s.GetRect();
		}

		RectI drawPos = pos;
		RectI drawPC = drawPos;
		Vec2 ratio = { (float)pos.GetWidth() / sourceR.GetWidth(),(float)pos.GetHeight() / sourceR.GetHeight() };

		RectF RemChanges = { 0,0,0,0 };
		*/
	template<typename E>
	void DrawSurface(Vei2 pos, const Surface& s, E effect, Color c = NULL, bool pretty = pretty)
	{
		DrawSurface(RectI(pos, s.GetWidth(), s.GetHeight()), s.GetRect(), GetScreenRect<int>(), s, effect, c, pretty);
	}
	template<typename E>
	void DrawSurface(RectI pos, const Surface& s, E effect, Color c = NULL, bool pretty = pretty)
	{
		DrawSurface(pos, s.GetRect(), GetScreenRect<int>(), s, effect, c, pretty)
	}
	template<typename E>
	void DrawSurface(RectI pos, RectI sourceR, const Surface& s, E effect, Color c = NULL, bool pretty = pretty)
	{
		DrawSurface(pos, sourceR, GetScreenRect<int>(), s, effect, c, pretty)
	}
	template<typename E>
	void DrawSurface(RectI pos, RectI sourceR, RectI clip, const Surface& s, E effect, Color c = NULL, bool pretty = pretty)
	{
		if (pretty)
		{
			DrawSurfacePretty(pos, sourceR, clip, s, effect, c);
		}
		else
		{
			DrawSurfaceQuick(pos, sourceR, clip, s, effect, c);
		}
	}
	template<typename E>
	void DrawSurfaceQuick(RectI pos, RectI sourceR, RectI clip, const Surface& s, E effect, Color c = NULL)
	{
		RectI sourceRcopy = sourceR;
		if (pos.left < clip.left)
		{
			int delta = (int)clip.left - pos.left;
			float shorter = (float)delta / pos.GetWidth();
			sourceR.left += (int)(shorter * sourceR.GetWidth());
			pos.left = clip.left;
			if (pos.GetWidth() <= 0 || pos.GetHeight() <= 0 || sourceR.GetWidth() <= 0 || sourceR.GetHeight() <= 0) {
				return;
			}
		}
		if (pos.right > clip.right)
		{
			int delta = pos.right - clip.right;
			float shorter = (float)delta / pos.GetWidth();
			sourceR.right -= std::round(shorter * sourceR.GetWidth());
			pos.right = clip.right;
			if (pos.GetWidth() <= 0 || pos.GetHeight() <= 0 || sourceR.GetWidth() <= 0 || sourceR.GetHeight() <= 0) {
				return;
			}
		}
		if (pos.top < clip.top)
		{
			int delta = clip.top - pos.top;
			float shorter = (float)delta / pos.GetHeight();
			sourceR.top += (int)(shorter * sourceR.GetHeight());
			pos.top = clip.top;
			if (pos.GetWidth() <= 0 || pos.GetHeight() <= 0 || sourceR.GetWidth() <= 0 || sourceR.GetHeight() <= 0) {
				return;
			}
		}
		if (pos.bottom > clip.bottom)
		{
			int delta = pos.bottom - clip.bottom;
			float shorter = (float)delta / pos.GetHeight();
			sourceR.bottom -= (int)(shorter * sourceR.GetHeight());
			pos.bottom = (int)clip.bottom;
			if (pos.GetWidth() <= 0 || pos.GetHeight() <= 0 || sourceR.GetWidth() <= 0 || sourceR.GetHeight() <= 0) {
				return;
			}
		}
		if (pos.GetWidth() > 0 && pos.GetHeight() > 0 && sourceR.GetWidth() > 0 && sourceR.GetHeight() > 0){
			for (int y = pos.top; y < pos.bottom; y++)
			{
				for (int x = pos.left; x < pos.right; x++)
				{
					assert(PixelInFrame({ x,y }));
					
					int xPixel = sourceR.left + std::round(((float)(x - pos.left) / pos.GetWidth()) * sourceR.GetWidth()) ;
					int yPixel = sourceR.top + std::round(((float)(y - pos.top) / pos.GetHeight()) * sourceR.GetHeight());
					Color col = s.GetPixel(xPixel, yPixel);
					if (c == NULL)
					{
						effect(x, y, col, col, *this);
					}
					else
					{
						effect(x, y, col, c, *this);
					}
				}
			}
		}
	}
	template<typename E>
	void DrawSurfacePretty(RectI pos, RectI sourceR, RectI clip, const Surface& s, E effect, Color c = NULL)
	{
			for (int y = pos.top; y < pos.bottom; y++)
			{
				for (int x = pos.left; x < pos.right; x++)
				{
					if (PixelInFrame({ x,y }) && clip.top <= y && clip.bottom >= y && clip.left <= x && clip.right >= x)
					{
						int xPixel = sourceR.left + std::round(((float)(x - pos.left) / pos.GetWidth()) * sourceR.GetWidth());
						int yPixel = sourceR.top + std::round(((float)(y - pos.top) / pos.GetHeight()) * sourceR.GetHeight());
						Color col = s.GetPixel(xPixel, yPixel);
						if (c == NULL)
						{
							effect(x, y, col, col, *this);
						}
						else
						{
							effect(x, y, col, c, *this);
						}
					}
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
public:
	static constexpr int ScreenWidth = 800;
	static constexpr int ScreenHeight = 600;
	static constexpr bool pretty = true;
	static Vei2 GetMidOfScreen() { return Vei2(ScreenWidth / 2, ScreenHeight / 2); };
	template<typename T>
	const static Rect_<T> GetScreenRect() { return Rect_<T>(Vec2_<T>(0, 0), ScreenWidth, ScreenHeight); };
	template<typename T>
	const static Vec2_<T> GetScreenVec2() { return Vec2_<T>(ScreenWidth, ScreenHeight); };

};