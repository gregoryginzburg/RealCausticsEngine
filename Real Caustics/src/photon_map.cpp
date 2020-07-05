#include <vector>
#include <memory>
#include "photon_map.h"
#include "photon.h"
#include "random_generators.h"
//initialize with element = 1

extern const float inf;
extern const float negative_inf;

void calculate_bbox(std::vector<std::shared_ptr<photon>>& points)
{
	vec3 bottom;
	vec3 top;
	for (int i = 0; i < points.size)
	{
		if ()
	}
}

bool x_compare(sorting_photon a, sorting_photon b)
{
	return a.point->position.x < b.point->position.x;
}
bool y_compare(sorting_photon a, sorting_photon b)
{
	return a.point->position.y < b.point->position.y;
}
bool z_compare(sorting_photon a, sorting_photon b)
{
	return a.point->position.z < b.point->position.z;
}


int aproximate_median(std::vector<std::shared_ptr<photon>>& points, int axis)
{
	auto comparator = (axis == 0) ? x_compare
		: (axis == 1) ? y_compare
		: z_compare;
	std::vector<sorting_photon> random_photons;
	random_photons.reserve(401);
	for (int i = 0; i < 401; ++i)
	{
		int random = random_int(points.size() + 1);
		random_photons.emplace_back(sorting_photon(points[random], random));
	}
	std::sort(random_photons.begin(), random_photons.end(), comparator);
	return random_photons[201].index;
}