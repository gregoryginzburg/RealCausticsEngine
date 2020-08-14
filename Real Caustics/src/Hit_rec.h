#ifndef HITTABLE_H
#define HITTABLE_H

class Material;
class aabb;
class vec3;
class vec2;

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

	unsigned int material_idx;
};


#endif


















