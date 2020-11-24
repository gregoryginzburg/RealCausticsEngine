#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"
#include <algorithm>
#include "Hit_rec.h"
class Hittable;
extern long long BVH_Intersection_TESTS;
extern const float inf;

class aabb
{
public:
	aabb()
	{
		min = vec3(inf, inf, inf);
		max = vec3(-inf, -inf, -inf);
	}
	aabb(const point3& a, const point3& b) : min(a), max(b) {}

	bool hit(const ray& r, float tmin, float tmax) const;

	vec3 Diagonal() const;

	float SurfaceArea() const;

	int MaximumExtent() const;

	vec3 Offset(const vec3& p) const;


public:
	point3 min;
	point3 max;
};

aabb Union(const aabb& bbox, const vec3& point);

aabb Union(const aabb& bbox1, const aabb& bbox2);

aabb surrounding_box(aabb box_a, aabb box_b);

#endif
