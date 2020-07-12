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


struct BVHNode_mesh {
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
	std::vector<int> triangle_indices;
	BVHLeaf_mesh() {}
	virtual bool IsLeaf() { return true; }
	bool hit(const ray& r, float tmin, float tmax, hit_rec& hit_inf)
	{
		bool hit_anything = false;
		hit_rec temp_rec;
		float closest_so_far = tmax;
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
	virtual ~BVHLeaf_mesh() 
	{

	}
};
struct aabb_temp_mesh
{
	aabb bbox;
	std::shared_ptr<Triangle> triangle;
	vec3 center;
	aabb_temp_mesh(aabb& box, vec3& c, std::shared_ptr<Triangle> t) : bbox(box), center(c), triangle(t) {}
};

void make_bvh_mesh(std::shared_ptr<Mesh> mesh);
bool hit_mesh(BVHNode_mesh* root, const ray& r, float tmin, float tmax, hit_rec& hit_inf);

#endif