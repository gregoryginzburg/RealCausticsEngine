#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"
#include <algorithm>
#include "hittable.h"
class Hittable;
class aabb
{
public:
	aabb() {}
	aabb(const point3& a, const point3& b) : min(a), max(b) {}

	aabb(const point3& a, const point3& b, std::shared_ptr<Hittable> tri, const point3 c) : min(a), max(b), triangle(tri), center(c)  {}

	bool hit(const ray& r, float tmin, float tmax) const
	{
		auto invD = 1 / r.direction.x;
		auto t0 = (min.x - r.origin.x) * invD;
		auto t1 = (max.x - r.origin.x) * invD;

		if (invD < 0) { std::swap(t0, t1); }

		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;
		if (tmax < tmin) { return false; }
		//-----------------------
		invD = 1 / r.direction.y;
		t0 = (min.y - r.origin.y) * invD;
		t1 = (max.y - r.origin.y) * invD;

		if (invD < 0) { std::swap(t0, t1); }

		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;
		if (tmax < tmin) { return false; }
		//----------------------
		invD = 1 / r.direction.z;
		t0 = (min.z - r.origin.z) * invD;
		t1 = (max.z - r.origin.z) * invD;

		if (invD < 0) { std::swap(t0, t1); }

		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;
		if (tmax < tmin) { return false; }

		return true;
	}

public:
	point3 min;
	point3 max;
	std::shared_ptr<Hittable> triangle;
	point3 center;
};
aabb surrounding_box(aabb box_a, aabb box_b);
void add_point(aabb& box, vec3& point);
#endif
