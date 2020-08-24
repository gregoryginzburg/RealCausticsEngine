#include "matrix.h"

matrix_3x3::matrix_3x3(vec3 ii, vec3 jj, vec3 kk) : i(ii), j(jj), k(kk)
{

}

matrix_3x3::matrix_3x3(vec3 r)
{
	// https://en.wikipedia.org/wiki/Rotation_matrix
	i = vec3(cos(r.z) * cos(r.y),
		sin(r.z) * cos(r.y),
		-sin(r.y));
	j = vec3(cos(r.z) * sin(r.y) * sin(r.x) - sin(r.z) * cos(r.x),
		sin(r.z) * sin(r.y) * sin(r.x) + cos(r.z) * cos(r.x),
		cos(r.y) * sin(r.x));
	k = vec3(cos(r.z) * sin(r.y) * cos(r.x) + sin(r.z) * sin(r.x),
		sin(r.z) * sin(r.y) * cos(r.x) - cos(r.z) * sin(r.x),
		cos(r.y) * cos(r.x));
}

matrix_3x3::matrix_3x3(float a, vec3 u)
{
	float c = cos(a);
	float s = sin(a);
	float t = 1 - c;
	i = vec3(c + u.x * u.x * t, u.x * u.y * t + u.z * s, u.x * u.z * t - u.y * s);
	j = vec3(u.x * u.y * t - u.z * s, c + u.y * u.y * t, u.y * u.z * t + u.x * s);
	k = vec3(u.x * u.z * t + u.y * s, u.y * u.z * t - u.x * s, c + u.z * u.z * t);
}

matrix_4x4::matrix_4x4(vec4 ii, vec4 jj, vec4 kk, vec4 ww) : i(ii), j(jj), k(kk), w(ww)
{

}