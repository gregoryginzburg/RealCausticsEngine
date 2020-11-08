#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include "random_generators.h"
#include "Blender_definitions.h"

class matrix_4x4;
extern const float PI;
extern const float PI2;

class vec3
{
public:
	float x;
	float y;
	float z;

public:

	vec3() : x(0.0f), y(0.0f), z(0.0f){}

	vec3(float e0) : x(e0), y(e0), z(e0){}

	vec3(float e0, float e1, float e2) : x(e0), y(e1), z(e2){}

	//construct from an array
	vec3(float arr[3]) : x(arr[0]), y(arr[1]), z(arr[2]){}



	vec3 &operator+=(const vec3 &other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	vec3 &operator-=(const vec3 &other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	vec3 &operator*=(const float t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}
	vec3 &operator*=(const vec3 &other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	vec3 &operator/=(const vec3 &other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	vec3 &operator/=(const float t)
	{
		x /= t;
		y /= t;
		z /= t;
		return *this;
	}

	//vec3& operator*=(const matrix_4x4& m);

	vec3 operator-() const
	{
		return vec3(-x, -y, -z);
	}
	float length_squared() const
	{
		return x * x + y * y + z * z;
	}
	float length() const
	{
		return std::sqrt(length_squared());
	}
	float Max() const
	{
		return std::max(std::max(x, y), z);
	}
};
using point3 = vec3; //point 3d

inline std::ostream &operator<<(std::ostream &stream, const vec3 &u)
{
	stream << u.x << " " << u.y << " " << u.z << "\n";
	return stream;
}
inline vec3 operator+(const vec3 &u, const vec3 &v)
{
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}
inline vec3 operator+(const vec3 &u, const float t)
{
	return vec3(u.x + t, u.y + t, u.z + t);
}
inline vec3 operator+(const float t, const vec3 &v)
{
	return v + t;
}

//----------------------------------------
inline vec3 operator-(const vec3 &u, const vec3 &v)
{
	return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}
inline vec3 operator-(const vec3 &u, const float t)
{
	return vec3(u.x - t, u.y - t, u.z - t);
}

//----------------------------------------
inline vec3 operator*(const vec3 &u, const vec3 &v)
{
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}
inline vec3 operator*(const vec3 &u, const float t)
{
	return vec3(u.x * t, u.y * t, u.z * t);
}
inline vec3 operator*(const float t, const vec3 &v)
{
	return v * t;
}
//----------------------------------------
inline vec3 operator/(const vec3 &u, const vec3 &v)
{
	return vec3(u.x / v.x, u.y / v.y, u.z / v.z);
}
inline vec3 operator/(const vec3 &u, const float t)
{
	return (1 / t) * u;
}
//----------------------------------------
inline bool operator==(const vec3 &u, const vec3 &v)
{
	return u.x == v.x && u.y == v.y && u.z == v.z;
}

//----------------------------------------
inline float dot(const vec3 &u, const vec3 &v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline float abs_dot(const vec3& u, const vec3& v)
{
	return std::abs(u.x * v.x + u.y * v.y + u.z * v.z);
}

inline vec3 cross(const vec3 &u, const vec3 &v)
{
	return vec3(u.y * v.z - u.z * v.y,
				u.z * v.x - u.x * v.z,
				u.x * v.y - u.y * v.x);
}
inline vec3 normalize(const vec3 &u)
{
	return u / u.length();
}
inline vec3 reflect(const vec3 &v, const vec3 &n)
{
	return v - 2 * dot(v, n) * n;
}
// wi - view local dir  wo - out direction
inline bool refract(const vec3 &wi, const vec3 &n, float etai_over_etat, vec3& wo)
{
	
	float cos_theta_i = dot(n, wi);
	float sin2_theta_i = std::max(0.0f, 1.0f - cos_theta_i * cos_theta_i);
	float sin2_theta_t = etai_over_etat * etai_over_etat * sin2_theta_i;

	// Total Internal Reflection
	if (sin2_theta_t >= 1.0f)
		return false;
	float cos_theta_t = std::sqrt(1.0f - sin2_theta_t);
	wo = etai_over_etat * -wi + (etai_over_etat * cos_theta_i - cos_theta_t) * n;
	return true;
	

	/*
	float cos_theta = dot(-wi, n);
	vec3 r_out_parallel = etai_over_etat * (wi + cos_theta * n);
	vec3 r_out_perp = -std::sqrtf(1.0f - r_out_parallel.length_squared()) * n;
	//float ior = etai_over_etat;
	//float c = dot(-n, uv);
	//return ior * uv + (ior * c - std::sqrt(1 - ior * ior * (1- c * c))) * n;
	wo = r_out_parallel + r_out_perp;
	return true;*/
}


inline float schlick(float cosine, float ref_idx) 
{
	float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	float inv_cosine = 1 - cosine;
	return r0 + (1 - r0) * inv_cosine * inv_cosine * inv_cosine * inv_cosine * inv_cosine;
}
inline float distance(const vec3 &u, const vec3 &v)
{
	return (u - v).length();
}
inline vec3 random_in_hemisphere_test()
{
	auto a = random_float(0.0f, PI2);
	auto z = random_float(-1.0f, 1.0f);
	auto r = sqrtf(1.0f - z * z);
	return vec3(r * cos(a), r * sin(a), z);
}

inline vec3 random_in_hemisphere()
{
	vec3 probe(2, 2, 2);
	while (probe.length_squared() > 1.0f)
	{
		probe.x = random_float_0_1() * 2.0f - 1.0f;
		probe.y = random_float_0_1() * 2.0f - 1.0f;
		probe.z = random_float_0_1() * 2.0f - 1.0f;
	}
	return probe;
}

inline vec3 random_point_in_sphere_test(float radius)
{
	float phi = random_float(0.0f, PI2);
	float cos_theta = random_float(-1.0f, 1.0f);
	float sin_theta = sin(acos(cos_theta));
	float r = radius * std::powf(random_float(0.0f, 1.0f), 0.33333f);

	return vec3(r * sin_theta * cos(phi), r * sin_theta * sin(phi), r * cos_theta);
}
inline vec3 random_point_in_sphere(float radius)
{
	float radius_sqr = radius * radius;
	vec3 probe(radius_sqr + 1000.0f, radius_sqr, radius_sqr);
	while (probe.length_squared() > radius_sqr)
	{
		probe.x = (random_float_0_1() * 2.0f - 1.0f) * radius;
		probe.y = (random_float_0_1() * 2.0f - 1.0f) * radius;
		probe.z = (random_float_0_1() * 2.0f - 1.0f) * radius;
	}
	return probe;
}
/*
// :angle - in radians
inline vec3 random_in_hemisphere(float angle)
{
	auto a = random_float(0.0f, PI2);
	auto z = random_float(cos(angle * PI / 180.f), 1.0f);
	auto r = sqrtf(1.0f - z * z);
	return vec3(r * cos(a), r * sin(a), z);
}*/

inline vec3 project_onto_plane(const vec3& point, const vec3& center, const vec3& normal)
{
	vec3 transformed_point = point - center;
	return (transformed_point - dot(transformed_point, normal) / normal.length_squared() * normal) + center;
}

#endif
