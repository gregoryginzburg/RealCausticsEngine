#ifndef LIGHTS_H
#define LIGHTS_H
#include "vec3.h"
#include "ray.h"
#include "utils.h"
class Light
{
public:
	virtual ray get_ray() const = 0;
};
class Area_Light : public Light
{
public:
	vec3 position;
	double width;
	double height;
	vec3 rotation;
public:
	Area_Light(vec3 p, double w, double h) : position(p), width(w), height(h) {}
public:

	virtual ray get_ray() const
	{
		vec3 bottom_left_corner = position - vec3(width / 2., height / 2., 0.);

	}
};



#endif
