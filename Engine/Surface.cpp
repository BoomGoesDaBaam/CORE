#include "Surface.h"
#include "ChiliWin.h"
#include <cassert>
#include <fstream>
#include <algorithm>

Surface::Surface(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);
	assert(file);

	BITMAPFILEHEADER bmFileHeader;
	file.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));

	BITMAPINFOHEADER bmInfoHeader;
	file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));

	assert(bmInfoHeader.biBitCount == 24 || bmInfoHeader.biBitCount == 32);
	assert(bmInfoHeader.biCompression == BI_RGB);

	const bool is32b = bmInfoHeader.biBitCount == 32;

	width = bmInfoHeader.biWidth;

	// test for reverse row order and control
	// y loop accordingly
	int yStart;
	int yEnd;
	int dy;
	if (bmInfoHeader.biHeight < 0)
	{
		height = -bmInfoHeader.biHeight;
		yStart = 0;
		yEnd = height;
		dy = 1;
	}
	else
	{
		height = bmInfoHeader.biHeight;
		yStart = height - 1;
		yEnd = -1;
		dy = -1;
	}

	pixels.resize(width * height);

	file.seekg(bmFileHeader.bfOffBits);
	// padding is for the case of of 24 bit depth only
	const int padding = (4 - (width * 3) % 4) % 4;

	for (int y = yStart; y != yEnd; y += dy)
	{
		for (int x = 0; x < width; x++)
		{
			PutPixel(x, y, Color(file.get(), file.get(), file.get()));
			if (is32b)
			{
				file.seekg(1, std::ios::cur);
			}
		}
		if (!is32b)
		{
			file.seekg(padding, std::ios::cur);
		}
	}
}

Surface::Surface(int width, int height)
	:
	width(width),
	height(height),
	pixels(width* height)
{}

Surface::Surface(Surface&& donor)
{
	*this = std::move(donor);
}

Surface& Surface::operator=(Surface&& rhs)
{
	width = rhs.width;
	height = rhs.height;
	pixels = std::move(rhs.pixels);
	rhs.width = 0;
	rhs.height = 0;
	return *this;
}

int Surface::GetWidth() const
{
	return width;
}

int Surface::GetHeight() const
{
	return height;
}

RectI Surface::GetRect() const
{
	return{ 0,width,0,height };
}
Vei2 Surface::GetSize() const
{
	return Vei2(width,height);
}
const Color* Surface::Data() const
{
	return pixels.data();
}
bool Surface::IsInBounds(int x, int y)const
{
	return y * width + x < (int)pixels.size();
}
Surface Surface::GetSupSurface(RectI where)
{
	assert(where.left >= 0 && where.GetWidth() <= width);
	assert(where.top >= 0 && where.GetHeight() <= height);

	Surface sup = { where.GetWidth(),where.GetHeight() };
	for (int y = 0; y < sup.GetHeight(); y++)
	{
		for (int x = 0; x < sup.GetWidth(); x++)
		{
			sup.PutPixel(x, y, pixels.at(where.top * width + y * width + x + where.left));
		}
	}
	return sup;
}
void Surface::PutPixel(int x, int y, Color c)
{
	pixels[y * width + x] = c;
}
Color Surface::GetPixel(int x, int y) const
{
	assert(IsInBounds(x, y));
	return pixels[(__int64)y * width + x];
}
Color Surface::GetPixel(Vei2 pos) const
{
	return GetPixel(pos.x, pos.y);
}
void Surface::Fill(Color c)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			pixels[y * width + x] = c;
		}
	}
}
Matrix<int> Surface::GetChromaMatrix(Color chroma)const
{
	Matrix<int> matrix = Matrix<int>(width, height, 0);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (pixels[y * width + x].dword != chroma.dword)
			{
				matrix[x][y] = 1;
			}
		}
	}
	return matrix;
}
bool Surface::TestIfHitOnScreen(Vec2 mP)const
{
	if (RectI(Vei2(0, 0), width, height).Contains((Vei2)mP))
	{
		Matrix<int> chromaM = GetChromaMatrix(Colors::Magenta);
		return chromaM((Vei2)mP) == 1;
	}
	return false;

	//Vei2 surSize = GetSize() + Vei2(-1,-1);
	/*
	if (size != Vei2(-1, -1))
	{
		mP.x *= surSize.x / size.x;
		mP.y *= surSize.y / size.y;
	}
	*/
	//Matrix<int> chromaM = GetChromaMatrix(Colors::Magenta);
	//return Vec2::IsPositivFactor(Vec2((float)mP.x / width, (float)mP.y / height)) && chromaM((Vei2)mP) == 1; //chromaM(Vei2((int)(((float)mP.x / width) * surSize.x),(int)( ((float)mP.y / height) * surSize.y))) == 1;
}