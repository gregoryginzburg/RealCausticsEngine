#ifndef BVH_WORLD_H
#define BVH_WORLD_H

#include <vector>
#include "../aabb.h"

struct Isect;
class ray;
class Triangle;
class Mesh;

extern const float inf;

struct BVHNode_world
{
	virtual bool IsLeaf() = 0;
	virtual ~BVHNode_world() {}
};

struct BVHInner_world : BVHNode_world
{
	BVHNode_world *_left = nullptr;
	BVHNode_world *_right = nullptr;
	aabb bbox;
	BVHInner_world() {}
	virtual bool IsLeaf() { return false; }
	virtual ~BVHInner_world()
	{
		delete _left;
		delete _right;
	}
};

struct BVHLeaf_world : BVHNode_world
{
	aabb bbox;
	std::vector<unsigned int> mesh_indices;
	BVHLeaf_world() {}
	virtual bool IsLeaf() { return true; }
	virtual ~BVHLeaf_world() {}
};

struct CacheBVHNode_world;
struct BVH_world
{
	std::vector<int> mesh_indices;
	std::vector<CacheBVHNode_world> bvh_nodes;
};
struct CacheBVHNode_world
{
	aabb bounding_box;
	union {
		// inner node - stores indexes to array of CacheFriendlyBVHNode
		struct
		{
			unsigned idxLeft;
			unsigned idxRight;
		} inner;
		// leaf node: stores triangle count and starting index in triangle list
		struct
		{
			unsigned count; // Top-most bit set, leafnode if set, innernode otherwise
			unsigned startIndex;
		} leaf;
	} u;
	bool hit(const ray &r, float tmin, float tmax, Isect &hit_inf, const BVH_world &bvh, Mesh *meshes) const;
};
struct aabb_temp_world
{
	unsigned int index;
	aabb bbox;
	vec3 center;

	int cost;

	aabb_temp_world(unsigned int i, aabb &box, vec3 &c, int co) : index(i), bbox(box), center(c), cost(co) {}
};


void create_cache_friendly_bvh(BVHNode_world *root, BVH_world &cache_friendly_bvh, const char *file_path);

#endif