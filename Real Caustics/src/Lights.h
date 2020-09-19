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
	// "S"(83) - Sun, "P"(80) - Point light, "A"(65) - Area Light
	char type;

	colorf color;
	vec3 position;
	// for area light
	float width;
	float height;
	float spread;
	// ---------- 
	vec3 rotation;
	float power;
	// for sun light
	float angle;
	// ----------
	// for point light 
	float radius;
	// ----------
};

class Light
{
public:
	virtual ray emit_photon(size_t ii, int total_number_of_photons) const = 0;
	virtual float get_power() const = 0;
	virtual void geyt_i_j(int number_of_rays, int &i, int &j) = 0;
};

class Area_Light : public Light
{
public:
	colorf color;
	vec3 position;
	vec3 bottom_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 normal = vec3(0, 0, -1);
	float power;
	float width;
	float height;
	float spread;

public:	
	Area_Light(vec3 p, vec3 rot, float w, float h, float pow, float spr, colorf c);	

public:
	virtual ray emit_photon(size_t ii, int total_number_of_photons) const;
	
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
	Point_Light(const vec3& p, float r, float pow, colorf c);
public:
	virtual ray emit_photon(size_t ii, int total_number_of_photons) const;

	virtual float get_power() const;

	virtual void geyt_i_j(int number_of_rays, int& i, int& j);

public:
	vec3 position;
	float radius;
	float power;
	colorf color;
};

class Sun_Light : public Light
{
public:
	Sun_Light(vec3 p, vec3 r, float a, float pow, colorf c);

public:
	virtual ray emit_photon(size_t ii, int total_number_of_photons) const;

	virtual float get_power() const;

	virtual void geyt_i_j(int number_of_rays, int &i, int &j);

public:
	colorf color;
	vec3 position;
	vec3 bottom_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 normal = vec3(0, 0, -1);
	float power;
	float width;
	float height;
	float spread;
	//in radians
	vec3 rotation;
};

struct helper_light_emit
{
	int light = 0;
	int current_call = -1;
};

#endif
