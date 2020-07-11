#include <vector>
#include <memory>
#include <algorithm>
#include "photon_map.h"
#include "photon.h"
#include "random_generators.h"
#include "aabb.h"

//initialize with element = 1

extern const float inf;


aabb construct_bbox(std::vector<std::shared_ptr<photon>>& points)
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
	return aabb(bottom, top);
}


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
	aabb bbox_temp = construct_bbox(points);
	float min_cost = inf;
	float best_split = inf;
	bool is_better_split = false;
	int count_left;
	int count_right;
	int best_left_amount;
	int best_right_amount;



	float side1 = bbox_temp.max.x - bbox_temp.min.x;
	float side2 = bbox_temp.max.y - bbox_temp.min.y;
	float side3 = bbox_temp.max.z - bbox_temp.min.z;

	float max_side = std::fmax(std::fmax(side1, side2), side3);
	int axis = max_side == side1 ? 0 : max_side == side2 ? 1 : 2;

	float start;
	float stop;


	if (axis == 0)
	{
		start = bbox_temp.min.x;
		stop = bbox_temp.max.x;
	}
	else if (axis == 1)
	{
		start = bbox_temp.min.y;
		stop = bbox_temp.max.y;
	}
	else
	{
		start = bbox_temp.min.z;
		stop = bbox_temp.max.z;
	}
	float step = (stop - start) / 16.f;

	for (float testSplit = start + step; testSplit < stop - step; testSplit += step)
	{
		count_left = 0;
		count_right = 0;
		for (unsigned int i = 0; i < points.size(); ++i)
		{
			float point_axis;
			if (axis == 0) point_axis = points[i]->position.x;
			else if (axis == 1) point_axis = points[i]->position.y;
			else point_axis = points[i]->position.z;

			if (point_axis < testSplit)
			{
				count_left += 1;
			}
			else
			{
				count_right += 1;
			}
		}
		if (count_left < 1 || count_right < 1)
		{
			continue;
		}
		float lside1;
		float lside2;
		float lside3;
		float rside1;
		float rside2;
		float rside3;
		if (axis == 0)
		{
			lside1 = testSplit;
			lside2 = bbox_temp.max.y - bbox_temp.min.y;
			lside3 = bbox_temp.max.z - bbox_temp.min.z;
			rside1 = bbox_temp.max.x - testSplit;
			rside2 = bbox_temp.max.y - bbox_temp.min.y;
			rside3 = bbox_temp.max.z - bbox_temp.min.z;
		}
		else if (axis == 1)
		{
			lside1 = bbox_temp.max.x - bbox_temp.min.x;
			lside2 = testSplit;
			lside3 = bbox_temp.max.z - bbox_temp.min.z;
			rside1 = bbox_temp.max.x - bbox_temp.min.x;
			rside2 = bbox_temp.max.y - testSplit;
			rside3 = bbox_temp.max.z - bbox_temp.min.z;
		}
		else
		{
			lside1 = bbox_temp.max.x - bbox_temp.min.x;
			lside2 = bbox_temp.max.y - bbox_temp.min.y;
			lside3 = testSplit;
			rside1 = bbox_temp.max.x - bbox_temp.min.x;
			rside2 = bbox_temp.max.y - bbox_temp.min.y;
			rside3 = bbox_temp.max.z - testSplit;
		}
		

		float lsurface_area = lside1 * lside2 + lside2 * lside3 + lside3 * lside1;
		float rsurface_area = rside1 * rside2 + rside2 * rside3 + rside3 * rside1;

		float total_cost = lsurface_area * count_left + rsurface_area * count_right;

		if (total_cost < min_cost)
		{
			min_cost = total_cost;
			best_split = testSplit;
			is_better_split = true;
			best_left_amount = count_left;
			best_right_amount = count_right;
		}

	}
	if (!is_better_split)
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
				/*if (photons_inner->points_on_median->points.size())
				{
					for (int i = 0; i < photons_inner->points_on_median->points.size(); ++i)
					{
						float dist_to_point_squared = (point - photons_inner->points_on_median->points[i]->position).length_squared();
						if (dist_to_point_squared < search_distance_squared)
						{
							closest_photons.insert_element(photons_inner->points_on_median->points[i], dist_to_point_squared);
							if (closest_photons.capacity == closest_photons.size)
							{
								search_distance_squared = (point - photons_inner->points_on_median->points[i]->position).length_squared();
							}
						}
					}
				}*/
				find_photons(photons_inner->_right, point, search_distance_squared, closest_photons);
			}

		}
		else
		{
			find_photons(photons_inner->_right, point, search_distance_squared, closest_photons);
			if (dist_to_plane * dist_to_plane < search_distance_squared)
			{
				/*if (photons_inner->points_on_median->points.size())
				{
					for (int i = 0; i < photons_inner->points_on_median->points.size(); ++i)
					{
						float dist_to_point_squared = (point - photons_inner->points_on_median->points[i]->position).length_squared();
						if (dist_to_point_squared < search_distance_squared)
						{
							closest_photons.insert_element(photons_inner->points_on_median->points[i], dist_to_point_squared);
							if (closest_photons.capacity == closest_photons.size)
							{
								search_distance_squared = (point - photons_inner->points_on_median->points[i]->position).length_squared();
							}
						}
					}
				}*/
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
void delete_kd_tree(KDTreeNode* root)
{
	if (!root->IsLeaf())
	{
		KDTreeInner* photons_inner = dynamic_cast<KDTreeInner*>(root);
		delete_kd_tree(photons_inner->_left);
		delete_kd_tree(photons_inner->_right);
		delete photons_inner;
	}
	else
	{
		KDTreeLeaf* photons_leaf = dynamic_cast<KDTreeLeaf*>(root);
		delete photons_leaf;
	}
		
}
