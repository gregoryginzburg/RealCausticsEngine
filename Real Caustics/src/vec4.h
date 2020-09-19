#ifndef VEC4_H
#define VEC4_H

class vec4
{
public:
	float x;
	float y;
	float z;
	float w;
public:
	// без параметров - вектор (0,0,0)
	vec4() : x(0), y(0), z(0), w(0) {};
	// с 1 параметром вектор с одинаковыми числами
	vec4(float e0) : x(e0), y(e0), z(e0), w(e0) {};
	// дефолт конструктор для вектора
	vec4(float e0, float e1, float e2, float e3) : x(e0), y(e1), z(e2), w(e3) { }
	float get_length()
	{
		return std::sqrtf(x * x + y * y + z * z);
	}
	vec4& operator/=(float t)
	{
		x /= t;
		y /= t;
		z /= t;
		w /= t;
		return *this;
	}
};

inline vec4 operator+(const vec4& u, const vec4& v)
{
	return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}
inline vec4 operator*(const vec4& u, float t)
{
	return vec4(u.x * t, u.y * t, u.z * t, u.w * t);
}
inline vec4 operator*(float t, const vec4& u)
{
	return u * t;
}






#endif 

