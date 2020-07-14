#ifndef BVH_MESH_H
#define BVH_MESH_H

#include "../vec3.h"
#include "../ray.h"
#include "../Hit_rec.h"
#include "../utils.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include "../aabb.h"
#include "../mesh.h"
#include "../Triangle.h"



extern const float inf;


struct BVHNode_mesh 
{
	virtual bool IsLeaf() = 0;
	virtual ~BVHNode_mesh() {}
	
};

struct BVHInner_mesh : BVHNode_mesh
{
	BVHNode_mesh* _left = nullptr;
	BVHNode_mesh* _right = nullptr;
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
	virtual ~BVHLeaf_mesh() 
	{
												
	}
};
struct CacheBVHNode;
struct BVH_mesh
{
	std::vector<int> tris_indices;
	std::vector<CacheBVHNode> bvh_nodes;
};
struct CacheBVHNode
{
	aabb bounding_box;
	union {
		// inner node - stores indexes to array of CacheFriendlyBVHNode
		struct {
			unsigned idxLeft;
			unsigned idxRight;
		} inner;
		// leaf node: stores triangle count and starting index in triangle list
		struct {
			unsigned count; // Top-most bit set, leafnode if set, innernode otherwise
			unsigned startIndex;
		} leaf;
	} u;
	bool hit(const ray& r, float tmin, float tmax, hit_rec& hit_inf, const BVH_mesh& bvh, const std::shared_ptr<Mesh>& mesh) const
	{
		hit_rec temp_rec;
		bool hit_anything = false;
		auto closest_so_far = tmax;
		int end_index = u.leaf.startIndex + (u.leaf.count & 0x7fffffff) + 1;
		for (int i = u.leaf.startIndex + 1; i < end_index; ++i)
		{
			if (mesh->triangles[bvh.tris_indices[i]]->hit(r, tmin, closest_so_far, temp_rec))
			{
				hit_anything = true;
				closest_so_far = temp_rec.t;
				hit_inf = temp_rec;
			}
		}
		return hit_anything;
	}
};
struct aabb_temp_mesh
{
	aabb bbox;
	std::shared_ptr<Triangle> triangle;
	vec3 center;
	unsigned int index;
	aabb_temp_mesh(aabb& box, vec3& c, std::shared_ptr<Triangle> t, unsigned int i) : bbox(box), center(c), triangle(t), index(i){}
};


bool hit_mesh(BVHNode_mesh* root, const ray& r, float tmin, float tmax, hit_rec& hit_inf);
void update_bvh(bool was_changed, BVH_mesh& BVH, std::shared_ptr<Mesh> mesh, const char* file_path);
bool hit_mesh_bvh(const BVH_mesh& BVH, unsigned index, const ray& r, float tmin, float tmax, hit_rec& hit_inf, const std::shared_ptr<Mesh>& mesh);

#endif