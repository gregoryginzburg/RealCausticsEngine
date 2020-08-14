#include "../vec3.h"
#include "../ray.h"
#include "../mesh.h"
#include "BVH_mesh.h"
#include "../Triangle.h"
#include "mesh.h"
#include <memory>
#include <fstream>
#include "Blender_definitions.h"

BVHNode_mesh *recurse(std::vector<aabb_temp_mesh> &triangle_temp_containers)
{
	if (triangle_temp_containers.size() < 4)
	{
		aabb temp(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
		BVHLeaf_mesh *leaf = new BVHLeaf_mesh;
		for (size_t i = 0; i < triangle_temp_containers.size(); ++i)
		{
			temp = surrounding_box(temp, triangle_temp_containers[i].bbox);
			leaf->triangle_indices.push_back(triangle_temp_containers[i].index);
		}
		leaf->bbox = temp;
		return leaf;
	}
	else
	{
		aabb bbox_temp(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
		for (size_t i = 0; i < triangle_temp_containers.size(); ++i)
		{
			bbox_temp = surrounding_box(bbox_temp, triangle_temp_containers[i].bbox);
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

			for (size_t i = 0; i < triangle_temp_containers.size(); ++i)
			{
				float bbox_center_axis;
				if (axis == 0)
					bbox_center_axis = triangle_temp_containers[i].center.x;
				else if (axis == 1)
					bbox_center_axis = triangle_temp_containers[i].center.y;
				else
					bbox_center_axis = triangle_temp_containers[i].center.z;

				if (bbox_center_axis < testSplit)
				{
					left = surrounding_box(left, triangle_temp_containers[i].bbox);
					count_left += 1;
				}
				else
				{
					right = surrounding_box(right, triangle_temp_containers[i].bbox);
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
			aabb temp(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
			BVHLeaf_mesh *leaf = new BVHLeaf_mesh;
			for (size_t i = 0; i < triangle_temp_containers.size(); ++i)
			{
				temp = surrounding_box(temp, triangle_temp_containers[i].bbox);
				leaf->triangle_indices.push_back(triangle_temp_containers[i].index);
			}
			leaf->bbox = temp;
			return leaf;
		}
		else
		{
			std::vector<aabb_temp_mesh> left_objects;
			left_objects.reserve(best_left_amount);
			std::vector<aabb_temp_mesh> right_objects;
			right_objects.reserve(best_right_amount);

			for (size_t i = 0; i < triangle_temp_containers.size(); ++i)
			{
				float bbox_center_axis;
				if (axis == 0)
					bbox_center_axis = triangle_temp_containers[i].center.x;
				else if (axis == 1)
					bbox_center_axis = triangle_temp_containers[i].center.y;
				else
					bbox_center_axis = triangle_temp_containers[i].center.z;

				if (bbox_center_axis < best_split)
				{
					left_objects.push_back(triangle_temp_containers[i]);
				}
				else
				{
					right_objects.push_back(triangle_temp_containers[i]);
				}
			}
			BVHInner_mesh *inner = new BVHInner_mesh;
			inner->bbox = bbox_temp;
			inner->_left = recurse(left_objects);
			inner->_right = recurse(right_objects);
			return inner;
		}
	}
}
unsigned int CountTriangles(BVHNode_mesh *root)
{
	if (!root->IsLeaf())
	{
		BVHInner_mesh *p = dynamic_cast<BVHInner_mesh *>(root);
		return CountTriangles(p->_left) + CountTriangles(p->_right);
	}
	else
	{
		BVHLeaf_mesh *p = dynamic_cast<BVHLeaf_mesh *>(root);
		return (unsigned)p->triangle_indices.size();
	}
}
unsigned int CountBoxes(BVHNode_mesh *root)
{
	if (!root->IsLeaf())
	{
		BVHInner_mesh *p = dynamic_cast<BVHInner_mesh *>(root);
		return 1 + CountBoxes(p->_left) + CountBoxes(p->_right);
	}
	else
		return 1;
}

BVHNode_mesh* Mesh::make_default_bvh()
{
	std::vector<aabb_temp_mesh> working_list;
	working_list.reserve(number_of_triangles);

	for (size_t i = 0; i < number_of_triangles; ++i)
	{
		aabb bbox = triangles[i].bounding_box(vertices);
		vec3 center = (bbox.min + bbox.max) / 2.0f;
		working_list.emplace_back(i, bbox, center);
	}

	BVHNode_mesh *root = recurse(working_list);

	return root;

	//mesh->triangles.clear();
	//mesh->triangles.shrink_to_fit();
}
void populate_cache_friendly_bvh(BVHNode_mesh *root, unsigned &idxBoxes, unsigned &idxTriList, BVH_mesh &cache_friendly_bvh)
{

	if (!root->IsLeaf())
	{
		BVHInner_mesh *p = dynamic_cast<BVHInner_mesh *>(root);
		unsigned currIdxBoxes = idxBoxes;
		cache_friendly_bvh.bvh_nodes[currIdxBoxes].bounding_box = p->bbox;

		int idxLeft = ++idxBoxes;
		populate_cache_friendly_bvh(p->_left, idxBoxes, idxTriList, cache_friendly_bvh);
		int idxRight = ++idxBoxes;
		populate_cache_friendly_bvh(p->_right, idxBoxes, idxTriList, cache_friendly_bvh);
		cache_friendly_bvh.bvh_nodes[currIdxBoxes].u.inner.idxLeft = idxLeft;
		cache_friendly_bvh.bvh_nodes[currIdxBoxes].u.inner.idxRight = idxRight;
	}

	else
	{
		BVHLeaf_mesh *p = dynamic_cast<BVHLeaf_mesh *>(root);

		unsigned currIdxBoxes = idxBoxes;
		cache_friendly_bvh.bvh_nodes[currIdxBoxes].bounding_box = p->bbox;

		unsigned count = p->triangle_indices.size();
		// highest bit set indicates a leaf node (inner node if highest bit is 0)
		cache_friendly_bvh.bvh_nodes[currIdxBoxes].u.leaf.count = 0x80000000 | count;

		cache_friendly_bvh.bvh_nodes[currIdxBoxes].u.leaf.startIndex = idxTriList;

		for (unsigned i = 0; i < p->triangle_indices.size(); ++i)
		{
			cache_friendly_bvh.tris_indices[++idxTriList] = p->triangle_indices[i];
		}
	}
}
void create_cache_friendly_bvh(BVHNode_mesh *root, BVH_mesh &cache_friendly_bvh, const char *file_path)
{
	unsigned tris = CountTriangles(root);
	int boxes = CountBoxes(root);

	cache_friendly_bvh.tris_indices.resize((size_t)tris + 1);
	cache_friendly_bvh.bvh_nodes.resize(boxes);
	unsigned idxTriList = 0;
	unsigned idxBoxes = 0;

	populate_cache_friendly_bvh(root, idxBoxes, idxTriList, cache_friendly_bvh);
	
	std::ofstream file(file_path, std::ios::binary |  std::ofstream::app);
	if (!file.is_open())
		std::cout << "ERROR OPENING A FILE";
	int size1 = cache_friendly_bvh.tris_indices.size();
	int size2 = cache_friendly_bvh.bvh_nodes.size();
	
	file.write((char *)&size1, sizeof(int));
	file.write((char *)&cache_friendly_bvh.tris_indices[0], sizeof(int) * (size_t)size1);
	file.write((char *)&size2, sizeof(int));
	file.write((char *)&cache_friendly_bvh.bvh_nodes[0], sizeof(CacheBVHNode_mesh) * size2);
	
	file.close();
	delete root;
}



bool CacheBVHNode_mesh::hit(const ray &r, float tmin, float tmax, hit_rec &hit_inf, const BVH_mesh &bvh, Triangle* triangles, MVert* vertices) const
{
	hit_rec temp_rec;
	bool hit_anything = false;
	auto closest_so_far = tmax;
	int end_index = u.leaf.startIndex + (u.leaf.count & 0x7fffffff) + 1;
	for (int i = u.leaf.startIndex + 1; i < end_index; ++i)
	{
		if (triangles[bvh.tris_indices[i]].hit(r, tmin, closest_so_far, temp_rec, vertices))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			hit_inf = temp_rec;
		}
	}
	return hit_anything;
}