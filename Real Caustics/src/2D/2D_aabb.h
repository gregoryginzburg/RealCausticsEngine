#ifndef _AABB_H
#define _AABB_H
#include "../vec2.h"

class aabb2
{
public:
	vec2 min;
	vec2 max;
public:
	aabb2() {}
	aabb2(vec2 a, vec2 b) : min(a), max(b) {}
public:
	bool hit(vec2 point)
	{
		return point.x >= min.x && point.x <= max.x&& point.y >= min.y && point.y <= max.y;
	}
};
aabb2 surrounding_box(const aabb2 a, const aabb2 b);


#endif 

