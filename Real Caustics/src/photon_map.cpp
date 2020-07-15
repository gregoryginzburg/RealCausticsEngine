#include <vector>
#include <memory>
#include <algorithm>
#include "photon_map.h"
#include "photon.h"
#include "random_generators.h"
#include "aabb.h"

//initialize with element = 1

extern const float inf;




KDTreeNode* build(std::vector<std::shared_ptr<photon>>& points)
{
	if (points.size() < 4)
	{
		KDTreeLeaf* leaf = new KDTreeLeaf;
		for (int i = 0; i < points.size(); ++i)
		{
			leaf->points.push_back(points[i]);
		}
		return leaf;
	}
	else
	{
		std::vector<std::shared_ptr<photon>> left_objects;
		left_objects.reserve(best_left_amount);
		std::vector<std::shared_ptr<photon>> right_objects;
		right_objects.reserve(best_right_amount);
		std::vector<std::shared_ptr<photon>> objects_on_median;

		for (size_t i = 0; i < points.size(); ++i)
		{
			float point_axis;
			if (axis == 0) point_axis = points[i]->position.x;
			else if (axis == 1) point_axis = points[i]->position.y;
			else point_axis = points[i]->position.z;

			if (point_axis < best_split)
			{
				left_objects.push_back(points[i]);
			}
			else if (point_axis == best_split)
			{
				objects_on_median.push_back(points[i]);
			}
			else
			{
				right_objects.push_back(points[i]);
			}
		}

		KDTreeInner* inner = new KDTreeInner;
		inner->axis = axis;
		inner->split = best_split;
		inner->points_on_median->points = objects_on_median;

		inner->_left = build(left_objects);
		inner->_right = build(right_objects);
		return inner;
	}


}

void find_photons(KDTreeNode* root, vec3& point, float search_d, Priority_queue& closest_photons)
{
	static float search_distance_squared = search_d;
	if (!root->IsLeaf())
	{
		KDTreeInner* photons_inner = dynamic_cast<KDTreeInner*>(root);
		float dist_to_plane;
		if (photons_inner->axis == 0)
			dist_to_plane = point.x - photons_inner->split;
		else if (photons_inner->axis == 1)
			dist_to_plane = point.y - photons_inner->split;
		else
			dist_to_plane = point.z - photons_inner->split;
		if (dist_to_plane < 0)
		{
			find_photons(photons_inner->_left, point, search_distance_squared, closest_photons);
			if (dist_to_plane * dist_to_plane < search_distance_squared)
			{
				find_photons(photons_inner->_right, point, search_distance_squared, closest_photons);
			}

		}
		else
		{
			find_photons(photons_inner->_right, point, search_distance_squared, closest_photons);
			if (dist_to_plane * dist_to_plane < search_distance_squared)
			{				
				find_photons(photons_inner->_left, point, search_distance_squared, closest_photons);
			}

		}
	}
	else
	{
		KDTreeLeaf* photons_leaf = dynamic_cast<KDTreeLeaf*>(root);
		for (int i = 0; i < photons_leaf->points.size(); ++i)
		{
			float dist_to_point_squared = (point - photons_leaf->points[i]->position).length_squared();
			if (dist_to_point_squared < search_distance_squared)
			{
				closest_photons.insert_element(photons_leaf->points[i], dist_to_point_squared);
				if (closest_photons.capacity == closest_photons.size)
				{
					search_distance_squared = (point - photons_leaf->points[i]->position).length_squared();
				}
			}
		}

	}

}
int CountBoxes(KDTreeNode* root)
{
	if (!root->IsLeaf()) {
		KDTreeInner* p = dynamic_cast<KDTreeInner*>(root);
		return 1 + CountBoxes(p->_left) + CountBoxes(p->_right);
	}
	else
		return 1;
}
