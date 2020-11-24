#include "../vec3.h"
#include "../ray.h"
#include "../mesh.h"
#include "BVH_mesh.h"
#include "../Triangle.h"
#include "../mesh.h"
#include <memory>
#include <fstream>
#include "../Blender_definitions.h"
#include "../matrix.h"



bool x_comparator(const aabb_temp_mesh& a, const aabb_temp_mesh& b)
{
	return a.center.x < b.center.x;
}
bool y_comparator(const aabb_temp_mesh& a, const aabb_temp_mesh& b)
{
	return a.center.y < b.center.y;
}
bool z_comparator(const aabb_temp_mesh& a, const aabb_temp_mesh& b)
{
	return a.center.z < b.center.z;
}


BVHNode_mesh *recurse(std::vector<aabb_temp_mesh>& triangles_info, int start, int end)
{
	aabb Bbox;
	for (int i = start; i < end; ++i)
	{
		Bbox = Union(Bbox, triangles_info[i].bbox);
	}

	int nTriangles = end - start;
	// One Triangle left
	if (nTriangles == 1)
	{
		BVHLeaf_mesh* leaf = new BVHLeaf_mesh;
		leaf->bbox = Bbox;
		leaf->triangle_indices.push_back(triangles_info[start].index);
		return leaf;
	}

	aabb CentroidsBbox;
	for (int i = start; i < end; ++i)
	{
		CentroidsBbox = Union(CentroidsBbox, triangles_info[i].center);
	}
	int dim = CentroidsBbox.MaximumExtent();

	int mid = (start + end) / 2;
	// All Triangles are packed in the same place
	if (CentroidsBbox.max[dim] == CentroidsBbox.min[dim])
	{
		BVHLeaf_mesh* leaf = new BVHLeaf_mesh;
		leaf->bbox = Bbox;
		for (int i = start; i < end; ++i)
		{
			leaf->triangle_indices.push_back(triangles_info[i].index);
		}
		return leaf;
	}
	// Partition
	auto comparator = dim == 0 ? x_comparator : dim == 1 ? y_comparator : z_comparator;
	if (nTriangles == 2)
	{
		if (!comparator(triangles_info[start], triangles_info[end - 1]))
		{
			std::swap(triangles_info[start], triangles_info[end - 1]);
		}
	}
	else
	{
		constexpr int nBuckets = 12;
		BucketInfo buckets[nBuckets];

		for (int i = start; i < end; ++i)
		{
			int b = nBuckets * CentroidsBbox.Offset(triangles_info[i].center)[dim];
			if (b == nBuckets)
				b = nBuckets - 1;

			buckets[b].count++;
			buckets[b].bbox = Union(buckets[b].bbox, triangles_info[i].bbox);
		}
		
		// Computing Costs
		float cost[nBuckets - 1];
		for (int i = 0; i < nBuckets - 1; ++i)
		{
			aabb bbox0, bbox1;
			int count0 = 0, count1 = 0;
			for (int j = 0; j <= i; ++j)
			{
				bbox0 = Union(bbox0, buckets[j].bbox);
				count0 += buckets[j].count;
			}
			for (int j = i + 1; j < nBuckets; ++j)
			{
				bbox1 = Union(bbox1, buckets[j].bbox);
				count1 += buckets[j].count;
			}

			cost[i] = 0.16874 + (count0 * bbox0.SurfaceArea() + count1 * bbox1.SurfaceArea()) / Bbox.SurfaceArea();
		}

		// Find split with minimum cost
		float minCost = cost[0];
		float minCostBucket = 0;
		for (int i = 0; i < nBuckets - 1; ++i)
		{
			if (cost[i] < minCost)
			{
				minCost = cost[i];
				minCostBucket = i;
			}
		}
		float leaf_cost = nTriangles;

		// Create leaf or interior node
		if (minCost < leaf_cost)
		{
			// create interior node (partition triangles based on slelected bucket
			aabb_temp_mesh* triMid = std::partition(
				&triangles_info[start], &triangles_info[end - 1] + 1,
				[=](const aabb_temp_mesh& tri)
				{
					int b = nBuckets * CentroidsBbox.Offset(tri.center)[dim];
					if (b == nBuckets)
						b = nBuckets - 1;
					return b <= minCostBucket;
				}
			);
			mid = triMid - &triangles_info[0];
		}
		else
		{
			// create leaf node
			BVHLeaf_mesh* leaf = new BVHLeaf_mesh;
			leaf->bbox = Bbox;
			for (int i = start; i < end; ++i)
			{
				leaf->triangle_indices.push_back(triangles_info[i].index);
			}
			return leaf;
		}		
	}
	BVHInner_mesh* inner_node = new BVHInner_mesh;
	inner_node->bbox = Bbox;
	inner_node->_left = recurse(triangles_info, start, mid);
	inner_node->_right = recurse(triangles_info, mid, end);
	return inner_node;
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
		aabb bbox = triangles[i].bounding_box(vertices, WorldTransformation);
		vec3 center = (bbox.min + bbox.max) / 2.0f;
		working_list.emplace_back(i, bbox, center);
	}

	BVHNode_mesh *root = recurse(working_list, 0, working_list.size());

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

	/*
	std::ofstream file(file_path, std::ios::binary |  std::ofstream::app);
	if (!file.is_open())
		std::cout << "ERROR OPENING A FILE";
	int size1 = cache_friendly_bvh.tris_indices.size();
	int size2 = cache_friendly_bvh.bvh_nodes.size();
	
	file.write((char *)&size1, sizeof(int));
	file.write((char *)&cache_friendly_bvh.tris_indices[0], sizeof(int) * (size_t)size1);
	file.write((char *)&size2, sizeof(int));
	file.write((char *)&cache_friendly_bvh.bvh_nodes[0], sizeof(CacheBVHNode_mesh) * size2);
	
	file.close();*/
	delete root;
}



bool CacheBVHNode_mesh::hit(const ray &r, float tmin, float tmax, Isect &hit_inf, const BVH_mesh &bvh, Triangle* triangles, MVert* vertices, 
	const Transform& WorldTransformation) const
{
	Isect temp_rec;
	bool hit_anything = false;
	auto closest_so_far = tmax;
	int end_index = u.leaf.startIndex + (u.leaf.count & 0x7fffffff) + 1;

	for (int i = u.leaf.startIndex + 1; i < end_index; ++i)
	{
		if (triangles[bvh.tris_indices[i]].hit(r, tmin, closest_so_far, temp_rec, vertices, WorldTransformation, true))
		{
			hit_anything = true;
			closest_so_far = temp_rec.distance;
			hit_inf = temp_rec;
		}
	}
	return hit_anything;
}


float CostOfTraversing(const BVH_mesh& bvh, int index)
{
	const CacheBVHNode_mesh& node = bvh.bvh_nodes[index];

	if (node.u.leaf.count & 0x80000000)
	{
		//leaf
		return static_cast<float>(node.u.leaf.count & 0x7fffffff);
	}
	else
	{
		float main_bbox_area = node.bounding_box.SurfaceArea();

		float left_area = bvh.bvh_nodes[node.u.inner.idxLeft].bounding_box.SurfaceArea();
		float right_area = bvh.bvh_nodes[node.u.inner.idxRight].bounding_box.SurfaceArea();

		return 0.16874 + (CostOfTraversing(bvh, node.u.inner.idxLeft) * left_area + CostOfTraversing(bvh, node.u.inner.idxRight) * right_area) / main_bbox_area;
	}
	
}