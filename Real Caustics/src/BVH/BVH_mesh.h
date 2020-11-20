#ifndef BVH_MESH_H
#define BVH_MESH_H

#include <vector>
#include "../aabb.h"

struct Isect;
class ray;
class Triangle;
class Transform;

extern const float inf;

struct BVHNode_mesh
{
	virtual bool IsLeaf() = 0;
	virtual ~BVHNode_mesh() {}
};

struct BVHInner_mesh : BVHNode_mesh
{
	BVHNode_mesh *_left = nullptr;
	BVHNode_mesh *_right = nullptr;
	aabb bbox;
	BVHInner_mesh() {}
	virtual bool IsLeaf() { return false; }
	virtual ~BVHInner_mesh()
	{
		delete _left;
		delete _right;
	}
};

struct BVHLeaf_mesh : BVHNode_mesh
{
	aabb bbox;
	std::vector<unsigned int> triangle_indices;
	BVHLeaf_mesh() {}
	virtual bool IsLeaf() { return true; }
	virtual ~BVHLeaf_mesh() {}
};

struct CacheBVHNode_mesh;

struct BVH_mesh
{
	std::vector<int> tris_indices;
	std::vector<CacheBVHNode_mesh> bvh_nodes;
};
struct CacheBVHNode_mesh
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

	bool hit(const ray &r, float tmin, float tmax, Isect &hit_inf, const BVH_mesh &bvh, Triangle *triangles, MVert* vertices,
		const Transform& WorldTransformation) const;
};
struct aabb_temp_mesh
{
	unsigned int index;
	aabb bbox;
	vec3 center;

	aabb_temp_mesh(unsigned int i, aabb &box, vec3 &c) : index(i), bbox(box), center(c) {}
};

void create_cache_friendly_bvh(BVHNode_mesh *root, BVH_mesh &cache_friendly_bvh, const char *file_path);

#endif