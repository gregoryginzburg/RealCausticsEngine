#ifndef SCENE_H
#define SCENE_H


#include "BVH/BVH_world.h"
class Mesh;


class Scene
{
public:
	BVHNode_world *make_default_bvh();

	void update_bvh(bool was_changed, const char *file_path);

	bool hit(const ray &r, float tmin, float tmax, hit_rec &hit_inf, int index) const;

	void init_meshes(long long* meshes_pointers, unsigned int* meshes_number_of_verts, unsigned int* meshes_number_of_tris);

public:
	int number_of_photons;
	int number_of_closest_photons;
	float search_radius;

	unsigned int number_of_meshes;

	Mesh *meshes;

	BVH_world BVH;
};

#endif // !SCENE_H
