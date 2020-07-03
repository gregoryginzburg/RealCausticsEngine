#ifndef LIGHTS_H
#define LIGHTS_H
#include "vec3.h"
#include "ray.h"
#include "utils.h"
#include "random_generators.h"
class Light
{
public:
	virtual ray get_ray(size_t j, size_t i) const = 0;
};
class Area_Light : public Light
{
public:
	vec3 position;
	vec3 bottom_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 normal = vec3(0, 0, -1);
	float spread;

public:
	Area_Light(vec3 p, float w, float h, float spr)  : spread(spr), position(p)
	{
		bottom_left_corner = position - vec3(w / 2.f, h / 2.f, 0.f);
		horizontal = vec3(w, 0., 0.);
		vertical = vec3(0., h, 0.);	
	}
	Area_Light(vec3 p, vec3 n, float w, float h, float spr) : normal(n), spread(spr), position(p)
	{
		double angle = std::acos(dot(n, vec3(0, 0, -1)));
		vec3 axis = cross(n, vec3(0, 0, -1));
		bottom_left_corner = position - vec3(w / 2.f, h / 2.f, 0.f);
		vec3 top_left_corner = bottom_left_corner + vec3(0., h, 0.);
		vec3 bottom_rigth_corner = bottom_left_corner + vec3(w, 0., 0.);
		rotate_vec(bottom_left_corner, angle, axis);
		rotate_vec(top_left_corner, angle, axis);
		rotate_vec(bottom_rigth_corner, angle, axis);
		rotate_vec(normal, angle, axis);
		horizontal = bottom_rigth_corner - bottom_left_corner;
		vertical = top_left_corner - bottom_left_corner;
	}
public:
	virtual ray get_ray(size_t j, size_t i) const
	{
		return ray(bottom_left_corner + halton_sequnce(i, 2) * horizontal + halton_sequnce(j, 3), normal*spread*random_in_hemisphere());
	}
	//rotation in degrees
	void rotate(vec3 rotation)
	{
		vec3 top_left_corner = bottom_left_corner + vertical;
		vec3 bottom_rigth_corner = bottom_left_corner + horizontal;
		rotate_vec(bottom_left_corner, rotation);
		rotate_vec(top_left_corner, rotation);
		rotate_vec(bottom_rigth_corner, rotation);
		rotate_vec(normal, rotation);
		horizontal = bottom_rigth_corner - bottom_left_corner;
		vertical = top_left_corner - bottom_left_corner;
	}
};



#endif
