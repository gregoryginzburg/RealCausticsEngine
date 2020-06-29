#ifndef VEC2_H
#define VEC2_H

class vec2
{
public:
	double x;
	double y;
public:
	vec2() : x(0.0), y(0.0) {}
	vec2(double a) : x(a), y(a) {}
	vec2(double a, double b) : x(a), y(b) {}

};
inline vec2 operator*(const vec2& u, double t)
{
	return vec2(u.x * t, u.y * t);
}
inline vec2 operator*(double t, const vec2& u)
{
	return u * t;
}
inline vec2 operator+(const vec2& u, const vec2& v)
{
	return vec2(u.x + v.x, u.y + v.y);
}

#endif
