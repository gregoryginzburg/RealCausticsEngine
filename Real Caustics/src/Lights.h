#ifndef LIGHTS_H
#define LIGHTS_H



#include <vector>
#include <memory>
#include "Scene.h"
#include "vec3.h"

extern const float negative_inf;





class Python_Light
{
public:
	char type;

	vec3 position;
	float width;
	float height;
	vec3 rotation;
	float power;
	float angle;
};

class Light
{
public:
	virtual ray emit_photon(size_t ii, size_t i, size_t j, float power, float total_i, float total_j) const = 0;
	virtual float get_power() const = 0;
	virtual void geyt_i_j(int number_of_rays, int &i, int &j) = 0;
};

class Area_Light : public Light
{
public:
	vec3 position;
	vec3 bottom_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 normal = vec3(0, 0, -1);
	float power;
	float width;
	float height;

public:
	Area_Light(vec3 p, float w, float h, float pow) : position(p), power(pow)
	{
		bottom_left_corner = position - vec3(w / 2.f, h / 2.f, 0.f);
		horizontal = vec3(w, 0., 0.);
		vertical = vec3(0., h, 0.);
		width = w;
		height = h;
	}
	
	Area_Light(vec3 p, vec3 rot, float w, float h, float pow) : Area_Light(p, w, h, pow)
	{
		rotate(rot);
	}

public:
	virtual ray emit_photon(size_t ii, size_t i, size_t j, float power, float total_i, float total_j) const;
	
	virtual float get_power() const
	{
		return power;
	}
	//rotation in degrees
	void rotate(vec3 rotation);
	
	void geyt_i_j(int number_of_rays, int& i, int& j);
	
};

class Point_Light : public Light
{
public:
	Point_Light() {}
public:
	virtual ray emit_photon(size_t ii, size_t i, size_t j, float power, float total_i, float total_j) const
	{
		return ray(vec3(0, 0, 0), vec3(0, 0, 0));
	}
	virtual float get_power() const
	{
		return 0.0f;
	}
	virtual void geyt_i_j(int number_of_rays, int &i, int &j)
	{

	}
};

class Sun_Light : public Light
{
public:
	Sun_Light(vec3 p, vec3 r, float a, float pow);

public:
	virtual ray emit_photon(size_t ii, size_t i, size_t j, float power, float total_i, float total_j) const;

	virtual float get_power() const;

	virtual void geyt_i_j(int number_of_rays, int &i, int &j);

public:
	vec3 position;
	vec3 bottom_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 normal = vec3(0, 0, -1);
	float power;
	float width;
	float height;
	float angle;
	//in radians
	vec3 rotation;
};

struct helper_light_emit
{
	int light = 0;
	int current_call = -1;
};

#endif
