#ifndef VEC2_H
#define VEC2_H

class vec2
{
public:
	float x;
	float y;
public:
	vec2() : x(0.0), y(0.0) {}
	vec2(float a) : x(a), y(a) {}
	vec2(float a, float b) : x(a), y(b) {}

};
inline vec2 operator*(const vec2& u, float t)
{
	return vec2(u.x * t, u.y * t);
}
inline vec2 operator*(float t, const vec2& u)
{
	return u * t;
}
inline vec2 operator/(const vec2& u, float t)
{
	return vec2(u.x / t, u.y / t);
}



inline vec2 operator+(const vec2& u, const vec2& v)
{
	return vec2(u.x + v.x, u.y + v.y);
}
inline vec2 operator-(const vec2& u, const vec2& v)
{
	return vec2(u.x - v.x, u.y - v.y);
}
inline float dot(const vec2& u, const vec2& v)
{
	return u.x * v.x + u.y * v.y;
}

#endif
