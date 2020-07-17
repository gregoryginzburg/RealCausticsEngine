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


#define REPORT_PROGRESS

//#define UNIFORM_GRID
#define HALTON_SEQUENCE

#define MULTI_THREADED

extern const float inf = std::numeric_limits<float>::infinity();
extern const float negative_inf = -inf;
extern const float PI = 3.14159265359f;
extern const float E = 2.71828182846;
extern const float PI2 = 6.28318530718f;
extern int number_of_photons = 1200000;

extern const int image_width = 1000;
extern const int image_height = 1000;





int main()
{
	Timer Summary;
	
	hittable_list world;
	std::shared_ptr<Mesh> ocean = std::make_shared<Mesh>();
	std::shared_ptr<Mesh> plane = std::make_shared<Mesh>();
	BVH_mesh plane_bvh;
	BVH_mesh ocean_bvh;
	Camera camera(1280, 720, vec3(), vec3(), vec3(), vec3(), vec3());
	Lights_list ligths;
	Photon_map map(number_of_photons);
	UV_Map uv;
	ligths.add(std::make_shared<Area_Light>(vec3(0., 0, 4.), 2., 2., 0, 100));
	ligths.calculate_weights();
	
	#ifdef REPORT_PROGRESS
	Timer parser;
	std::cout << "Parsing Started" << std::endl;
	#endif
	parse("floor.obj", plane, std::make_shared<Catcher>());
	parse("poool.obj", ocean, std::make_shared<Glass>(1.45, colorf(1, 1, 1)));


	#ifdef REPORT_PROGRESS
	std::cout << "Done  :  " << parser.elapsed() << std::endl;
	#endif


	
	
	#ifdef REPORT_PROGRESS
	std::cout << "Building BVH" << std::endl;
	Timer BVH_timer;
	#endif
	update_bvh(0, ocean_bvh, ocean, "ocean_bvh.bvh");
	update_bvh(0, plane_bvh, plane, "plane_bvh.bvh");
	world.add(ocean_bvh, ocean);
	world.add(plane_bvh, plane);
	uv.mesh = plane;
	uv.make_uv_map();
	uv.build_bvh();
	#ifdef REPORT_PROGRESS
	std::cout << "BVH Built  :  " << BVH_timer.elapsed() << std::endl;
	#endif
	
	Timer rendering;
	
	std::cout << "Tracing started";
	for (int i = 0; i < number_of_photons; ++i)
	{
		ray r = ligths.emit_photon();
		trace_photon(map, world, r, 2);
	}	
	

	#ifdef REPORT_PROGRESS
	std::cout << "\n";
	std::cout << "Done  :  " << rendering.elapsed() << std::endl;
	#endif

	Timer balacing;
	std::cout << "Balacing Started" << std::endl;
	
	map.update_kdtree(1, "kdtree.kd");

	std::cout << "Done " << balacing.elapsed() << std::endl;
	Timer writing;
	std::ofstream out;																		//создать и открыть файл
	out.open("D:\\hello.ppm");
	out << "P3" << "\n" << image_width << " " << image_height << "\n" << 255 << "\n";
	int n_closest = 300;
	float radius = 1.4f * std::sqrtf(n_closest * ligths.lights[0]->get_power() / number_of_photons);
	for (int j = camera.pixel_height - 1; j >= 0; --j)
	{
		int progress = static_cast<int>(float((float(image_height) - 1 - j)) / (float(image_height) - 1) * 100);
		#ifdef REPORT_PROGRESS
		std::cout << "Progress writing to image: ";
		std::cout << progress << "%" << "\r" << std::flush;
		#endif

		for (int i = 0; i < camera.pixel_width; ++i)
		{
			ray r = camera.get_ray(j, i);
			hit_rec rec;
			trace_ray(r, world, rec, 2);
			color pixel_color = gather_photons(rec.p, map, radius, n_closest);
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
