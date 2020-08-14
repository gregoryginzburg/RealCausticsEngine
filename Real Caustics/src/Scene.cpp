#include "Scene.h"

#include <fstream>
#include "mesh.h"
#include "Blender_definitions.h"
#include "BVH/BVH_world.h"



void Scene::init_meshes(long long* meshes_pointers, unsigned int* meshes_number_of_verts, unsigned int* meshes_number_of_tris)
{

    meshes = new Mesh[number_of_meshes];
    for (int i = 0; i < number_of_meshes; ++i)
    {
        Mesh_blender *mesh_blender_data = (Mesh_blender *)meshes_pointers[i];
        meshes[i] = Mesh(mesh_blender_data, meshes_number_of_verts[i], meshes_number_of_tris[i]);
    }
}

void Scene::update_bvh(bool was_changed, const char *file_path)
{
	if (was_changed)
	{
		for (int i = 0; i < number_of_meshes; ++i)
		{
			meshes[i].update_bvh(was_changed, file_path);
		}
		BVHNode_world *root = make_default_bvh();
		create_cache_friendly_bvh(root, BVH, file_path);
	}
	else
	{
		for (int i = 0; i < number_of_meshes; ++i)
		{
			meshes[i].update_bvh(was_changed, file_path);
		}
		std::ifstream file(file_path, std::ios::binary);
		int size1;
		int size2;
		file.read((char *)&size1, sizeof(int));
		BVH.mesh_indices.resize(size1);
		file.read((char *)&BVH.mesh_indices[0], sizeof(int) * (size_t)size1);
		file.read((char *)&size2, sizeof(int));
		BVH.bvh_nodes.resize(size2);
		file.read((char *)&BVH.bvh_nodes[0], sizeof(CacheBVHNode_world) * size2);
	}
}
//index = 0
bool Scene::hit(const ray &r, float tmin, float tmax, hit_rec &hit_inf, int index) const
{
	//1 - leaf node
	if (BVH.bvh_nodes[index].u.leaf.count & 0x80000000)
	{

		if (BVH.bvh_nodes[index].bounding_box.hit(r, tmin, tmax))
		{
			return BVH.bvh_nodes[index].hit(r, tmin, tmax, hit_inf, BVH, meshes);
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (BVH.bvh_nodes[index].bounding_box.hit(r, tmin, tmax))
		{
			bool left_hit = hit(r, tmin, tmax, hit_inf, BVH.bvh_nodes[index].u.inner.idxLeft);
			bool right_hit = hit(r, tmin, tmax, hit_inf, BVH.bvh_nodes[index].u.inner.idxRight);

			return left_hit | right_hit;
		}
		else
		{
			return false;
		}
	}
}