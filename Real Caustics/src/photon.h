#ifndef PHOTON_H
#define PHOTON_H
#include "Color.h"
#include "vec3.h"
//26 bytes
struct photon
{
	vec3 position;
	colorf power;
	//0 = x, 1 = y, 2 = z
	photon(const vec3& p, const colorf& pow) : position(p), power(pow) {}
	photon(const vec3& p) : position(p) {}
};




#endif
