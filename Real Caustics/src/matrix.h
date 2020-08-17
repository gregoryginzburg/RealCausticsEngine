#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include "vec3.h"
#include "vec4.h"

using std::sin;
using std::cos;

extern const float PI;

class matrix_3x3
{
public:
	vec3 i;
	vec3 j;
	vec3 k;
public:
	matrix_3x3(vec3 ii, vec3 jj, vec3 kk) : i(ii), j(jj), k(kk) {}
	// rotation matrix in cartesian coordinates (radians)
	matrix_3x3(vec3 r)
	{
		// r.x = z, r.y = y, r.z = x
		i = vec3(cos(r.z) * cos(r.y),
			sin(r.z) * cos(r.y),
			-sin(r.y));
		j = vec3(cos(r.z) * sin(r.y) * sin(r.x) - sin(r.z) * cos(r.x),
			sin(r.z) * sin(r.y) * sin(r.x) + cos(r.z) * cos(r.x),
			cos(r.z) * sin(r.x));
		k = vec3(cos(r.z) * sin(r.y) * cos(r.x) + sin(r.z) * sin(r.x),
			sin(r.z) * sin(r.y) * cos(r.x) - cos(r.z) * sin(r.x),
				cos(r.y) * cos(r.x));
	}
		
	
	// rotation matrix from angle and axis (radians)
	matrix_3x3(float a, vec3 u)
	{
		float c = cos(a);
		float s = sin(a);
		float t = 1 - c;
		i = vec3(c + u.x * u.x * t, u.x * u.y * t + u.z * s, u.x * u.z * t - u.y * s);
		j = vec3(u.x * u.y * t - u.z * s, c + u.y * u.y * t, u.y * u.z * t + u.x * s);
		k = vec3(u.x * u.z * t + u.y * s, u.y * u.z * t - u.x * s, c + u.z * u.z * t);
	}
public:
	float& operator()(int idx1, int idx2)
	{
		if (idx1 == 0)
		{
			if (idx2 == 0)
			{
				return i.x;
			}
			else if (idx2 == 1)
			{
				return i.y;
			}
			else
			{
				return i.z;
			}
			
		}
		else if (idx1 == 1)
		{
			if (idx2 == 0)
			{
				return j.x;
			}
			else if (idx2 == 1)
			{
				return j.y;
			}
			else
			{
				return j.z;
			}
		}
		else
		{
			if (idx2 == 0)
			{
				return k.x;
			}
			else if (idx2 == 1)
			{
				return k.y;
			}
			else
			{
				return k.z;
			}
		}
		
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
inline void rotate_vec(vec3& v, vec3 p, vec3 rotation)
{
	matrix_3x3 rot_matrix(vec3(rotation.x * PI / 180.f, rotation.y * PI / 180.f, rotation.z * PI / 180.f));
	v = (v - p) * rot_matrix + p;
}
//rotation in radians
inline void rotate_vec(vec3& v, vec3& p, float angle, vec3 axis)
{
	matrix_3x3 rot_matrix(angle, axis);
	v = (v - p) * rot_matrix + p;
}
// not optimized at all - only used to invert 1 matrix to transform 8 bounding box points
inline void invert_matrix(matrix_3x3& m)
{
	float det = m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) -
				m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
				m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));

	float inv_det = 1.0f / det;

	m(0, 0) = (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) * inv_det;
	m(0, 1) = (m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2)) * inv_det;
	m(0, 2) = (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * inv_det;
	m(1, 0) = (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) * inv_det;
	m(1, 1) = (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) * inv_det;
	m(1, 2) = (m(1, 0) * m(0, 2) - m(0, 0) * m(1, 2)) * inv_det;
	m(2, 0) = (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)) * inv_det;
	m(2, 1) = (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) * inv_det;
	m(2, 2) = (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) * inv_det;
}













#endif
