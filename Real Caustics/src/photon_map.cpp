#include <vector>
#include <memory>
#include <algorithm>
#include "photon_map.h"
#include "photon.h"
#include "random_generators.h"
#include "aabb.h"

//initialize with element = 1

extern const float inf;


float biggest_dimension(std::vector<std::shared_ptr<photon>>& points)
{
	vec3 bottom(inf, inf, inf);
	vec3 top(-inf, -inf, -inf);
	for (int i = 0; i < points.size(); ++i)
	{
		if (points[i]->position.x < bottom.x)
		{
			bottom.x = points[i]->position.x;
		}
		if (points[i]->position.y < bottom.y)
		{
			bottom.y = points[i]->position.y;
		}
		if (points[i]->position.z < bottom.z)
		{
			bottom.z = points[i]->position.z;
		}
		if (points[i]->position.x > top.x)
		{
			top.x = points[i]->position.x;
		}
		if (points[i]->position.y > top.y)
		{
			top.y = points[i]->position.y;
		}
		if (points[i]->position.z > top.z)
		{
			top.z = points[i]->position.z;
		}
	}
	return std::fmax(std::fmax(top.x - bottom.x, top.y - bottom.y), top.z - bottom.z);
}

bool x_compare(sorting_photon& a, sorting_photon& b)
{
	return a.point->position.x < b.point->position.x;
}
bool y_compare(sorting_photon& a, sorting_photon& b)
{
	return a.point->position.y < b.point->position.y;
}
bool z_compare(sorting_photon& a, sorting_photon& b)
{
	return a.point->position.z < b.point->position.z;
}


bool x_compare_1(std::shared_ptr<photon>& a, std::shared_ptr<photon>& b)
{
	return a->position.x < b->position.x;
}
bool y_compare_1(std::shared_ptr<photon>& a, std::shared_ptr<photon>& b)
{
	return a->position.y < b->position.y;
}
bool z_compare_1(std::shared_ptr<photon>& a, std::shared_ptr<photon>& b)
{
	return a->position.z < b->position.z;
}


int aproximate_median(std::vector<std::shared_ptr<photon>>& points, int axis, int& index)
{
	
	if (points.size() < 500)
	{
		auto comparator = (axis == 0) ? x_compare_1
			: (axis == 1) ? y_compare_1
			: z_compare_1;
		std::vector<sorting_photon> photons;
		photons.reserve(points.size());
		for (int i = 0; i < points.size(); ++i)
		{
			photons.emplace_back(sorting_photon(points[i], i));
		}
		std::sort(points.begin(), points.end(), comparator);
		index = photons[points.size() / 2].index;
		return index;
	}
	else
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
		index = random_photons[201].index;
		return index;
	}
	
}