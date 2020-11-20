#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "vec3.h"
#include "matrix.h"

class Transform
{
public:
	Transform() {}

	Transform(const Matrix4x4& m) : mat(m)
	{
		inv_mat = Inverse(m);
	}

	Transform(const Matrix4x4& m, const Matrix4x4& inv_m) : mat(m), inv_mat(inv_m) {}

public:
	vec3 ApplyToPoint(const vec3& point) const;

	vec3 ApplyToVector(const vec3& v) const;

	vec3 ApplyToNormal(const vec3& n) const;

public:
	Matrix4x4 mat, inv_mat;
};




#endif 
