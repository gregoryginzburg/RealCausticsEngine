#ifndef BVH_H
#define BVH_H

#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include "hittable_list.h"
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include "aabb.h"

#define WITH_LEAFS_4

class Hittable;

extern const double inf;
class BVH_Leaf : public Hittable
{
public:
	BVH_Leaf(std::vector<std::shared_ptr<Hittable>> tris) : triangles(tris) {}
	std::vector<std::shared_ptr<Hittable>> triangles;
	virtual bool hit(const ray& r, double tmin, double tmax, hit_rec& hit_inf) const
	{
		bool hit_anything = false;
		hit_rec temp_rec;
		double closest_so_far = tmax;
		for (size_t i = 0; i < triangles.size(); ++i)
		{
			if (triangles[i]->hit(r, tmin, closest_so_far, temp_rec))
			{
				hit_anything = true;
				closest_so_far = temp_rec.t;
				hit_inf = temp_rec;
			}
			
		}
		return hit_anything;
	}
	virtual bool bounding_box(aabb& output_box) const
	{
		aabb box(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
		for (size_t i = 0; i < triangles.size(); ++i)
		{
			aabb tri_bbox;
			triangles[i]->bounding_box(tri_bbox);
			box = surrounding_box(box, tri_bbox);
		}
		output_box = box;
		return true;
	}

};
class BVH_node : public Hittable
{
public:
	
	BVH_node(std::vector<aabb>& objects);

	virtual bool hit(const ray& r, double tmin, double tmax, hit_rec& hit_inf) const;
	virtual bool bounding_box(aabb& output_box) const;

public:
	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	aabb box;

};

bool BVH_node::hit(const ray& r, double tmin, double tmax, hit_rec& hit_inf) const
{
	if (!box.hit(r, tmin, tmax))
	{
		return false;
	}
	bool left_hit = left->hit(r, tmin, tmax, hit_inf);
	bool right_hit = right->hit(r, tmin, left_hit ? hit_inf.t : tmax, hit_inf);

	return left_hit || right_hit;

}
bool BVH_node::bounding_box(aabb& output_box) const
{
	output_box = box;
	return true;
}



BVH_node::BVH_node(std::vector<aabb>& objects)
{
	#ifdef WITH_LEAFS_4
	if (objects.size() < 5)
	{
		std::vector<std::shared_ptr<Hittable>> tris;
		for (size_t i = 0; i < objects.size(); ++i)
		{
			tris.push_back(objects[i].triangle);
		}
		left = std::make_shared<BVH_Leaf>(tris);
		right = std::make_shared<BVH_Leaf>(tris);
	}
	#else
	if (objects.size() == 1)
	{
		left = right = objects[0].triangle;
	}
	else if (objects.size() == 2)
	{
		left = objects[0].triangle;
		right = objects[1].triangle;
	
	}
	#endif
	else 
	{
		aabb bbox_temp(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
		for (unsigned int i = 0; i < objects.size(); ++i)
		{
			bbox_temp = surrounding_box(bbox_temp, objects[i]);
		}

		double min_cost = inf;
		double best_split = inf;
		//int best_axis;
		bool is_better_split = false;
		int count_left;
		int count_right;
		int best_left_amount;
		int best_right_amount;

		 

		double side1 = bbox_temp.max.x - bbox_temp.min.x;
		double side2 = bbox_temp.max.y - bbox_temp.min.y;
		double side3 = bbox_temp.max.z - bbox_temp.min.z;

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

		for (double testSplit = start + step; testSplit < stop - step; testSplit += step)
		{
			aabb left(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
			aabb right(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
			//int count_left = 0;
			//int count_right = 0;
			count_left = 0;
			count_right = 0;

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
			if (count_left < 1 || count_right < 1)
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
				is_better_split = true;
				best_left_amount = count_left;
				best_right_amount = count_right;
			}
				
		}
		if (!is_better_split)
		{
			std::vector<std::shared_ptr<Hittable>> tris;
			for (size_t i = 0; i < objects.size(); ++i)
			{
				tris.push_back(objects[i].triangle);
			}
			left = std::make_shared<BVH_Leaf>(tris);
			right = std::make_shared<BVH_Leaf>(tris);

		}
		else
		{



			std::vector<aabb> left_objects;
			left_objects.reserve(best_left_amount);
			std::vector<aabb> right_objects;
			right_objects.reserve(best_right_amount);

			for (size_t i = 0; i < objects.size(); ++i)
			{
				double bbox_center_axis;
				if (axis == 0) bbox_center_axis = objects[i].center.x;
				else if (axis == 1) bbox_center_axis = objects[i].center.y;
				else bbox_center_axis = objects[i].center.z;

				if (bbox_center_axis < best_split)
				{
					left_objects.push_back(objects[i]);
				}
				else
				{
					right_objects.push_back(objects[i]);
				}
			}

			right = std::make_shared<BVH_node>(right_objects);
			left = std::make_shared<BVH_node>(left_objects);
		}
	}

	aabb box_left;
	aabb box_right;

	left->bounding_box(box_left);
	right->bounding_box(box_right);
	box = surrounding_box(box_left, box_right);
}


#endif

