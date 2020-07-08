#include <algorithm>
#include "aabb.h"
#include "vec3.h"
//class aabb;
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
void add_point(aabb& box, vec3& point)
{
	if (point.x < box.min.x)
	{
		box.min.x = point.x;
	}
	if (point.y < box.min.y)
	{
		box.min.y = point.y;
	}
	if (point.z < box.min.z)
	{
		box.min.z = point.z;
	}
	if (point.x > box.max.x)
	{
		box.max.x = point.x;
	}
	if (point.y > box.max.y)
	{
		box.max.y = point.y;
	}
	if (point.z > box.max.z)
	{
		box.max.z = point.z;
	}
	return;
}
