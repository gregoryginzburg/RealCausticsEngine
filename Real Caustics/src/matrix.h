#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include "vec3.h"
#include "vec4.h"

using std::sinf;
using std::cosf;

extern const float PI;

class matrix_3x3
{
public:
	vec3 i;
	vec3 j;
	vec3 k;
public:
	matrix_3x3(vec3 ii, vec3 jj, vec3 kk) : i(ii), j(jj), k(kk) {}
	matrix_3x3(vec3 r)
	{
		i = vec3(cosf(r.x) * cosf(r.y), sinf(r.x) * cosf(r.y), -sinf(r.y));
		j = vec3(cosf(r.x) * sinf(r.y) * sinf(r.z) - sinf(r.x) * cosf(r.z), sinf(r.x) * sinf(r.y) * sinf(r.z) + cosf(r.x) * cosf(r.z), cosf(r.x) * sinf(r.z));
		k = vec3(cosf(r.x) * sinf(r.y) * cosf(r.z) + sinf(r.x) * sinf(r.z), sinf(r.x) * sinf(r.y) * cosf(r.z) - cosf(r.x) * sinf(r.z), cosf(r.y) * cosf(r.z));
	}
	matrix_3x3(float a, vec3 u)
	{
		float c = cosf(a);
		float s = sinf(a);
		float t = 1 - c;
		i = vec3(c + u.x * u.x * t, u.x * u.y * t + u.z * s, u.x * u.z * t - u.y * s);
		j = vec3(u.x * u.y * t - u.z * s, c + u.y * u.y * t, u.y * u.z * t + u.x * s);
		k = vec3(u.x * u.z * t + u.y * s, u.y * u.z * t - u.x * s, c + u.z * u.z * t);
	}
};
class matrix_4x4
{
public:
	vec4 i;
	vec4 j;
	vec4 k;
	vec4 w;
public:
	matrix_4x4(vec4 ii, vec4 jj, vec4 kk, vec4 ww) : i(ii), j(jj), k(kk), w(ww) {}
};


inline vec3 operator*(const vec3& v, const matrix_4x4& m)
{
	vec4 temp;
	temp = v.x * m.i + v.y * m.j + v.z * m.k + m.w;
	return vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);
}
inline vec3 operator*(const matrix_4x4& m, const vec3& v)
{
	return v * m;
}
inline vec3 operator*(const vec3& v, const matrix_3x3& m)
{
	return v.x * m.i + v.y * m.j + v.z * m.k;
}
inline vec3 operator*(const matrix_3x3& m, const vec3& v)
{
	return v * m;
}
//rotation in degrees
inline void rotate_vec(vec3& v, vec3& rotation)
{
	matrix_3x3 rot_matrix(vec3(rotation.x * PI / 180.f), rotation.y * PI / 180.f, rotation.z * PI / 180.f);
	v = v * rot_matrix;
}
//rotation in radians
inline void rotate_vec(vec3& v, double angle, vec3 axis)
{
	matrix_3x3 rot_matrix(angle, axis);
	v = v * rot_matrix;;
}













#endif
