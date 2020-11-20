#ifndef SCENE_H
#define SCENE_H


#include "BVH/BVH_world.h"
#include "Camera.h"
#include "materials.h"
#include "Light_list.h"
#include "Vertex_Merging.h"
#include <vector>
#include <memory>

class Mesh;
class Python_Light;
class Python_Material;
class Matrix4x4;
class Object_Materials;
class Scene
{
public:
	Scene() {}
public:
	BVHNode_world *make_default_bvh();

	void update_bvh(bool was_changed, const char *file_path);

	bool hit(const ray &r, float tmin, float tmax, Isect &hit_inf, int index) const;

	void init_meshes(long long* meshes_pointers, unsigned int* meshes_number_of_verts, unsigned int* meshes_number_of_tris,
		PythonMatrix4x4* mesh_matrices, int** materials_indices);

	void init_lights(Python_Light* python_lights);

	void init_materials(Python_Material* python_materials);


public:
	Vertex_Merging integrator;
	int samples;

	unsigned int number_of_meshes;
	unsigned int number_of_lights;
	unsigned int number_of_materials;

	Mesh *meshes;
	Camera camera;
	Lights_list lights;
	KD_Tree photon_map;
	Material** materials;

	BVH_world BVH;
	aabb bounding_box;

	int debug_test;
};

std::ostream &operator<<(std::ostream &stream, const Scene &scene);

#endif // !SCENE_H
