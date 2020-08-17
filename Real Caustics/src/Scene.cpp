#include "Scene.h"

#include <fstream>
#include <memory>
#include "mesh.h"
#include "Blender_definitions.h"
#include "BVH/BVH_world.h"
#include "materials.h"
#include "Lights.h"



void Scene::init_meshes(long long* meshes_pointers, unsigned int* meshes_number_of_verts, unsigned int* meshes_number_of_tris, 
	int* meshes_material_idx)
{

    meshes = new Mesh[number_of_meshes];
    for (int i = 0; i < number_of_meshes; ++i)
    {
        Mesh_blender *mesh_blender_data = (Mesh_blender *)meshes_pointers[i];
		
		meshes[i] = Mesh(mesh_blender_data, meshes_number_of_verts[i], meshes_number_of_tris[i], meshes_material_idx[i]);
    }
}

void Scene::init_materials(Python_Material* python_materials)
{
	materials = new Material *[number_of_materials];

	for (size_t i = 0; i < number_of_materials; ++i)
	{
		if (python_materials[i].type == 'C')
			materials[i] = new Catcher();
		else if (python_materials[i].type == 'G')
			materials[i] = new Glass(python_materials[i].ior, python_materials[i].color);
		else
			materials[i] = new Metal(python_materials[i].color);
	}
}

void Scene::init_lights(Python_Light* python_lights)
{
	for (size_t i = 0; i < number_of_lights; ++i)
	{
		std::cout << python_lights[i].type << std::endl;
		if (python_lights[i].type == 'P')
		{
			lights.add(std::make_shared<Point_Light>());
		}
			
		else if (python_lights[i].type == 'S')
		{
			std::cout << "SUN To ADD" << std::endl;
			auto pos = python_lights[i].position;
			std::cout << "LIGHT PASSED" << std::endl;
			auto rot = python_lights[i].rotation;
			std::cout << "LIGHT PASSED" << std::endl;
			auto pow = python_lights[i].power;
			std::cout << "LIGHT PASSED" << std::endl;
			/*lights.add(std::make_shared<Sun_Light>(python_lights[i].position,
				python_lights[i].rotation,
				0,
				python_lights[i].power));*/
			auto eaweaw = std::make_shared<Sun_Light>(pos, rot, 0, pow);
			std::cout << "LIGHT PASSED" << std::endl;

			lights.add(eaweaw);
		}	
		else
		{
			lights.add(std::make_shared<Area_Light>(python_lights[i].position,
				python_lights[i].rotation,
				python_lights[i].width,
				python_lights[i].height,
				python_lights[i].power));
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

void Scene::trace_photon(const ray &r, int depth)
{
    static bool was_refracted = false;
    if (depth == 0)
	{
		return;
	}
	hit_rec rec;
	ray scattered_ray;

	if (hit(r, 0.0001f, inf, rec, 0))
	{
		if (materials[rec.material_idx]->scatter(r, rec, scattered_ray))
		{
			scattered_ray.power = r.power;
			scattered_ray *= materials[rec.material_idx]->get_color();
			was_refracted = true;
			return trace_photon(scattered_ray, depth - 1);
		}
		else
		{
			if (was_refracted)
			{
				photon_map.add(rec.p, r.power);
			}
				
		}		
	}   
}

bool Scene::trace_ray(const ray &r, hit_rec &rec, int depth)
{
	if (depth == 0)
	{
		return false;
	}
	ray scattered_ray;
	if (hit(r, 0.0001f, inf, rec, 1))
	{
		if (materials[rec.material_idx]->scatter(r, rec, scattered_ray))
		{
			return trace_ray(scattered_ray, rec, depth - 1);
		}
		else
		{
			return true;
		}
	}
	return false;    
}


std::ostream &operator<<(std::ostream &stream, const Scene &scene)
{
	stream << "                   SCENE                   " << std::endl;
	stream << "Numbers of photons:            " << scene.number_of_photons << std::endl;
	stream << "Numbers of closest photons:    " << scene.number_of_closest_photons << std::endl;
	stream << "Search radius                  " << scene.search_radius << std::endl;
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
	if (scene.photon_map.photons.size() == 0)
	{
		stream <<"Not Initialized                " << std::endl; 
	}	
	else
	{
		stream << "Initialized                   " << std::endl;	
	}
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
