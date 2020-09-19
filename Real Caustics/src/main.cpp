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
#include "hdri.h"

#define TINYEXR_IMPLEMENTATION
#include "../lib/tinyexr.h"


//#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

#define REPORT_PROGRESS
extern std::ofstream test_out = std::ofstream();




extern const float inf = std::numeric_limits<float>::infinity();
extern const float negative_inf = -inf;
extern const float PI = 3.14159265359f;
extern const float E = 2.71828182846;
extern const float PI2 = 6.28318530718f;

extern int radius_estimate_right = 0;

//-------------------------------------------
//----------------SCENE----------------------
//extern Scene *scene = nullptr;
extern Scene* scene = nullptr;
Python_Light* python_lights = nullptr;

//----------------SCENE----------------------
//-------------------------------------------


// :number_of_photons - number of emitted photons
// :n_closest - maximum number of photons used for gathering
// :radius - maximum gather radius
// :mesh_pointers - array of pointers to blender meshes
// :number_of_meshes
// :mesh_matrices - matrix_world matrices 
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
// :materials_indices - triangles have material indices but they are tied to materials linked to mesh - array of material indices (length = number of meshes)
// :meshes_material_idx - Array with length of number_of_meshes, indices of materials
DLLEXPORT void init(int number_of_photons, int n_closest, float radius,
					long long *meshes_pointers,
					unsigned int number_of_meshes,
					matrix_4x4* mesh_matrices,
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
					int** materials_indices,
					const char* hdri_path)
{
	/*
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
	*/
	test_out.open("D:\\directions.obj");

	scene = new Scene;
	//initialize common photon mapping settings
	scene->number_of_photons = number_of_photons;
	scene->number_of_closest_photons = n_closest;
	scene->search_radius = radius;

	//initialize meshes with blender data
	scene->number_of_meshes = number_of_meshes;

	scene->init_meshes(meshes_pointers, meshes_number_of_verts, meshes_number_of_tris, mesh_matrices, materials_indices);

	//initialize camera
	scene->camera = Camera(camera_x, camera_y,
						   camera_position,
						   camera_corner0,
						   camera_corner1,
						   camera_corner2,
						   camera_corner3);

	//initialize lights and calculate weights for photon emission
	scene->number_of_lights = number_of_lights;
	python_lights = lights;

	//initialize photon map (reserve photons)
	scene->photon_map.init_photon_map(number_of_photons);
	//initialize materials
	scene->number_of_materials = number_of_materials;

	scene->init_materials(materials);

	scene->hdri_path = hdri_path;
	std::cout << *scene;
}
using std::cos;
using std::sin;
inline void convert_to_spherical(vec3& point, float u, float v)
{
	point.x = cos(PI * (0.5 - v)) * cos(PI2 * (u - 0.5));
	point.y = -cos(PI * (0.5 - v)) * sin(PI2 * (u - 0.5));
	point.z = -sin(PI * (0.5 - v));
}

DLLEXPORT int main()
{
	

	int depth = 5;
	radius_estimate_right = 0;
	
	Timer Summary;


#ifdef REPORT_PROGRESS
	std::cout << "Building BVH" << std::endl;
	Timer BVH_timer;
#endif

	scene->update_bvh(1, "test_scene_bvh");

#ifdef REPORT_PROGRESS
	std::cout << "BVH Built  :  " << BVH_timer.elapsed() << std::endl;
#endif
	// 
	// scene bounding bbox is availibale adter building bvh
	scene->init_lights(python_lights);
	scene->lights.calculate_weights();
	
	std::cout << scene->lights.lights[0]->get_power();

	Timer rendering;

	std::cout << "Tracing started";


	//std::ofstream origins;
	//origins.open("D:\\origins.obj");
	/*
	helper_light_emit helper;
	for (int i = 0; i < scene->number_of_photons; ++i)
	{
		ray r = scene->lights.emit_photon(helper);
		//vec3 point = r.origin + r.direction;
		//ou << "v " << point.x << " " << point.y << " " << point.z << "\n";
		//origins << "v " << r.origin.x << " " << r.origin.y << " " << r.origin.z << "\n";
		if (i % 100000 == 0)
		{
			std::cout << i << std::endl;
		}
		bool was_refracted = false;
		scene->trace_photon(r, depth, was_refracted);
	}*/
	scene->trace_photons_from_lights();
	//trace_photons_from_hdri(scene);
	

	

#ifdef REPORT_PROGRESS
	std::cout << "Tracing finished";
	std::cout << "\n";
	std::cout << "Done  :  " << rendering.elapsed() << std::endl;
#endif

	if (scene->photon_map.photons.size() == 0)
	{
		std::cout << "!!!!!!!!!!Warning!!!!!!    - No photons in the photon map" << std::endl;
		return 0;
	}
	std::ofstream ou;
	ou.open("D:\\test.obj");

	std::ofstream o;
	o.open("D:\\photon_locs.obj");
	
	for (int i = 0; i < scene->photon_map.photons.size(); i++)
	{
	 	o << "v " << scene->photon_map.photons[i]->position.x << " " << scene->photon_map.photons[i]->position.y << " " << scene->photon_map.photons[i]->position.z << "\n";

		//test_out << "v " << scene->photon_map.photons[i]->direction.x << " " << scene->photon_map.photons[i]->direction.y << " " << scene->photon_map.photons[i]->direction.z << "\n";
	}

	std::cout << "\n";
	
	
	/*for (int j = scene->camera.pixel_height - 1; j >= 0; --j)
	{
		for (int i = 0; i < scene->camera.pixel_width; ++i)
		{

			ray r = scene->camera.get_ray(i, j);
			hit_rec rec;
			scene->trace_ray(r, rec, depth);
			
			ou << "v " << rec.p.x << " " << rec.p.y << " " << rec.p.z << "\n";
		}	
	}*/


	

	Timer balacing;
	std::cout << "Balacing Started" << std::endl;

	scene->photon_map.update_kdtree(1, "kdtree.kd");

	std::cout << "Done " << balacing.elapsed() << std::endl;
	/*
	
	int number_closest = 1000;
	Priority_queue closest_photons(number_closest);
	float search_distance_squared = 0.5f;
	scene->photon_map.find_closest_photons(vec3(-1, 0, -2.15805), search_distance_squared, closest_photons, 1);
	for (int i = 1; i < closest_photons.number_of_photons + 1; ++i)
	{
		ou << "v " << closest_photons.photons[i]->position.x << " " << closest_photons.photons[i]->position.y << " " << closest_photons.photons[i]->position.z << "\n";
	}*/

	

	
	Timer writing;

	#ifdef REPORT_PROGRESS
	
	#endif
	int number_of_pixels_render = scene->camera.pixel_height * scene->camera.pixel_width;
	float* image = new float[number_of_pixels_render * 3];

	float search_radius = scene->search_radius * scene->search_radius;
	// float search_radius = 1.4f * std::sqrtf(n_closest * ligths.lights[0]->get_power() / number_of_photons) * 2;
	float number_of_closest_photons = scene->number_of_closest_photons;
	
	int i = 0;
	int number_of_channels = number_of_pixels_render * 3;
	int x = scene->camera.pixel_width;
	int y = scene->camera.pixel_height;
	int ii_number_of_pixels = 0;
	while (i < number_of_channels)
	{
		#ifdef REPORT_PROGRESS
		if (i % (scene->camera.pixel_height * 3) == 0)
		{
			std::cout << "Progress writing to image: ";
			std::cout << (i / (float)number_of_channels) * 100.0f << "%"  << std::flush << "\r";
		}	
		#endif
		//ii% x, ii / y
		ray r = scene->camera.get_ray(ii_number_of_pixels % x, (y - (ii_number_of_pixels / x) - 1));
		hit_rec rec;
		float pixel_color[3]{ 0.0f };
		scene->materials[rec.material_idx];
		if (scene->trace_ray(r, rec, depth))
		{
			scene->photon_map.gather_photons(rec, 
				dynamic_cast<Catcher*>(scene->materials[rec.material_idx]), 
				search_radius, number_of_closest_photons, pixel_color);
		}
		image[i] = pixel_color[0];
		image[i + 1] = pixel_color[1];
		image[i + 2] = pixel_color[2];
		i += 3;
		++ii_number_of_pixels;
	}
	const char* err = nullptr;
	SaveEXR(image, scene->camera.pixel_width, scene->camera.pixel_height, 3, 0, "D:\\caustics.exr", &err);

	

#ifdef REPORT_PROGRESS
	std::cout << "\n";
	std::cout << "Done  :  " << writing.elapsed() << std::endl;

	std::cout << "\n";
	std::cout << "Summary Time  :  " << Summary.elapsed() << std::endl;
	int all_pixels = scene->camera.pixel_width * scene->camera.pixel_height;
	std::cout << "Radius Estimate Was Wrong in  " << ((float)all_pixels - (float)radius_estimate_right) / float(all_pixels) * 100.0f << "%" << std::endl;
	std::cout << "was wrong times " << all_pixels - radius_estimate_right << "    out of " << all_pixels;
#endif
	test_out.close();
	delete scene;
	scene = nullptr;
	return 0;
}
