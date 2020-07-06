#ifndef PHOTON_MAP_H
#define PHOTON_MAP_H
#include <vector>
#include <memory>
#include "photon.h"

int biggest_dimension(std::vector<std::shared_ptr<photon>>& points);
int aproximate_median(std::vector<std::shared_ptr<photon>>& points, int axis, int& index);

class Photon_map
{
public:
	std::vector<std::shared_ptr<photon>> photons;
	Photon_map() {}
	Photon_map(int number_of_photons) 
	{
		photons.resize(number_of_photons);
	}
	void balance(int element, std::vector<std::shared_ptr<photon>>& points)
	{
		static Photon_map balanced(photons.size());
		if (points.size() < 2)
		{
			balanced.photons[element - 1] = points[0];
			return;
		}
		int axis = biggest_dimension(points);
		int index;

		std::shared_ptr<photon> best_split_photon = points[aproximate_median(points, axis, index)];
		float best_split;
		if (axis == 0) best_split = best_split_photon->position.x;
		else if (axis == 1) best_split = best_split_photon->position.y;
		else best_split = best_split_photon->position.z;

		std::vector<std::shared_ptr<photon>> left;
		left.reserve(points.size() / 2 + 1);
		std::vector<std::shared_ptr<photon>> right;
		right.reserve(points.size() / 2 + 1);

		for (int i = 0; i < points.size(); ++i)
		{
			if (i == index)
			{
				continue;
			}
			float position_axis;
			if (axis == 0) position_axis = points[i]->position.x;
			else if (axis == 1) position_axis = points[i]->position.y;
			else position_axis = points[i]->position.z;

			if (position_axis < best_split)
			{
				left.push_back(points[i]);
			}
			else
			{
				right.push_back(points[i]);
			}
		}
		balance(2 * element, left);
		balance(2 * element + 1, right);
		points[index]->splitting_plane = (short)axis;
		balanced.photons[element - 1] = points[index];
		if (element == 1)
		{
			*this = balanced;
		}
	
	}
};



/*void find_photons(vec3& x, float serach_distance, std::vector<photon>& photons)
{
	if (element * 2 + 1 < photons.size())
	{
		if (points[element]->splitting_plane == 0)
		{
			dist_to_plane = 0;
		}

	};

}*/
struct sorting_photon
{
	std::shared_ptr<photon> point;
	int index;
	sorting_photon(std::shared_ptr<photon> p, int i) : point(p), index(i) {}
};

#endif
