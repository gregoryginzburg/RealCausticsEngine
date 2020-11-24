#include <algorithm>
#include "aabb.h"
#include "vec3.h"


bool aabb::hit(const ray& r, float tmin, float tmax) const
{
	++BVH_Intersection_TESTS;

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


aabb Union(const aabb& bbox, const vec3& point)
{
	return aabb(
		vec3(std::min(bbox.min.x, point.x),
			std::min(bbox.min.y, point.y),
			std::min(bbox.min.z, point.z)),
		vec3(std::max(bbox.max.x, point.x),
			std::max(bbox.max.y, point.y),
			std::max(bbox.max.z, point.z))
	);
}


aabb Union(const aabb& bbox1, const aabb& bbox2)
{
	return aabb(
		vec3(std::min(bbox1.min.x, bbox2.min.x),
			std::min(bbox1.min.y, bbox2.min.y),
			std::min(bbox1.min.z, bbox2.min.z)),
		vec3(std::max(bbox1.max.x, bbox2.max.x),
			std::max(bbox1.max.y, bbox2.max.y),
			std::max(bbox1.max.z, bbox2.max.z))
	);
}

vec3 aabb::Diagonal() const
{
	return max - min;
}

float aabb::SurfaceArea() const
{
	vec3 d = Diagonal();
	return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
}

int aabb::MaximumExtent() const
{
	vec3 d = Diagonal();
	if (d.x > d.y && d.x > d.z)
		return 0;
	else if (d.y > d.z)
		return 1;
	else
		return 2;
}

vec3 aabb::Offset(const vec3& p) const
{
	vec3 o = p - min;
	o.x /= max.x - min.x;
	o.y /= max.y - min.y;
	o.z /= max.z - min.z;

	return o;
}

aabb surrounding_box(aabb box_a, aabb box_b)
{
	point3 min = point3(std::fmin(box_a.min.x, box_b.min.x),
		std::fmin(box_a.min.y, box_b.min.y),
		std::fmin(box_a.min.z, box_b.min.z));
	point3 max = point3(std::fmax(box_a.max.x, box_b.max.x),
		std::fmax(box_a.max.y, box_b.max.y),
		std::fmax(box_a.max.z, box_b.max.z));
	return aabb(min, max);
}