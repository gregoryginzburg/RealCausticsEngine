#include "matrix.h"
#include <iostream>

Matrix4x4 Inverse(const Matrix4x4& mat)
{
	// Convert to doubles for more precision when substracting
	double m[4][4];
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			m[i][j] = mat.m[i][j];
		}
	}

	double A2323 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
	double A1323 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
	double A1223 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
	double A0323 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
	double A0223 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
	double A0123 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
	double A2313 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
	double A1313 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
	double A1213 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
	double A2312 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
	double A1312 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
	double A1212 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
	double A0313 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
	double A0213 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
	double A0312 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
	double A0212 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
	double A0113 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
	double A0112 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

	double det = m[0][0] * (m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223)
		- m[0][1] * (m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223)
		+ m[0][2] * (m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123)
		- m[0][3] * (m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123);
	det = 1.0 / det;
	if (det < 0)
		std::cout << "DETERMINENT < 0" << "\n";

	Matrix4x4 new_mat;

	new_mat.m[0][0] = static_cast<float>(det *  (m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223));
	new_mat.m[0][1] = static_cast<float>(det * -(m[0][1] * A2323 - m[0][2] * A1323 + m[0][3] * A1223));
	new_mat.m[0][2] = static_cast<float>(det *  (m[0][1] * A2313 - m[0][2] * A1313 + m[0][3] * A1213));
	new_mat.m[0][3] = static_cast<float>(det * -(m[0][1] * A2312 - m[0][2] * A1312 + m[0][3] * A1212));
	new_mat.m[1][0] = static_cast<float>(det * -(m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223));
	new_mat.m[1][1] = static_cast<float>(det *  (m[0][0] * A2323 - m[0][2] * A0323 + m[0][3] * A0223));
	new_mat.m[1][2] = static_cast<float>(det * -(m[0][0] * A2313 - m[0][2] * A0313 + m[0][3] * A0213));
	new_mat.m[1][3] = static_cast<float>(det *  (m[0][0] * A2312 - m[0][2] * A0312 + m[0][3] * A0212));
	new_mat.m[2][0] = static_cast<float>(det *  (m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123));
	new_mat.m[2][1] = static_cast<float>(det * -(m[0][0] * A1323 - m[0][1] * A0323 + m[0][3] * A0123));
	new_mat.m[2][2] = static_cast<float>(det *  (m[0][0] * A1313 - m[0][1] * A0313 + m[0][3] * A0113));
	new_mat.m[2][3] = static_cast<float>(det * -(m[0][0] * A1312 - m[0][1] * A0312 + m[0][3] * A0112));
	new_mat.m[3][0] = static_cast<float>(det * -(m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123));
	new_mat.m[3][1] = static_cast<float>(det *  (m[0][0] * A1223 - m[0][1] * A0223 + m[0][2] * A0123));
	new_mat.m[3][2] = static_cast<float>(det * -(m[0][0] * A1213 - m[0][1] * A0213 + m[0][2] * A0113));
	new_mat.m[3][3] = static_cast<float>(det *  (m[0][0] * A1212 - m[0][1] * A0212 + m[0][2] * A0112));

	return new_mat;
}








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
