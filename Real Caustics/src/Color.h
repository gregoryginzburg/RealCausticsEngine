#ifndef COLOR_H
#define COLOR_H

class color
{
public:
	short int r;
	short int g;
	short int b;
public:
	color() : r(0), g(0), b(0) {}
	color(short int c) : r(c), g(c), b(c) {}
	color(short int c0, short int c1, short int c2) : r(c0), g(c1), b(c2) {}

	color& operator+=(const color& other)
	{
		r += other.r;
		g += other.g;
		b += other.b;
		return *this;
	}

};
class colorf
{
public:
	float r;
	float g;
	float b;
public:
	colorf() : r(0.f), g(0.f), b(0.f) {}
	colorf(float red, float green, float blue) : r(red), g(green), b(blue) {}
public:
	inline colorf& operator*=(const colorf& other)
	{
		r *= other.r;
		g *= other.g;
		b *= other.b;
		return *this;
	}
	inline colorf& operator/=(float t)
	{
		r /= t;
		g /= t;
		b /= t;
		return *this;
	}
	inline colorf& operator+=(const colorf& other)
	{
		r += other.r;
		g += other.g;
		b += other.b;
		return *this;
	}
};
inline colorf operator*(const colorf& a, const colorf& b)
{
	return colorf(a.r * b.r, a.g * b.g, a.b * b.b);
}
inline color clamp(color& c)
{
	if (c.r > 255)
	{
		c.r = 255;
	}
	if (c.g > 255)
	{
		c.g = 255;
	}
	if (c.b > 255)
	{
		c.b = 255;
	}
	if (c.r < 0)
	{
		c.r = 0;
	}
	if (c.g < 0)
	{
		c.g = 0;
	}
	if (c.b < 0)
	{
		c.b = 0;
	}
	return c;
}

#endif
