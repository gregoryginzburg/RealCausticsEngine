#include <algorithm>
#include "aabb.h"
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
