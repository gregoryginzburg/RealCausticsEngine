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
#include "hittable_list.h"
#include "utils.h"
#include "Color.h"
#include "parser.h"
#include "photon.h"
#include "Lights.h"
#include "matrix.h"
#include "photon_map.h"
#include "priority_queue.h"
#include "trace_photon.h"
#include "2D\UV_Map.h"
#include "Gather_photons.h"
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
Scene *scene;
//----------------SCENE----------------------
//-------------------------------------------


DLLEXPORT void init(int number_of_photons, int n_closest, float radius, long long* meshes_pointers, unsigned int number_of_meshes,
					unsigned int* meshes_number_of_verts, unsigned int* meshes_number_of_tris)
{
	scene = new Scene;

	scene->number_of_photons = number_of_photons;
	scene->number_of_closest_photons = n_closest;
	scene->search_radius = radius;

	std::cout << "number of photons = " << number_of_photons << std::endl;
	std::cout << "number of closest photons = " << n_closest << std::endl;
	std::cout << "number of photons = " << radius << std::endl;

	scene->number_of_meshes = number_of_meshes;
	//initiliazes meshes with blender data
	scene->init_meshes(meshes_pointers, meshes_number_of_verts, meshes_number_of_tris);
	
}


DLLEXPORT int main()
{
	
	
	Timer Summary;

	hittable_list world;

	// std::shared_ptr<Mesh> water = std::make_shared<Mesh>();
	// std::shared_ptr<Mesh> cornel_box = std::make_shared<Mesh>();
	// std::shared_ptr<Mesh> cube_big = std::make_shared<Mesh>();
	// std::shared_ptr<Mesh> cube_small = std::make_shared<Mesh>();

	std::shared_ptr<Mesh> water = std::make_shared<Mesh>();
	std::shared_ptr<Mesh> floor = std::make_shared<Mesh>();

	// BVH_mesh water_bvh;
	// BVH_mesh cornel_box_bvh;
	// BVH_mesh cube_big_bvh;
	// BVH_mesh cube_small_bvh;

	BVH_mesh water_bvh;
	BVH_mesh floor_bvh;

	// Camera camera(837, 912,
	// 			  vec3(-3.95455, 0.0, 1.25949),
	// 			  vec3(-2.5823276042938232, -0.4588816463947296, 1.7594863176345825),
	// 			  vec3(-2.582327365875244, -0.4588816463947296, 0.7594862580299377),
	// 			  vec3(-2.582327365875244, 0.45888152718544006, 0.7594862580299377),
	// 			  vec3(-2.5823276042938232, 0.45888152718544006, 1.7594863176345825));

	Camera camera(915, 1095,
				  vec3(8.00657, -0.909492, 8.63837),
				  vec3(6.7962470054626465, -0.31748080253601074, 7.904736518859863),
				  vec3(7.550374984741211, -0.4234650135040283, 7.256617069244385),
				  vec3(7.434081077575684, -1.2509496212005615, 7.256617069244385),
				  vec3(6.679953575134277, -1.144965410232544, 7.904736518859863));
	Lights_list ligths;

	Photon_map map(number_of_photons);
	// 0.05
	// ligths.add(std::make_shared<Area_Light>(vec3(0, 0, 2.33103), 0.03, 0.03, 0, 2));
	ligths.add(std::make_shared<Area_Light>(vec3(0, 4.91844, 6.53646), vec3(-40, 0, 0), 8.84011, 3.2278, 0, 5));
	ligths.calculate_weights();

#ifdef REPORT_PROGRESS
	Timer parser;
	std::cout << "Parsing Started" << std::endl;
#endif

	// parse("water.obj", water, std::make_shared<Glass>(1.333, colorf(1, 1, 1)));
	// parse("cornel_box.obj", cornel_box, std::make_shared<Catcher>());
	// parse("cube_big.obj", cube_big, std::make_shared<Catcher>());
	// parse("cube_small.obj", cube_small, std::make_shared<Catcher>());

	parse("water.obj", water, std::make_shared<Glass>(1.333, colorf(1, 1, 1)));
	parse("everything.obj", floor, std::make_shared<Catcher>());

#ifdef REPORT_PROGRESS
	std::cout << "Done  :  " << parser.elapsed() << std::endl;
#endif

#ifdef REPORT_PROGRESS
	std::cout << "Building BVH" << std::endl;
	Timer BVH_timer;
#endif
	// update_bvh(0, water_bvh, water, "water_bvh.bvh");
	// update_bvh(0, cornel_box_bvh, cornel_box, "cornel_box_bvh.bvh");
	// update_bvh(0, cube_big_bvh, cube_big, "cube_big_bvh.bvh");
	// update_bvh(0, cube_small_bvh, cube_small, "cube_small_bvh.bvh");

	update_bvh(0, water_bvh, water, "water_bvh.bvh");
	update_bvh(0, floor_bvh, floor, "floor_bvh.bvh");

	// world.add(water_bvh, water);
	// world.add(cornel_box_bvh, cornel_box);
	// world.add(cube_big_bvh, cube_big);
	// world.add(cube_small_bvh, cube_small);

	world.add(water_bvh, water);
	world.add(floor_bvh, floor);
#ifdef REPORT_PROGRESS
	std::cout << "BVH Built  :  " << BVH_timer.elapsed() << std::endl;
#endif

	Timer rendering;

	std::cout << "Tracing started";
	for (int i = 0; i < number_of_photons; ++i)
	{
		ray r = ligths.emit_photon();
		bool was_refracted = false;
		trace_photon(map, world, r, was_refracted, 5);
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

	map.update_kdtree(1, "kdtree.kd");

	std::cout << "Done " << balacing.elapsed() << std::endl;
	Timer writing;
	std::ofstream out; //создать и открыть файл
	out.open("D:\\hello.ppm");
	out << "P3"
		<< "\n"
		<< camera.pixel_width << " " << camera.pixel_height << "\n"
		<< 255 << "\n";
	
	// float radius = 1.4f * std::sqrtf(n_closest * ligths.lights[0]->get_power() / number_of_photons) * 2;
	
	for (int j = camera.pixel_height - 1; j >= 0; --j)
	{
		int progress = static_cast<int>(float((float(camera.pixel_height) - 1 - j)) / (float(camera.pixel_height) - 1) * 100);
	
#ifdef REPORT_PROGRESS
		std::cout << "Progress writing to image: ";
		std::cout << progress << "%"
				  << "\r" << std::flush;
#endif

		for (int i = 0; i < camera.pixel_width; ++i)
		{
			ray r = camera.get_ray(j, i);
			hit_rec rec;
			color pixel_color;
			if (trace_ray(r, world, rec, 5))
			{
				//o << "v " << rec.p.x << " " << rec.p.y << " " << rec.p.z << "\n";
				pixel_color = gather_photons(rec.p, map, radius, n_closest, ou);
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
#endif*/
	return 0;
}
