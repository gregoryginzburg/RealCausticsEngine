#include <vector>
#include <memory>
#include <algorithm>
#include "photon_map.h"
#include "photon.h"
#include "random_generators.h"
#include "aabb.h"

//initialize with element = 1

extern const float inf;


int biggest_dimension(std::vector<std::shared_ptr<photon>>& points)
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
	float side1 = top.x - bottom.x;
	float side2 = top.y - bottom.y;
	float side3 = top.z - bottom.z;

	
	float max_side = std::fmax(std::fmax(top.x - bottom.x, top.y - bottom.y), top.z - bottom.z);
	return max_side == side1 ? 0 : max_side == side2 ? 1 : 2;
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





int aproximate_median(std::vector<std::shared_ptr<photon>>& points, int axis, int& index)
{
	auto comparator = (axis == 0) ? x_compare
		: (axis == 1) ? y_compare
		: z_compare;
	if (true)  //points.size() < 500)
	{
		std::vector<sorting_photon> photons;
		photons.reserve(points.size());
		for (int i = 0; i < points.size(); ++i)
		{
			photons.emplace_back(sorting_photon(points[i], i));
		}
		std::sort(photons.begin(), photons.end(), comparator);
		index = photons[photons.size() / 2].index;
		return index;
	}
	else
	{
		
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