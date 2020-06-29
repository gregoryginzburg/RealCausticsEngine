#include <memory>
#include <vector>
#include <algorithm>
#include "hittable.h"
#include "vec3.h"
#include "ray.h"
#include "aabb.h"
#include "Bvh_Sah_Construction.h"
const double inf = std::numeric_limits<double>::infinity();



//aabb boxes
std::shared_ptr<BVH_node> create_bvh(std::vector<aabb>& objects)
{
	if (objects.size() < 4)
	{
		std::shared_ptr<BVHLeaf> leaf = std::make_shared<BVHLeaf>();
		for (unsigned int i = 0; i < objects.size(); ++i)
		{
			leaf->triangles.push_back(objects[i].triangle);
		}
		return leaf;
	}

	aabb bbox_temp(vec3(inf,inf,inf), vec3(-inf,-inf,-inf));
	for (unsigned int i = 0; i < objects.size(); ++i)
	{
		bbox_temp = surrounding_box(bbox_temp, objects[i]);
	}
	double side1 = bbox_temp.max.x - bbox_temp.min.x;
	double side2 = bbox_temp.max.y - bbox_temp.min.y;
	double side3 = bbox_temp.max.z - bbox_temp.min.z;
	
	double surface_area = side1 * side2 + side2 * side3 + side3 * side1;
	double min_cost = surface_area * objects.size();
	double best_split = inf;

	bool isbettercost = false;

	double max_side = std::fmax(std::fmax(side1, side2), side3);
	double axis = max_side == side1 ? 0 : max_side == side2 ? 1 : 2;

	double start;
	double stop;

	if (axis == 0)
	{
		start = bbox_temp.min.x;
		stop = bbox_temp.max.x;
	}
	// Y-axis
	else if (axis == 1) 
	{
		start = bbox_temp.min.y;
		stop = bbox_temp.max.y;
	}
	// Z-axis
	else
	{
		start = bbox_temp.min.z;
		stop = bbox_temp.max.z;
	}
	double step = (stop - start) / 16.;

	for (float testSplit = start + step; testSplit < stop - step; testSplit += step)
	{
		aabb left(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
		aabb right(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
		int count_left = 0;
		int count_right = 0;
		for (unsigned int i = 0; i < objects.size(); ++i)
		{
			double bbox_center_axis;
			if (axis == 0) bbox_center_axis = objects[i].center.x;
			else if (axis == 1) bbox_center_axis = objects[i].center.y;
			else bbox_center_axis = objects[i].center.z;

			if (bbox_center_axis < testSplit)
			{
				left = surrounding_box(left, objects[i]);
				count_left += 1;
			}
			else
			{
				right = surrounding_box(right, objects[i]);
				count_right += 1;

			}
		}
		if (count_left < 2 || count_right < 2)
		{
			continue;
		}

		double lside1 = left.max.x - left.min.x;
		double lside2 = left.max.y - left.min.y;
		double lside3 = left.max.z - left.min.z;

		double rside1 = right.max.x - right.min.x;
		double rside2 = right.max.y - right.min.y;
		double rside3 = right.max.z - right.min.z;

		double lsurface_area = lside1 * lside2 + lside2 * lside3 + lside3 * lside1;
		double rsurface_area = rside1 * rside2 + rside2 * rside3 + rside3 * rside1;

		double total_cost = lsurface_area * count_left + rsurface_area * count_right;

		if (total_cost < min_cost)
		{
			min_cost = total_cost;
			best_split = testSplit;
			isbettercost = true;
		}

	}

	if (!isbettercost)
	{
		std::shared_ptr<BVHLeaf> leaf = std::make_shared<BVHLeaf>();
		for (int i = 0; i < objects.size(); ++i)
		{
			leaf->triangles.push_back(objects[i].triangle);	
		}
		return leaf;
	}

	std::vector<aabb> left_objects;
	std::vector<aabb> right_objects;
	aabb left(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
	aabb right(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));

	for (int i = 0; i < objects.size(); ++i)
	{
		double bbox_center_axis;
		if (axis == 0) bbox_center_axis = objects[i].center.x;
		else if (axis == 1) bbox_center_axis = objects[i].center.y;
		else bbox_center_axis = objects[i].center.z;
	
		if (bbox_center_axis < best_split)
		{
			left = surrounding_box(left, objects[i]);
			left_objects.push_back(objects[i]);
		}
		else
		{
			right = surrounding_box(right, objects[i]);
			right_objects.push_back(objects[i]);

		}
	}


	std::shared_ptr<BVH_inner> inner = std::make_shared<BVH_inner>();
	inner->left = create_bvh(left_objects);
	inner->left->box = left;
	
	inner->right = create_bvh(right_objects);
	inner->right->box = right;



	return inner;



}