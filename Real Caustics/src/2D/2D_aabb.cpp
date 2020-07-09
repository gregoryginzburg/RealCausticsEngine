#include "2D_aabb.h"
#include <algorithm>
#include <cmath> 
aabb2 surrounding_box(const aabb2 a, const aabb2 b)
{
	vec2 min = vec2(std::fmin(a.min.x, b.min.x), std::fmin(a.min.y, b.min.y));
	vec2 max = vec2(std::fmax(a.max.x, b.max.x), std::fmax(a.max.y, b.max.y));
	return aabb2(min, max);
}