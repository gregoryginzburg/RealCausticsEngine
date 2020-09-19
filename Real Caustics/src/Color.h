#ifndef COLOR_H
#define COLOR_H


class colorf
{
public:
	float r;
	float g;
	float b;
public:
	colorf() : r(0.f), g(0.f), b(0.f) {}
	colorf(float c) : r(c), g(c), b(c) {}
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
inline colorf operator*(const colorf& a, float t)
{
	return colorf(a.r * t, a.g * t, a.b * t);
}
inline colorf operator*(float t, const colorf& a)
{
	return a * t;
}

inline colorf operator/(const colorf& a, float t)
{
	return colorf(a.r / t, a.g / t, a.b / t);
}
inline colorf operator/(float t, const colorf& a)
{
	return a / t;
}

inline colorf operator+(const colorf& a, float t)
{
	return colorf(a.r + t, a.g + t, a.b + t);
}

inline colorf operator+(float t, const colorf& a)
{
	return a + t;
}

#endif
