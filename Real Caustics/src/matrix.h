#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include "vec3.h"
#include "vec4.h"
using std::sin;
using std::cos;

extern const double PI;

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
		i = vec3(cos(r.x) * cos(r.y), sin(r.x) * cos(r.y), -sin(r.y));
		j = vec3(cos(r.x) * sin(r.y) * sin(r.z) - sin(r.x) * cos(r.z), sin(r.x) * sin(r.y) * sin(r.z) + cos(r.x) * cos(r.z), cos(r.x) * sin(r.z));
		k = vec3(cos(r.x) * sin(r.y) * cos(r.z) + sin(r.x) * sin(r.z), sin(r.x) * sin(r.y) * cos(r.z) - cos(r.x) * sin(r.z), cos(r.y) * cos(r.z));
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
inline void rotate_vec(vec3& v, vec3& rotation)
{
	matrix_3x3 rot_matrix(vec3(rotation.x * PI / 180.), rotation.y * PI / 180., rotation.z * PI / 180.);
	v = v * rot_matrix;
}













#endif
