#ifndef _TRIANGLE_H
#define _TRIANGLE_H
#include "../vec2.h"
#include "2D_aabb.h"
#include "2D_Hit_rec.h"
#include <cmath> 
class triangle2
{
public:
	vec2 vertice0;
	vec2 vertice1;
	vec2 vertice2;
public:
	triangle2() {}
	triangle2(vec2 vert0, vec2 vert1, vec2 vert2) : vertice0(vert0), vertice1(vert1), vertice2(vert2) {}
public:
	bool hit(vec2 point, hit_rec_2& rec)
	{
		vec2 c = vertice1 - vertice0;
		vec2 b = vertice2 - vertice0;
		vec2 p = point - vertice0;
		float cc = dot(c, c);
		float bc = dot(b, c);
		float pc = dot(c, p);
		float bb = dot(b, b);
		float pb = dot(b, p);
		float denom = cc * bb - bc * bc;
		rec.u = (bb * pc - bc * pb) / denom;
		rec.v = (cc * pb - bc * pc) / denom;
		return (rec.u >= 0.f) && (rec.v >= 0.f) && (rec.u + rec.v <= 1.f);
	}
	aabb2 bounding_box()
	{
		return aabb2(vec2(std::fmin(std::fmin(vertice0.x, vertice1.x), vertice2.x),
			std::fmin(std::fmin(vertice0.y, vertice1.y), vertice2.y)),
			vec2(std::fmax(std::fmax(vertice0.x, vertice1.x), vertice2.x),
				std::fmax(std::fmax(vertice0.y, vertice1.y), vertice2.y)));
	}
};



#endif
