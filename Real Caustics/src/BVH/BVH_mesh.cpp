#include "../vec3.h"
#include "../ray.h"
#include "../mesh.h"
#include "BVH_mesh.h"
#include "../Triangle.h"
#include <memory>

BVHNode_mesh* recurse(std::vector<aabb_temp_mesh> objects)
{
	if (objects.size() < 4)
	{
		BVHLeaf_mesh* leaf = new BVHLeaf_mesh;
		for (size_t i = 0; i < objects.size(); ++i)
		{
			leaf->triangles.push_back(objects[i].triangle);
		}
		return leaf;
	}
	else
	{
		aabb bbox_temp(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
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
			aabb left(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
			aabb right(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
			count_left = 0;
			count_right = 0;

			for (size_t i = 0; i < objects.size(); ++i)
			{
				float bbox_center_axis;
				if (axis == 0) bbox_center_axis = objects[i].center.x;
				else if (axis == 1) bbox_center_axis = objects[i].center.y;
				else bbox_center_axis = objects[i].center.z;

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
			float lside3 = left.max.z - left.min.z;

			float rside1 = right.max.x - right.min.x;
			float rside2 = right.max.y - right.min.y;
			float rside3 = right.max.z - right.min.z;

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
			BVHLeaf_mesh* leaf = new BVHLeaf_mesh;
			for (size_t i = 0; i < objects.size(); ++i)
			{
				leaf->triangles.push_back(objects[i].triangle);
			}
			return leaf;
		}
		else
		{
			std::vector<aabb_temp_mesh> left_objects;
			left_objects.reserve(best_left_amount);
			std::vector<aabb_temp_mesh> right_objects;
			right_objects.reserve(best_right_amount);

			for (size_t i = 0; i < objects.size(); ++i)
			{
				float bbox_center_axis;
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
			BVHInner_mesh* inner = new BVHInner_mesh;
			inner->bbox = bbox_temp;
			inner->_left = recurse(left_objects);
			inner->_right = recurse(right_objects);
			return inner;			
		}		
	}

}



void make_bvh_mesh(std::shared_ptr<Mesh> mesh)
{
	std::vector<aabb_temp_mesh> working_list;
	working_list.reserve(mesh->triangles.size());
	for (size_t i = 0; i < mesh->triangles.size(); ++i)
	{	
		aabb bbox = mesh->triangles[i]->bounding_box();
		vec3 center = (bbox.min + bbox.max) / 2.f;
		working_list.emplace_back(bbox, center, mesh->triangles[i]);
	}
	mesh->root = recurse(working_list);
	BVHInner_mesh* root = dynamic_cast<BVHInner_mesh*>(mesh->root);
	mesh->bounding_box = root->bbox;
	mesh->triangles.clear();
	mesh->triangles.shrink_to_fit();
}



bool hit_mesh(BVHNode_mesh* root, const ray& r, float tmin, float tmax, hit_rec& hit_inf)
{
	if (!root->IsLeaf())
	{
		BVHInner_mesh* inner = dynamic_cast<BVHInner_mesh*>(root);
		if (!inner->bbox.hit(r, tmin, tmax))
		{
			return false;
		}
		else
		{
			bool left_hit = hit_mesh(inner->_left, r, tmin, tmax, hit_inf);
			bool right_hit = hit_mesh(inner->_right, r, tmin, left_hit ? hit_inf.t : tmax, hit_inf);
			return left_hit || right_hit;
		}

	}
	else
	{
		BVHLeaf_mesh* leaf = dynamic_cast<BVHLeaf_mesh*>(root);
		return leaf->hit(r, tmin, tmax, hit_inf);
	}
}

