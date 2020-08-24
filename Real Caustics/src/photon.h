#ifndef PHOTON_H
#define PHOTON_H
#include "Color.h"
#include "vec3.h"
//26 bytes
struct photon
{
	vec3 position;
	colorf power;
	// θ
	unsigned char theta;
	// 
	//0 = x, 1 = y, 2 = z
	photon(const vec3& p, const colorf& pow) : position(p), power(pow) {}
	photon(const vec3& p, const colorf& pow, bool is_on_side) : position(p), power(pow), 
		is_on_normal_side(is_on_side) {}
	photon(const vec3& p) : position(p) {}
};




#endif
