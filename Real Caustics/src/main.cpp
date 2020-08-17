#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <future>
#include <cmath>
#include "vec3.h"
#include "timer.h"
#include "mesh.h"
#include "Triangle.h"
#include "ray.h"
#include "utils.h"
#include "Color.h"
#include "parser.h"
#include "photon.h"
#include "Lights.h"
#include "matrix.h"
#include "photon_map.h"
#include "priority_queue.h"
#include "BVH\BVH_mesh.h"
#include "Camera.h"
#include "DLL.h"
#include "Blender_definitions.h"
#include "Scene.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

#define REPORT_PROGRESS




extern const float inf = std::numeric_limits<float>::infinity();
extern const float negative_inf = -inf;
extern const float PI = 3.14159265359f;
extern const float E = 2.71828182846;
extern const float PI2 = 6.28318530718f;


//-------------------------------------------
//----------------SCENE----------------------
extern Scene *scene = nullptr;
//----------------SCENE----------------------
//-------------------------------------------


// :number_of_photons - number of emitted photons
// :n_closest - maximum number of photons used for gathering
// :radius - maximum gather radius
// :mesh_pointers - array of pointers to blender meshes
// :number_of_meshes
// :meshes_number_of_verts - array of number of vertices
// :meshes_number_of_tris - array of number of triangles 
// :camera_x - camera width resolution
// :camera_y - camera height resolution  
// :camera_position
// :camera_corner 0,1,2,3 - top right, boottom right, bottom left, top left corners of camera
//  4----1
//	|    |  - Camera corners
//  |    |
//  3----2
// :lights - Lights from Python, need to be converted
// :number_of_lights
// :materials - Materials from Python, need to be converted 
// :number_of_materials
// :meshes_material_idx - Array with length of number_of_meshes, indices of materials
DLLEXPORT void init(int number_of_photons, int n_closest, float radius,
					long long *meshes_pointers,
					unsigned int number_of_meshes,
					unsigned int *meshes_number_of_verts,
					unsigned int *meshes_number_of_tris,
					unsigned int camera_x,
					unsigned int camera_y,
					vec3 camera_position,
					vec3 camera_corner0,
					vec3 camera_corner1,
					vec3 camera_corner2,
					vec3 camera_corner3,
					Python_Light *lights,
					unsigned int number_of_lights,
					Python_Material *materials,
					unsigned int number_of_materials,
					int* meshes_material_idx)
{

	std::cout << number_of_photons << std::endl;
	std::cout << n_closest << std::endl;
	std::cout << radius << std::endl;
	std::cout << meshes_pointers << std::endl;
	std::cout << number_of_meshes << std::endl;
	std::cout << meshes_number_of_verts << std::endl;
	std::cout << meshes_number_of_tris << std::endl;
	std::cout << camera_x << std::endl;
	std::cout << camera_y << std::endl;
	std::cout << lights << std::endl;
	std::cout << number_of_lights << std::endl;
	std::cout << materials << std::endl;
	std::cout << meshes_material_idx << std::endl;


	scene = new Scene;
	//initialize common photon mapping settings
	scene->number_of_photons = number_of_photons;

	std::cout << "passed" << std::endl;

	scene->number_of_closest_photons = n_closest;

	std::cout << "passed" << std::endl;

	scene->search_radius = radius;

	std::cout << "passed" << std::endl;
	scene->number_of_meshes = number_of_meshes;

	std::cout << "passed" << std::endl;
	//initialize meshes with blender data
	scene->init_meshes(meshes_pointers, meshes_number_of_verts, meshes_number_of_tris, meshes_material_idx);

	std::cout << "passed" << std::endl;

	//initialize camera
	scene->camera = Camera(camera_x, camera_y,
						   camera_position,
						   camera_corner0,
						   camera_corner1,
						   camera_corner2,
						   camera_corner3);

	std::cout << "passed" << std::endl;

	//initialize lights and calculate weights for photon emission
	scene->number_of_lights = number_of_lights;

	std::cout << "passed" << std::endl;

	scene->init_lights(lights);
	
	std::cout << "passed" << std::endl;
	
	scene->lights.calculate_weights();
	//initialize photon map (reserve photons)
	
	std::cout << "passed" << std::endl;
	
	scene->photon_map.init_photon_map(number_of_photons);
	//initialize materials
	
	std::cout << "passed" << std::endl;
	
	scene->number_of_materials = number_of_materials;
	
	std::cout << "passed" << std::endl;
	
	scene->init_materials(materials);

	std::cout << "passed" << std::endl;

	std::cout << *scene;
}


DLLEXPORT int main()
{	
	Timer Summary;

#ifdef REPORT_PROGRESS
	std::cout << "Building BVH" << std::endl;
	Timer BVH_timer;
#endif

	scene->update_bvh(1, "test_scene_bvh");

#ifdef REPORT_PROGRESS
	std::cout << "BVH Built  :  " << BVH_timer.elapsed() << std::endl;
#endif

	Timer rendering;

	std::cout << "Tracing started";

	for (int i = 0; i < scene->number_of_photons; ++i)
	{
		ray r = scene->lights.emit_photon();
		
		scene->trace_photon(r, 5);
		std::cout << i << "\r" << std::flush;
		
	}
	
	std::cout << "Tracing finished";
	std::ofstream ou;
	ou.open("D:\\test.obj");

	std::ofstream o;
	o.open("D:\\photon_locs.obj");
	// for (int i = 0; i < map.photons.size(); i++)
	// {
	// 	o << "v " << map.photons[i]->position.x << " " << map.photons[i]->position.y << " " << map.photons[i]->position.z << "\n";
	// }

	std::cout << "\n";
	// for (int j = camera.pixel_height - 1; j >= 0; --j)
	// {
	// 	int progress = static_cast<int>(float((float(camera.pixel_height) - 1 - j)) / (float(camera.pixel_height) - 1) * 100);
	// 	std::cout << progress << "%" << "\r" << std::flush;
	// 	for (int i = 0; i < camera.pixel_width; ++i)
	// 	{

	// 		ray r = camera.get_ray(j, i);
	// 		hit_rec rec;
	// 		trace_ray(r, world, rec, 3);
			
	// 		ou << "v " << rec.p.x << " " << rec.p.y << " " << rec.p.z << "\n";
	// 	}	
	// }
	// for (int i = 0; i < map.photons.size(); i++)
	// {
	// 	map.photons[i]->power /= (float)number_of_photons;
	// }

#ifdef REPORT_PROGRESS
	std::cout << "\n";
	std::cout << "Done  :  " << rendering.elapsed() << std::endl;
#endif

	Timer balacing;
	std::cout << "Balacing Started" << std::endl;

	scene->photon_map.update_kdtree(1, "kdtree.kd");

	std::cout << "Done " << balacing.elapsed() << std::endl;

	Timer writing;
	std::ofstream out;
	out.open("D:\\hello.ppm");
	out << "P3"
		<< "\n"
		<< scene->camera.pixel_width << " " << scene->camera.pixel_height << "\n"
		<< 255 << "\n";
	
	// float radius = 1.4f * std::sqrtf(n_closest * ligths.lights[0]->get_power() / number_of_photons) * 2;
	
	for (int j = scene->camera.pixel_height - 1; j >= 0; --j)
	{
		int progress = static_cast<int>(float((float(scene->camera.pixel_height) - 1 - j)) / (float(scene->camera.pixel_height) - 1) * 100);
	
#ifdef REPORT_PROGRESS
		std::cout << "Progress writing to image: ";
		std::cout << progress << "%"
				  << "\r" << std::flush;
#endif

		for (int i = 0; i < scene->camera.pixel_width; ++i)
		{
			ray r = scene->camera.get_ray(j, i);
			hit_rec rec;
			color pixel_color;
			if (scene->trace_ray(r, rec, 5))
			{
				pixel_color = scene->photon_map.gather_photons(rec.p, scene->search_radius, scene->number_of_closest_photons);
			}
			else
			{
				pixel_color = color(0, 0, 0);
			}
			clamp_color(pixel_color);
			out << pixel_color.r << " " << pixel_color.g << " " << pixel_color.b << "\n";
		}
	}

	out.close();

#ifdef REPORT_PROGRESS
	std::cout << "\n";
	std::cout << "Done  :  " << writing.elapsed() << std::endl;

	std::cout << "\n";
	std::cout << "Summary Time  :  " << Summary.elapsed();
#endif
	return 0;
}
