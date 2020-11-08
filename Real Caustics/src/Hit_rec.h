#ifndef HITTABLE_H
#define HITTABLE_H

class Material;
class aabb;
class vec3;
class vec2;

#include "materials.h"

struct Isect
{
	vec3 geometric_normal;
	vec3 shade_normal;
	vec3 position;
	float distance = 0;	
	bool front_face = false;
	float u = 0;
	float v = 0;

	vec2 tex_coord_v0;
	vec2 tex_coord_v1;
	vec2 tex_coord_v2;

	vec3 direction;

	unsigned int material_idx;

	void compute_scattering_functions(BxDF** brdf, Material** materials, TransportMode mode)
	{
		materials[material_idx]->compute_scattering_functions(brdf, *this, mode);
	}

};


#endif


















