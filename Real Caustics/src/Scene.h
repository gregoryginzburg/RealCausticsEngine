#ifndef SCENE_H
#define SCENE_H


#include "BVH/BVH_world.h"
#include "Camera.h"
#include "photon_map.h"
#include "materials.h"
#include "Light_list.h"
#include <vector>
#include <memory>

class Mesh;
class Python_Light;
class Python_Material;

class Scene
{
public:
	Scene() {}
public:
	BVHNode_world *make_default_bvh();

	void update_bvh(bool was_changed, const char *file_path);

	bool hit(const ray &r, float tmin, float tmax, hit_rec &hit_inf, int index) const;

	void init_meshes(long long* meshes_pointers, unsigned int* meshes_number_of_verts, unsigned int* meshes_number_of_tris, 
		int* meshes_material_idx);

	void init_lights(Python_Light* python_lights);

	void init_materials(Python_Material* python_materials);

	void trace_photon(const ray &r, int depth);

	bool trace_ray(const ray &r, hit_rec &rec, int depth);

public:
	int number_of_photons;
	int number_of_closest_photons;
	float search_radius;

	unsigned int number_of_meshes;
	unsigned int number_of_lights;
	unsigned int number_of_materials;

	Mesh *meshes;
	Camera camera;
	Lights_list lights;
	Photon_map photon_map;
	Material** materials;

	BVH_world BVH;
	aabb bounding_box;
};

std::ostream &operator<<(std::ostream &stream, const Scene &scene);

#endif // !SCENE_H
