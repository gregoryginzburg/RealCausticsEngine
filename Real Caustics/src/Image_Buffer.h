#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H
#include <vector>
#include "Color.h"

class Image_Buffer
{
public:
	Image_Buffer()
	{

	}
	Image_Buffer(int resX, int resY) : ResX(resX), ResY(resY)
	{
		color.resize(resX * resY);
	}
public:
	void init(int resX, int resY)
	{
		ResX = resX;
		ResY = resY;
		color.resize(resX * resY);
	};
	void add_color(const vec2& sample, const vec3& aColor)
	{
		if (sample.x < 0 || sample.x >= (float)ResX)
			return;
		if (sample.y < 0 || sample.y >= (float)ResY)
			return;

		int X = (int)sample.x;
		int Y = ResY - (int)sample.y;
		if (X + Y * ResX > color.size() - 1)
			return;

		color[X + Y * ResX] += aColor;
	}
	void scale(float aScale)
	{
		for (size_t i = 0; i < color.size(); ++i)
		{
			color[i] *= aScale;
		}
	}
	void save_as_hdr(const char* path)
	{
		
	}
public:
	std::vector<vec3> color;
	int ResX;
	int ResY;
};



#endif
