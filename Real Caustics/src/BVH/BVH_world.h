#ifndef BVH_WORLD_H
#define BVH_WORLD_H

#include "../vec3.h"
#include "../ray.h"
#include "../Hit_rec.h"
#include "../utils.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include "../aabb.h"
#include "../mesh.h"
#include "BVH_mesh.h"
#include "../Triangle.h"

/*
class Hittable;
class hittable_list;
extern const float inf;


struct BVHNode_world {
	virtual bool IsLeaf() = 0;
	virtual ~BVHNode_world() {}

};

struct BVHInner_world : BVHNode_world
{
	BVHNode_world* _left = nullptr;
	BVHNode_world* _right = nullptr;
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
	std::vector<BVHNode_mesh*> meshes;
	BVHLeaf_world() {}
	virtual bool IsLeaf() { return true; }
	bool hit(const ray& r, float tmin, float tmax, hit_rec& hit_inf)
	{
		bool hit_anything = false;
		hit_rec temp_rec;
		float closest_so_far = tmax;
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			if (!meshes[i]->IsLeaf())
			{
				BVHInner_mesh* mesh_root = dynamic_cast<BVHInner_mesh*>(meshes[i]);
				if (hit_mesh(mesh_root, r, tmin, closest_so_far, temp_rec))
				{
					hit_anything = true;
					closest_so_far = temp_rec.t;
					hit_inf = temp_rec;
				}
			}
			else
			{
				BVHLeaf_mesh* mesh_root = dynamic_cast<BVHLeaf_mesh*>(meshes[i]);
				if (mesh_root->hit(r, tmin, closest_so_far, temp_rec))
				{
					hit_anything = true;
					closest_so_far = temp_rec.t;
					hit_inf = temp_rec;
				}
			}
			
		}
		return hit_anything;
	}
	virtual ~BVHLeaf_world()
	{
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			delete meshes[i];
		}
	}
};
struct aabb_temp_world
{
	aabb bbox;
	std::shared_ptr<Mesh> mesh;
	vec3 center;
	aabb_temp_world(aabb& box, vec3& c, std::shared_ptr<Mesh> m) : bbox(box), center(c), mesh(m) {}
};

void make_bvh_world(hittable_list& objects);
bool hit_world(BVHNode_world* root, const ray& r, float tmin, float tmax, hit_rec& hit_inf);
*/
#endif