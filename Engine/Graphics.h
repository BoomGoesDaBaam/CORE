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
#include "GigaMath.h"
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
	void RainbowPutPixel(int x, int y);

	bool PixelInFrame(Vei2 check) { return GetScreenRect<int>().Contains(check); }
	void DrawCircle(int x, int y, float radius, Color c);
	void DrawCircle(int x, int y, float outerRadius,float innerRadius, Color innerC, Color outerC);
	void DrawLine(Vec2 p1, Vec2 p0, Color c, int thickness = 1);
	template<typename E>
	void DrawLine(Vec2 p1, Vec2 p0, E effect, int thickness = 1)		//
	{
		float m = 0.0f;
		if (p1.x != p0.x)
		{
			m = (p1.y - p0.y) / (p1.x - p0.x);
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
	void DrawRect(Vec2 pos, Vec2 size, Color c, float radiant = 0);
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

				int rotXmove = cosTheta * x - sinTheta * y;
				int rotYmove = sinTheta * x + cosTheta * y;

				effect(rotX + pos.left + rotXmove, y + pos.top + rotYmove, sourceP, *this);
				//}
			}
		}
	}
	template<typename E>
	void DrawSurface(RectI pos, RectI sourceR, const Surface& s, E effect)
	{
		DrawSurface(pos, sourceR, GetScreenRect<int>(), s, effect, 0);
	}
	template<typename E>
	void DrawSurface(RectI pos, RectI sourceR, RectI clip, const Surface& s, E effect, int n90rot=0)
	{
		for (int y = 0; y < pos.GetHeight(); y++)
		{
			for (int x = 0; x < pos.GetWidth(); x++)
			{
				if (PixelInFrame({ x + pos.left, y + pos.top }) && clip.top <= y && clip.bottom >= y && clip.left <= x && clip.right >= x)
				{
					int sPixelX=0;
					int sPixelY=0;
					sPixelX =(int)(sourceR.left + ((float)(x) / pos.GetWidth()) * sourceR.GetWidth());
					sPixelY = (int)(sourceR.top + ((float)(y) / pos.GetHeight()) * sourceR.GetHeight());
					Color sourceP = s.GetPixel(sPixelX, sPixelY);
					effect(x + pos.left, y + pos.top, sourceP, *this);

					/*
					if (n90rot == 0)
					{
						sPixelX = sourceR.left + ((float)(x) / pos.GetWidth()) * sourceR.GetWidth();
						sPixelY = sourceR.top + ((float)(y) / pos.GetHeight()) * sourceR.GetHeight();
					}
					if (n90rot == 1)
					{ 
						sPixelX = sourceR.left + ((float)(y) / pos.GetHeight()) * sourceR.GetHeight();
						sPixelY = sourceR.top - 1 - ((float)(x) / pos.GetWidth()) * sourceR.GetWidth();
					} 
					if (n90rot == 2)
					{
						sPixelX = sourceR.left + ((float)(x) / pos.GetWidth()) * sourceR.GetWidth();
						sPixelY = std::ceil(sourceR.bottom - 1 - ((float)(y) / pos.GetHeight()) * sourceR.GetHeight());
					}
					if (n90rot == 3)
					{
						sPixelX = sourceR.top + ((float)(y) / pos.GetHeight()) * sourceR.GetHeight();
						sPixelY = sourceR.bottom - ((float)(x) / pos.GetWidth()) * sourceR.GetWidth();
					}
					Color sourceP = s.GetPixel(sPixelX, sPixelY);
					effect(x + pos.left, y + pos.top, sourceP, *this);
					*/
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
	static Vei2 GetMidOfScreen() { return Vei2(ScreenWidth / 2, ScreenHeight / 2); };
	template<typename T>
	const static Rect_<T> GetScreenRect() { return Rect_<T>(Vec2_<T>(0, 0), ScreenWidth, ScreenHeight); };
	template<typename T>
	const static Vec2_<T> GetScreenVec2() { return Vec2_<T>(ScreenWidth, ScreenHeight); };

};