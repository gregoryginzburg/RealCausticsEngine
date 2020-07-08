#ifndef _TRIANGLE_H
#define _TRIANGLE_H
#include "../vec2.h"
#include "2D_Hittable.h"
class triangle2 : public hit_rec
{
public:
	vec2 vertice0;
	vec2 vertice1;
	vec2 vertice2;
public:
	bool hit(vec2 point, hit_rec rec)
	{
		vec2 v0 = vertice1 - vertice0;
		vec2 v1 = vertice2 - vertice0;
		vec2 v2 = point - vertice0;
		float d00 = dot(v0, v0);
		float d01 = dot(v0, v1);
		float d11 = dot(v1, v1);
		float d20 = dot(v2, v0);
		float d21 = dot(v2, v1);
		float denom = d00 * d11 - d01 * d01;
		rec.v = (d11 * d20 - d01 * d21);
		float w = (d00 * d21 - d01 * d20);
		rec.u = 1.f - v - w;
		return rec.u <= 1.f && rec.u >= 0 && rec.v <= 1.f && rec.v >= 0;
	}
};



#endif
