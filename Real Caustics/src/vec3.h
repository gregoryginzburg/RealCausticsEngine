#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include "random_generators.h"

extern const float PI;

class vec3
{
public:
	float x;
	float y;
	float z;
public:
	// без параметров - вектор (0,0,0)
	vec3() : x(0), y(0), z(0) {};  
	// с 1 параметром вектор с одинаковыми числами
	vec3(float e0) : x(e0), y(e0), z(e0) {};
	// дефолт конструктор для вектора
	vec3(float e0, float e1, float e2) : x(e0), y(e1), z(e2) {};
	
	vec3& operator+=(const vec3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	vec3& operator-=(const vec3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	vec3& operator*=(const float t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}
	vec3& operator*=(const vec3& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}
	vec3& operator/=(const vec3& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	vec3& operator/=(const float t)
	{
		x /= t;
		y /= t;
		z /= t;
		return *this;
	}
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
};
using point3 = vec3;    //point 3d


inline std::ostream& operator<<(std::ostream& stream,const vec3& u)
{
	stream << u.x << " " << u.y << " " << u.z << "\n";
	return stream;
}
inline vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}
inline vec3 operator+(const vec3& u, const float t)
{
	return vec3(u.x + t, u.y + t, u.z + t);
}
inline vec3 operator+(const float t, const vec3& v)
{
	return v + t;
}
//----------------------------------------
inline vec3 operator-(const vec3& u, const vec3& v)
{
	return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}
inline vec3 operator-(const vec3& u, const float t)
{
	return vec3(u.x - t, u.y - t, u.z - t);
}
//----------------------------------------
inline vec3 operator*(const vec3& u, const vec3& v)
{
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}
inline vec3 operator*(const vec3& u, const float t)
{
	return vec3(u.x * t, u.y * t, u.z * t);
}
inline vec3 operator*(const float t, const vec3& v)
{
	return v * t;
}
//----------------------------------------
inline vec3 operator/(const vec3& u, const vec3& v)
{
	return vec3(u.x / v.x, u.y / v.y, u.z / v.z);
}
inline vec3 operator/(const vec3& u, const float t)
{
	return (1 / t) * u;
}
//----------------------------------------
inline bool operator==(const vec3& u, const vec3& v)
{
	return u.x == v.x && u.y == v.y && u.z == v.z;
}


//----------------------------------------
inline float dot(const vec3& u, const vec3& v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}
inline vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x);
}
inline vec3 normalize(const vec3& u)
{
	return u / u.length();
}
inline vec3 reflect(const vec3& v, const vec3& n)
{
	return v - 2 * dot(v, n) * n;
}
inline vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) {
	auto cos_theta = dot(-uv, n);
	vec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_perp = -std::sqrt(1.0 - r_out_parallel.length_squared()) * n;
	return r_out_parallel + r_out_perp;
}
inline float distance(const vec3& u, const vec3& v)
{
	return (u - v).length();
}
inline vec3 random_in_hemisphere() 
{
	auto a = random_float(0, 2. * PI);
	auto z = random_float(0., 1.);
	auto r = sqrt(1 - z * z);
	return vec3(r * cos(a), r * sin(a), z);
}



#endif
