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
	short splitting_plane;
	photon(vec3& p, short flag) : position(p), splitting_plane(flag) {}
};




#endif
