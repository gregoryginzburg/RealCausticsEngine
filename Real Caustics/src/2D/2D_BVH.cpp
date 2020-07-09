#include "2D_BVH.h"
#include "2D_triangle.h"
#include "2D_aabb.h"
#include "UV_Map.h"
#include <vector>
#include <memory>
#include <cmath> 
#include <iostream>
extern const float inf;
BVHNode* recurse_bvh_2(std::vector<aabb2_temp>& objects)
{
	if (objects.size() < 4)
	{
		BVHLeaf* leaf = new BVHLeaf;
		for (size_t i = 0; i < objects.size(); ++i)
		{
			leaf->triangles.push_back(objects[i].triangle);
		}
		return leaf;
	}

	else
	{
		aabb2 bbox_temp(vec2(inf, inf), vec2(-inf, -inf));
		for (size_t i = 0; i < objects.size(); ++i)
		{
			bbox_temp = surrounding_box(bbox_temp, objects[i].bbox);
		}

		float min_cost = inf;
		float best_split = inf;
		bool is_better_split = false;
		int count_left;
		int count_right;
		int best_left_amount;
		int best_right_amount;



		float side1 = bbox_temp.max.x - bbox_temp.min.x;
		float side2 = bbox_temp.max.y - bbox_temp.min.y;

		float max_side = std::fmax(side1, side2);
		int axis = max_side == side1 ? 0 : 1;

		float start;
		float stop;


		if (axis == 0)
		{
			start = bbox_temp.min.x;
			stop = bbox_temp.max.x;
		}
		else
		{
			start = bbox_temp.min.y;
			stop = bbox_temp.max.y;
		}

		float step = (stop - start) / 16.f;

		for (float testSplit = start + step; testSplit < stop - step; testSplit += step)
		{
			aabb2 left(vec2(inf, inf), vec2(-inf, -inf));
			aabb2 right(vec2(inf, inf), vec2(-inf, -inf));
			count_left = 0;
			count_right = 0;

			for (size_t i = 0; i < objects.size(); ++i)
			{
				float bbox_center_axis;
				if (axis == 0) bbox_center_axis = objects[i].center.x;
				else bbox_center_axis = objects[i].center.y;

				if (bbox_center_axis < testSplit)
				{
					left = surrounding_box(left, objects[i].bbox);
					count_left += 1;
				}
				else
				{
					right = surrounding_box(right, objects[i].bbox);
					count_right += 1;
				}
			}
			if (count_left < 1 || count_right < 1)
			{
				continue;
			}
			float lside1 = left.max.x - left.min.x;
			float lside2 = left.max.y - left.min.y;

			float rside1 = right.max.x - right.min.x;
			float rside2 = right.max.y - right.min.y;

			float lsurface_area = lside1 * lside2;
			float rsurface_area = rside1 * rside2;

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
			BVHLeaf* leaf = new BVHLeaf;
			for (size_t i = 0; i < objects.size(); ++i)
			{
				leaf->triangles.push_back(objects[i].triangle);
			}
			return leaf;
		}
		else
		{
			std::vector<aabb2_temp> left_objects;
			left_objects.reserve(best_left_amount);
			std::vector<aabb2_temp> right_objects;
			right_objects.reserve(best_right_amount);

			for (size_t i = 0; i < objects.size(); ++i)
			{
				float bbox_center_axis;
				if (axis == 0) bbox_center_axis = objects[i].center.x;
				else bbox_center_axis = objects[i].center.y;

				if (bbox_center_axis < best_split)
				{
					left_objects.push_back(objects[i]);
				}
				else
				{
					right_objects.push_back(objects[i]);
				}
			}

			BVHInner* inner = new BVHInner;
			inner->bbox = bbox_temp;
			inner->_left = recurse_bvh_2(left_objects);
			inner->_right = recurse_bvh_2(right_objects);
			return inner;
		}
	}
}

BVHNode* build_bvh_2(UV_Map& uv_map)
{
	std::vector<aabb2_temp> temp_bboxes;
	temp_bboxes.reserve(uv_map.triangles.size());
	for (size_t i = 0; i < uv_map.triangles.size(); ++i)
	{
		aabb2 bbox = uv_map.triangles[i]->bounding_box();
		vec2 center = (bbox.min + bbox.max) / 2.f;
		temp_bboxes.emplace_back(bbox, center, uv_map.triangles[i]);
	}
	return recurse_bvh_2(temp_bboxes);
}

bool hit(BVHNode* root, vec2 point, hit_rec_2& rec)
{
	if (!root->IsLeaf())
	{
		BVHInner* inner = dynamic_cast<BVHInner*>(root);
		if (!inner->bbox.hit(point))
		{
			return false;
		}
		else
		{
			bool left_hit = hit(inner->_left, point, rec);
			bool right_hit = hit(inner->_right, point, rec);
			if (left_hit && right_hit)
			{
				std::cout << "Triangles are overlapping!" << std::endl;
			}
			else
			{
				return left_hit || right_hit;
			}
		}

	}
	else
	{
		BVHLeaf* leaf = dynamic_cast<BVHLeaf*>(root);
		return leaf->hit(point, rec);
	}
}