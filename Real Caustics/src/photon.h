#ifndef PHOTON_H
#define PHOTON_H
#include "Color.h"
#include "vec3.h"
#include <math.h>
//26 bytes
extern const float PI;

struct photon
{
	vec3 position;
	colorf power;
	vec3 direction;
	// θ
	//unsigned char theta;
	// φ 
	//unsigned char phi;
	//0 = x, 1 = y, 2 = z
	photon(const vec3& p, const colorf& pow) : position(p), power(pow) {}
	photon(const vec3& p, const colorf& pow, const vec3& dir) : position(p), power(pow), direction(normalize(dir))
	{
		/*
		int temp_theta = int(std::acos(direction.z) * (256.0 / PI));

		if (theta > 255)
			theta = temp_theta;
		else
			theta = (unsigned char)temp_theta;

		int local_phi = int(std::atan2(direction.y, direction.x) * (256.0 / (2.0 * PI)));

		if (local_phi > 255)
			phi = 255;
		else if (phi < 0)
			phi = (unsigned char)(local_phi + 256);
		else
			phi = (unsigned char)local_phi;*/
	}
	photon(const vec3& p) : position(p) {}
};




#endif
