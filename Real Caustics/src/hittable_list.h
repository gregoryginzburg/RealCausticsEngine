#ifndef HITTABLE_LIST_H
#define	HITTABLE_LIST_H
#include <memory>
#include <vector>
#include "vec3.h"
#include "ray.h"
#include "Hit_rec.h"
#include "mesh.h"
#include "BVH\BVH_mesh.h"

class hittable_list 
{
public:
	std::vector<BVH_mesh> bvhs;
	std::vector<std::shared_ptr<Mesh>> meshes;

public:
	hittable_list() {}

	bool hit(const ray& r, float tmin, float tmax, hit_rec& hit_inf) const
	{
		hit_rec temp_rec;
		bool hit_anything = false;
		auto closest_so_far = tmax;
		//тут проходим по указателям на каждый объект класса hittable
		for (int i = 0; i < meshes.size(); ++i)
		{
			if (hit_mesh_bvh(bvhs[i], 0, r, tmin, closest_so_far, temp_rec, meshes[i]))
			{
				hit_anything = true;
				closest_so_far = temp_rec.t;
				hit_inf = temp_rec;
			}
		}
		return hit_anything;
	}
	void add(const BVH_mesh& bvh, const std::shared_ptr<Mesh>& mesh)
	{
		bvhs.push_back(bvh);
		meshes.push_back(mesh);
	}


	
};

#endif
