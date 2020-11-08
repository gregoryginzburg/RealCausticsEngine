#ifndef LIGHTS_H
#define LIGHTS_H



#include <vector>
#include <memory>
#include "vec3.h"
#include "Frame.h"
#include "Color.h"
#include "MLT_Sampler.h"

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
	virtual vec3 sample_light(vec3& origin, vec3& direction, float& pdf, MLT_Sampler& Sampler) const = 0;
	virtual float get_power() const = 0;
};

class Area_Light : public Light
{
public:	
	Area_Light(vec3 p, vec3 rotation, float w, float h, float pow, vec3 c);

public:	
	virtual float get_power() const;
	virtual vec3 sample_light(vec3& origin, vec3& direction, float& pdf, MLT_Sampler& Sampler) const;
public:
	float power;
	vec3 color;

	vec3 position;
	vec3 bottom_left_corner;
	vec3 horizontal;
	vec3 vertical;
	
	Frame frame;

	float area;
	float inv_area;
	int halton_idx = 0;
	
};


class Point_Light : public Light
{
public:
	Point_Light(const vec3& p, float r, float pow, colorf c);

public:
	virtual float get_power() const;
	virtual vec3 sample_light(vec3& origin, vec3& direction, float& pdf, MLT_Sampler& Sampler) const
	{
		return vec3(0.0f);
	}
public:
	vec3 position;
	float radius;
	float power;
	colorf color;
};

class Sun_Light : public Light
{
public:
	Sun_Light(vec3 r, float a, float pow, vec3 c);

public:
	virtual float get_power() const;
	virtual vec3 sample_light(vec3& origin, vec3& direction, float& pdf, MLT_Sampler& Sampler) const;

public:
	vec3 color;
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

	int halton_idx = 0;
};


#endif
