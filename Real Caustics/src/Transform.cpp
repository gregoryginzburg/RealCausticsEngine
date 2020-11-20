#include "Transform.h"

vec3 Transform::ApplyToPoint(const vec3& point) const
{
	float x = point.x, y = point.y, z = point.z;
	float xp = mat.m[0][0] * x + mat.m[0][1] * y + mat.m[0][2] * z + mat.m[0][3];
	float yp = mat.m[1][0] * x + mat.m[1][1] * y + mat.m[1][2] * z + mat.m[1][3];
	float zp = mat.m[2][0] * x + mat.m[2][1] * y + mat.m[2][2] * z + mat.m[2][3];

	return vec3(xp, yp, zp);
}

vec3 Transform::ApplyToVector(const vec3& v) const
{
	float x = v.x, y = v.y, z = v.z;
	float xp = mat.m[0][0] * x + mat.m[0][1] * y + mat.m[0][2] * z;
	float yp = mat.m[1][0] * x + mat.m[1][1] * y + mat.m[1][2] * z;
	float zp = mat.m[2][0] * x + mat.m[2][1] * y + mat.m[2][2] * z;

	return vec3(xp, yp, zp);
}

vec3 Transform::ApplyToNormal(const vec3& n) const
{
	float x = n.x, y = n.y, z = n.z;

	float xp = inv_mat.m[0][0] * x + inv_mat.m[1][0] * y + inv_mat.m[2][0] * z;
	float yp = inv_mat.m[0][1] * x + inv_mat.m[1][1] * y + inv_mat.m[2][1] * z;
	float zp = inv_mat.m[0][2] * x + inv_mat.m[1][2] * y + inv_mat.m[2][2] * z;

	return vec3(xp, yp, zp);
}
