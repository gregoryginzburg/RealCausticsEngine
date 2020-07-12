#include <vector>
#include <memory>
#include <algorithm>
#include "photon_map.h"
#include "photon.h"
#include "random_generators.h"
#include "aabb.h"
#include <cmath>


extern const float inf;
void construct_bbox(std::vector<std::shared_ptr<photon>>& points, vec3& bottom, vec3& top)
{
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
}

int biggest_axis(std::vector<std::shared_ptr<photon>>& points)
{
	vec3 bottom(inf, inf, inf);
	vec3 top(-inf, -inf, -inf);
	if (points.size() < 500)
	{
		construct_bbox(points, bottom, top);
	}
	else
	{
		std::vector<std::shared_ptr<photon>> random_photons;
		random_photons.reserve(500);
		for (int i = 0; i < 500; ++i)
		{
			int random = random_int(points.size());
			random_photons.push_back(points[random]);
		}
		construct_bbox(random_photons, bottom, top);
	}
		
	float side1 = top.x - bottom.x;
	float side2 = top.y - bottom.y;
	float side3 = top.z - bottom.z;
	float max_side = std::fmax(std::fmax(top.x - bottom.x, top.y - bottom.y), top.z - bottom.z);
	return max_side == side1 ? 0 : max_side == side2 ? 1 : 2;
}

bool x_compare(std::shared_ptr<photon>& a, std::shared_ptr<photon>& b)
{
	return a->position.x < b->position.x;
}
bool y_compare(std::shared_ptr<photon>& a, std::shared_ptr<photon>& b)
{
	return a->position.y < b->position.y;
}
bool z_compare(std::shared_ptr<photon>& a, std::shared_ptr<photon>& b)
{
	return a->position.z < b->position.z;
}





float aproximate_median(std::vector<std::shared_ptr<photon>>& points, int axis)
{
	auto comparator = (axis == 0) ? x_compare
		: (axis == 1) ? y_compare
		: z_compare;
	if (points.size() < 500) 
	{
		std::sort(points.begin(), points.end(), comparator);
		return axis == 0 ? points[points.size() / 2]->position.x : axis == 1 ? points[points.size() / 2]->position.y : points[points.size() / 2]->position.z;
	}
	else
	{

		std::vector<std::shared_ptr<photon>> random_photons;
		random_photons.reserve(401);
		for (int i = 0; i < 401; ++i)
		{
			int random = random_int(points.size());
			random_photons.push_back(points[random]);
		}
		std::sort(random_photons.begin(), random_photons.end(), comparator);

		return axis == 0 ? random_photons[200]->position.x : axis == 1 ? random_photons[200]->position.y : random_photons[200]->position.z;
	}

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
	else
	{
		int axis = biggest_axis(points);
		float best_split = aproximate_median(points, axis);
		std::vector<std::shared_ptr<photon>> left_objects;
		left_objects.reserve(points.size() / 2);
		std::vector<std::shared_ptr<photon>> right_objects;
		right_objects.reserve(points.size() / 2);

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
				/*if (closest_photons.capacity == closest_photons.size)
				{
					search_distance_squared = (point - photons_leaf->points[i]->position).length_squared();
				}*/
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
