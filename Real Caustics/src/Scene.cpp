#include "Scene.h"

#include <fstream>
#include <memory>
#include "mesh.h"
#include "Blender_definitions.h"
#include "BVH/BVH_world.h"
#include "materials.h"
#include "Lights.h"
#include "matrix.h"



void Scene::init_meshes(long long* meshes_pointers, unsigned int* meshes_number_of_verts, unsigned int* meshes_number_of_tris,
	PythonMatrix4x4* mesh_matrices, int** materials_indices)
{

    meshes = new Mesh[number_of_meshes];
    for (int i = 0; i < number_of_meshes; ++i)
    {
        Mesh_blender *mesh_blender_data = (Mesh_blender *)meshes_pointers[i];
		
		Matrix4x4 mat;
		mat.m[0][0] = mesh_matrices[i].t00;
		mat.m[0][1] = mesh_matrices[i].t01;
		mat.m[0][2] = mesh_matrices[i].t02;
		mat.m[0][3] = mesh_matrices[i].t03;
		mat.m[1][0] = mesh_matrices[i].t10;
		mat.m[1][1] = mesh_matrices[i].t11;
		mat.m[1][2] = mesh_matrices[i].t12;
		mat.m[1][3] = mesh_matrices[i].t13;
		mat.m[2][0] = mesh_matrices[i].t20;
		mat.m[2][1] = mesh_matrices[i].t21;
		mat.m[2][2] = mesh_matrices[i].t22;
		mat.m[2][3] = mesh_matrices[i].t23;
		mat.m[3][0] = mesh_matrices[i].t30;
		mat.m[3][1] = mesh_matrices[i].t31;
		mat.m[3][2] = mesh_matrices[i].t32;
		mat.m[3][3] = mesh_matrices[i].t33;

		meshes[i] = Mesh(mesh_blender_data, meshes_number_of_verts[i], meshes_number_of_tris[i], mat, materials_indices[i], true);
    }
}

void Scene::init_materials(Python_Material* python_materials)
{
	materials = new Material*[number_of_materials];

	for (size_t i = 0; i < number_of_materials; ++i)
	{
		vec3 color;
		color.x = python_materials[i].color.r;
		color.y = python_materials[i].color.g;
		color.z = python_materials[i].color.b;
		float ior = python_materials[i].ior;
		float roughness = python_materials[i].roughness;
		float specular = python_materials[i].specular;

		if (python_materials[i].type == 'P')
			materials[i] = new Plastic_Material(color, roughness, specular);
		else if (python_materials[i].type == 'G')
			materials[i] = new Glass_Material(color, ior);
		else if (python_materials[i].type == 'M')
			materials[i] = new Metal_Material(color, roughness);
		else
			materials[i] = new Plastic_Material(vec3(1, 1, 1), 1, 0.0);
	}
}

void Scene::init_lights(Python_Light* python_lights)
{
	for (size_t i = 0; i < number_of_lights; ++i)
	{
		vec3 color(python_lights[i].color.r, python_lights[i].color.g, python_lights[i].color.b);

		if (python_lights[i].type == 'P')
		{
			lights.add(std::make_shared<Point_Light>(python_lights[i].position,
				python_lights[i].radius, 
				python_lights[i].power,
				python_lights[i].color));
		}
			
		else if (python_lights[i].type == 'S')
		{

			lights.add(std::make_shared<Sun_Light>(
				python_lights[i].rotation,
				python_lights[i].angle,
				python_lights[i].power,
				color));

		}	
		else
		{
			lights.add(std::make_shared<Area_Light>(python_lights[i].position,
				python_lights[i].rotation,
				python_lights[i].width,
				python_lights[i].height,
				python_lights[i].power,
				color));
		}
			


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
		aabb scene_bbox;
		BVHInner_world *test_root_cast = dynamic_cast<BVHInner_world *>(root);

		if (test_root_cast == nullptr)
		{
			BVHLeaf_world* casted_root = dynamic_cast<BVHLeaf_world *>(root);
			scene_bbox = casted_root->bbox;
		}		 
		else
		{
			scene_bbox = test_root_cast->bbox;
		}
		bounding_box = scene_bbox;
		create_cache_friendly_bvh(root, BVH, file_path);
	}
	else
	{
		/*
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
		file.read((char *)&BVH.bvh_nodes[0], sizeof(CacheBVHNode_world) * size2);*/
	}
}
//index = 0
bool Scene::hit(const ray &r, float tmin, float tmax, Isect &hit_inf, int index) const
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
			bool right_hit = hit(r, tmin, left_hit ? hit_inf.distance : tmax, hit_inf, BVH.bvh_nodes[index].u.inner.idxRight);
			return left_hit | right_hit;
		}
		else
		{
			return false;
		}
	}
}



std::ostream &operator<<(std::ostream &stream, const Scene &scene)
{
	stream << "                   SCENE                   " << std::endl;

	stream << "\n";

	stream << "    Camera:                    " << std::endl;
	stream << "Resolution x:                  " << scene.camera.pixel_width << std::endl;
	stream << "Resolution y:                  " << scene.camera.pixel_height << std::endl;
	stream << "\n";

	stream << "    Meshes:                    " << std::endl;
	stream << "Numbers of meshes:             " << scene.number_of_meshes << std::endl;
	int number_of_triangles = 0;
	for (int i = 0; i < scene.number_of_meshes; ++i)
	{
		number_of_triangles += scene.meshes[i].number_of_triangles;
	}
	stream << "Total Number of Triangles:     " << number_of_triangles << std::endl;
	stream << "\n";

	stream << "    Lights:                    " << std::endl;
	stream << "Number of lights               " << scene.number_of_lights << std::endl;	
	
	stream << "\n";

	stream << "    Materials:                 " << std::endl;
	stream << "Number of Materials            " << scene.number_of_materials << std::endl;
	stream << "\n";

	stream << "    Photon Map:                " << std::endl;
	

	stream << "\n";

	stream << "    BVH:                       " << std::endl;
	if (scene.BVH.bvh_nodes.size() == 0)
	{
		stream <<"Not Initialized                " << std::endl; 
	}	
	else
	{
		stream << "Initialized                   " << std::endl;	
	}
	stream << "\n";

	return stream;
}
