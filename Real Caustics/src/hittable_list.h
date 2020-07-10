#ifndef HITTABLE_LIST_H
#define	HITTABLE_LIST_H
#include <memory>
#include <vector>
#include "vec3.h"
#include "ray.h"
#include "Hit_rec.h"
#include "mesh.h"
#include "BVH\BVH_world.h"

class hittable_list 
{
public:
	std::vector<std::shared_ptr<Mesh>> objects;
	BVHNode_world* root = nullptr;
public:
	hittable_list() {}

	bool hit(const ray& r, float tmin, float tmax, hit_rec& hit_inf) const
	{
		return hit_world(root, r, tmin, tmax, hit_inf);
	}
	void create_bvh()
	{
		make_bvh_world(*this);
	}
	void add(std::shared_ptr<Mesh> mesh)
	{
		objects.push_back(mesh);
	}
	void delete_world()
	{
		delete root;
	}
	
};

#endif
