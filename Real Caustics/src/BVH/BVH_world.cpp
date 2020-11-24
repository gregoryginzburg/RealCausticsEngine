#include "../vec3.h"
#include "../ray.h"
#include "../mesh.h"
#include "BVH_world.h"
#include "../Triangle.h"
#include "../mesh.h"
#include "../Scene.h"
#include <memory>
#include <fstream>

bool x_comparator(const aabb_temp_world& a, const aabb_temp_world& b)
{
	return a.center.x < b.center.x;
}
bool y_comparator(const aabb_temp_world& a, const aabb_temp_world& b)
{
	return a.center.y < b.center.y;
}
bool z_comparator(const aabb_temp_world& a, const aabb_temp_world& b)
{
	return a.center.z < b.center.z;
}


BVHNode_world* recurse(std::vector<aabb_temp_world>& meshes_info, int start, int end)
{
	aabb Bbox;
	for (int i = start; i < end; ++i)
	{
		Bbox = Union(Bbox, meshes_info[i].bbox);
	}

	int nMeshes = end - start;
	// One Triangle left
	if (nMeshes == 1)
	{
		BVHLeaf_world* leaf = new BVHLeaf_world;
		leaf->bbox = Bbox;
		leaf->mesh_indices.push_back(meshes_info[start].index);
		return leaf;
	}

	aabb CentroidsBbox;
	for (int i = start; i < end; ++i)
	{
		CentroidsBbox = Union(CentroidsBbox, meshes_info[i].center);
	}
	int dim = CentroidsBbox.MaximumExtent();

	int mid = (start + end) / 2;
	// All Triangles are packed in the same place
	if (CentroidsBbox.max[dim] == CentroidsBbox.min[dim])
	{
		BVHLeaf_world* leaf = new BVHLeaf_world;
		leaf->bbox = Bbox;
		for (int i = start; i < end; ++i)
		{
			leaf->mesh_indices.push_back(meshes_info[i].index);
		}
		return leaf;
	}
	// Partition
	auto comparator = dim == 0 ? x_comparator : dim == 1 ? y_comparator : z_comparator;
	if (nMeshes == 2)
	{
		if (!comparator(meshes_info[start], meshes_info[end - 1]))
		{
			std::swap(meshes_info[start], meshes_info[end - 1]);
		}
	}
	else
	{
		constexpr int nBuckets = 12;
		BucketInfo buckets[nBuckets];

		for (int i = start; i < end; ++i)
		{
			int b = nBuckets * CentroidsBbox.Offset(meshes_info[i].center)[dim];
			if (b == nBuckets)
				b = nBuckets - 1;

			buckets[b].count++;
			buckets[b].bbox = Union(buckets[b].bbox, meshes_info[i].bbox);
			buckets[b].SumTraversCosts += meshes_info[i].cost;
		}

		// Computing Costs
		float cost[nBuckets - 1];
		for (int i = 0; i < nBuckets - 1; ++i)
		{
			aabb bbox0, bbox1;
			int count0 = 0, count1 = 0;
			int TraversCosts0 = 0, TraversCosts1 = 0;
			for (int j = 0; j <= i; ++j)
			{
				bbox0 = Union(bbox0, buckets[j].bbox);
				count0 += buckets[j].count;
				TraversCosts0 += buckets[j].SumTraversCosts;
			}
			for (int j = i + 1; j < nBuckets; ++j)
			{
				bbox1 = Union(bbox1, buckets[j].bbox);
				count1 += buckets[j].count;
				TraversCosts1 += buckets[j].SumTraversCosts;
			}

			cost[i] = 0.16874 + (bbox0.SurfaceArea() * TraversCosts0 + bbox1.SurfaceArea() * TraversCosts1) / Bbox.SurfaceArea();
		}

		// Find split with minimum cost
		float minCost = cost[0];
		float minCostBucket = 0;

		// Calculate leaf travers cost and minimum cost for splitting
		float leaf_cost = 0;
		for (int i = 0; i < nBuckets - 1; ++i)
		{
			leaf_cost += buckets[i].SumTraversCosts;
			if (cost[i] < minCost)
			{
				minCost = cost[i];
				minCostBucket = i;
			}
		}

		// Create leaf or interior node
		if (minCost < leaf_cost)
		{
			// create interior node (partition triangles based on slelected bucket
			aabb_temp_world* triMid = std::partition(
				&meshes_info[start], &meshes_info[end - 1] + 1,
				[=](const aabb_temp_world& tri)
				{
					int b = nBuckets * CentroidsBbox.Offset(tri.center)[dim];
					if (b == nBuckets)
						b = nBuckets - 1;
					return b <= minCostBucket;
				}
			);
			mid = triMid - &meshes_info[0];
		}
		else
		{
			// create leaf node
			BVHLeaf_world* leaf = new BVHLeaf_world;
			leaf->bbox = Bbox;
			for (int i = start; i < end; ++i)
			{
				leaf->mesh_indices.push_back(meshes_info[i].index);
			}
			return leaf;
		}
	}
	BVHInner_world* inner_node = new BVHInner_world;
	inner_node->bbox = Bbox;
	inner_node->_left = recurse(meshes_info, start, mid);
	inner_node->_right = recurse(meshes_info, mid, end);
	return inner_node;
}

unsigned int CountTriangles(BVHNode_world *root)
{
	if (!root->IsLeaf())
	{
		BVHInner_world *p = dynamic_cast<BVHInner_world *>(root);
		return CountTriangles(p->_left) + CountTriangles(p->_right);
	}
	else
	{
		BVHLeaf_world *p = dynamic_cast<BVHLeaf_world *>(root);
		return (unsigned)p->mesh_indices.size();
	}
}
unsigned int CountBoxes(BVHNode_world *root)
{
	if (!root->IsLeaf())
	{
		BVHInner_world *p = dynamic_cast<BVHInner_world *>(root);
		return 1 + CountBoxes(p->_left) + CountBoxes(p->_right);
	}
	else
		return 1;
}

BVHNode_world* Scene::make_default_bvh()
{
	std::vector<aabb_temp_world> working_list;
	working_list.reserve(number_of_meshes);

	for (size_t i = 0; i < number_of_meshes; ++i)
	{
		aabb bbox = meshes[i].bounding_box;
		vec3 center = (bbox.min + bbox.max) / 2.0f;
		working_list.emplace_back(i, bbox, center, meshes[i].BVH.cost);
	}

	BVHNode_world *root = recurse(working_list, 0, working_list.size());

	return root;

	//mesh->triangles.clear();
	//mesh->triangles.shrink_to_fit();
}
void populate_cache_friendly_bvh(BVHNode_world *root, unsigned &idxBoxes, unsigned &idxTriList, BVH_world &cache_friendly_bvh)
{

	if (!root->IsLeaf())
	{
		BVHInner_world *p = dynamic_cast<BVHInner_world *>(root);
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
		BVHLeaf_world *p = dynamic_cast<BVHLeaf_world *>(root);

		unsigned currIdxBoxes = idxBoxes;
		cache_friendly_bvh.bvh_nodes[currIdxBoxes].bounding_box = p->bbox;

		unsigned count = p->mesh_indices.size();
		// highest bit set indicates a leaf node (inner node if highest bit is 0)
		cache_friendly_bvh.bvh_nodes[currIdxBoxes].u.leaf.count = 0x80000000 | count;

		cache_friendly_bvh.bvh_nodes[currIdxBoxes].u.leaf.startIndex = idxTriList;

		for (unsigned i = 0; i < p->mesh_indices.size(); ++i)
		{
			cache_friendly_bvh.mesh_indices[++idxTriList] = p->mesh_indices[i];
		}
	}
}
void create_cache_friendly_bvh(BVHNode_world *root, BVH_world &cache_friendly_bvh, const char *file_path)
{
	unsigned tris = CountTriangles(root);
	int boxes = CountBoxes(root);

	cache_friendly_bvh.mesh_indices.resize((size_t)tris + 1);
	cache_friendly_bvh.bvh_nodes.resize(boxes);
	unsigned idxTriList = 0;
	unsigned idxBoxes = 0;

	populate_cache_friendly_bvh(root, idxBoxes, idxTriList, cache_friendly_bvh);
	/*
	std::ofstream file(file_path, std::ios::binary);
	int size1 = cache_friendly_bvh.mesh_indices.size();
	int size2 = cache_friendly_bvh.bvh_nodes.size();
	
	file.write((char *)&size1, sizeof(int));
	file.write((char *)&cache_friendly_bvh.mesh_indices[0], sizeof(int) * (size_t)size1);
	file.write((char *)&size2, sizeof(int));
	file.write((char *)&cache_friendly_bvh.bvh_nodes[0], sizeof(CacheBVHNode_world) * size2);
	
	file.close();*/
	delete root;
}



bool CacheBVHNode_world::hit(const ray &r, float tmin, float tmax, Isect &hit_inf, const BVH_world &bvh, Mesh *meshes) const
{
	Isect temp_rec;
	bool hit_anything = false;
	auto closest_so_far = tmax;
	int end_index = u.leaf.startIndex + (u.leaf.count & 0x7fffffff) + 1;
	for (int i = u.leaf.startIndex + 1; i < end_index; ++i)
	{
		if (meshes[bvh.mesh_indices[i]].hit(r, tmin, closest_so_far, temp_rec, 0))
		{
			hit_anything = true;
			closest_so_far = temp_rec.distance;
			hit_inf = temp_rec;
		}
	}
	return hit_anything;
}
