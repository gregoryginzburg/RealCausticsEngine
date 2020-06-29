#ifndef MATRIX_H
#define MATRIX_H
#include "vec3.h"
#include "vec4.h"
#include <cmath>

class matrix_3x3
{
public:
	vec3 i;
	vec3 j;
	vec3 k;
public:
	matrix_3x3(vec3 ii, vec3 jj, vec3 kk) : i(ii), j(jj), k(kk) {}

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













#endif