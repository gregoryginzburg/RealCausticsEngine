#ifndef MATRIX_H
#define MATRIX_H

#include <string.h>
#include "vec3.h"

class Matrix4x4
{
public:
	Matrix4x4()
	{
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
		m[0][1] = m[0][2] = m[0][3] = m[1][0] =
			m[1][2] = m[1][3] = m[2][0] = m[2][1] = m[2][3] =
			m[3][0] = m[3][1] = m[3][2] = 0.0f;
	};
	Matrix4x4(float t00, float t01, float t02, float t03,
		float t10, float t11, float t12, float t13,
		float t20, float t21, float t22, float t23,
		float t30, float t31, float t32, float t33)
	{
		m[0][0] = t00;
		m[0][1] = t01;
		m[0][2] = t02;
		m[0][3] = t03;
		m[1][0] = t10;
		m[1][1] = t11;
		m[1][2] = t12;
		m[1][3] = t13;
		m[2][0] = t20;
		m[2][1] = t21;
		m[2][2] = t22;
		m[2][3] = t23;
		m[3][0] = t30;
		m[3][1] = t31;
		m[3][2] = t32;
		m[3][3] = t33;

	};
	
	Matrix4x4(float mat[4][4]) 
	{
		memcpy(m, mat, 16 * sizeof(float));
	}

public:
	float m[4][4];
};


Matrix4x4 Inverse(const Matrix4x4& mat);



class matrix_3x3
{
public:
	vec3 i;
	vec3 j;
	vec3 k;
public:
	matrix_3x3(vec3 ii, vec3 jj, vec3 kk);
	// rotation matrix in cartesian coordinates (radians)
	matrix_3x3(vec3 r);
	// rotation matrix from angle and axis (radians)
	matrix_3x3(float a, vec3 u);

};/*
class matrix_4x4
{
public:
	vec4 i;
	vec4 j;
	vec4 k;
	vec4 w;
public:
	matrix_4x4() {}
	matrix_4x4(vec4 ii, vec4 jj, vec4 kk, vec4 ww);
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
}*/
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
inline void rotate_vec_rad(vec3& v, vec3 p, vec3 rotation)
{
	matrix_3x3 rot_matrix(rotation);
	v = (v - p) * rot_matrix + p;
}
//rotation in radians
inline void rotate_vec(vec3& v, vec3& p, float angle, vec3 axis)
{
	matrix_3x3 rot_matrix(angle, axis);
	v = (v - p) * rot_matrix + p;
}
// not optimized at all - only used to invert 1 matrix to transform 8 bounding box points
/*
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
}*/
inline void transpose(matrix_3x3& mat)
{
	vec3 i = vec3(mat.i.x, mat.j.x, mat.k.x);
	vec3 j = vec3(mat.i.y, mat.j.y, mat.k.y);
	vec3 k = vec3(mat.i.z, mat.j.z, mat.k.z);
	mat.i = i;
	mat.j = j;
	mat.k = k;
}



class PythonMatrix4x4
{
public:
	float t00;
	float t01;
	float t02;
	float t03;
	float t10;
	float t11;
	float t12;
	float t13;
	float t20;
	float t21;
	float t22;
	float t23;
	float t30;
	float t31;
	float t32;
	float t33;
};	


#endif 
