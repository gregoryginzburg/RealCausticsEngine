#ifndef HITTABLE_H
#define HITTABLE_H
#include "vec3.h"
#include "ray.h"
#include "aabb.h"
#include "vec2.h"
#include "Color.h"
class Material;
class aabb;
struct hit_rec
{
	vec3 normal;
	vec3 p;
	float t = 0;	
	bool front_face = false;
	float u = 0;
	float v = 0;
	
	vec2 tex_coord_v0;
	vec2 tex_coord_v1;
	vec2 tex_coord_v2;

	std::shared_ptr<Material> mat_ptr;

	inline void set_normal(const ray& r, vec3 outward_normal)
	{
		front_face = dot(outward_normal, r.direction) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}

};


#endif


















