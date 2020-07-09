#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <future>
#include "vec3.h"
#include "timer.h"
#include "mesh.h"
#include "ray.h"
#include "hittable_list.h"
#include "utils.h"
#include "Color.h"
#include "parser.h"
#include "bvh.h"
#include "photon.h"
#include "Lights.h"
#include "matrix.h"
#include "photon_map.h"
#include "priority_queue.h"
#include "trace_photon.h"
#include "2D\2D_triangle.h"
#include "2D\2D_BVH.h"
#include "2D\UV_Map.h"


#define REPORT_PROGRESS

//#define UNIFORM_GRID
#define HALTON_SEQUENCE

#define MULTI_THREADED

extern const float inf = std::numeric_limits<float>::infinity();
extern const float negative_inf = -inf;
extern const float PI = 3.14159265359f;
extern const float PI2 = 6.28318530718f;
extern int number_of_photons = 1000;

extern const int image_width = 2000;
extern const int image_height = 2000;





int main()
{
	Timer Summary;
	hittable_list world;
	Mesh ocean;
	Mesh plane;
	Lights_list ligths;
	Photon_map map(number_of_photons);
	ligths.add(std::make_shared<Area_Light>(vec3(0., 0, 4.), 2., 2., 0, 500));
	//ligths.add(std::make_shared<Area_Light>(vec3(0., 2., 4.), 2., 2., 0, 500));
	ligths.calculate_weights();

	UV_Map uv;
	uv.triangles.push_back(std::make_shared<triangle2>(vec2(0.42, 0.39), vec2(0.39, 0.0), vec2(0.0, 0.0)));
	uv.triangles.push_back(std::make_shared<triangle2>(vec2(0.06, 0.69), vec2(-0.57, 0.12), vec2(-0.27, 0.0)));
	uv.triangles.push_back(std::make_shared<triangle2>(vec2(0.84, -0.42), vec2(1.17, 0.0), vec2(0.72, 0.0)));
	uv.triangles.push_back(std::make_shared<triangle2>(vec2(0.33, -0.72), vec2(0.0, -0.42), vec2(-0.24, -0.93)));
	uv.triangles.push_back(std::make_shared<triangle2>(vec2(0.78, 0.39), vec2(1.02, 1.05), vec2(1.29, 0.45)));
	uv.build_bvh();
	hit_rec_2 rec;
	vec2 point(0.f, 0.f);
	uv.get_u_v(point, rec);





	#ifdef REPORT_PROGRESS
	Timer parser;
	std::cout << "Parsing Started" << std::endl;
	#endif
	parse("floor.obj", plane, std::make_shared<Catcher>());
	parse("poool.obj", ocean, std::make_shared<Glass>(1.4, colorf(0.5, 1, 1)));
	#ifdef REPORT_PROGRESS
	std::cout << "Done  :  " << parser.elapsed() << std::endl;
	#endif
	std::vector<aabb> work1;
	std::vector<aabb> work2;
	make_list_for_bvh(plane, work1);
	make_list_for_bvh(ocean, work2);
	
	#ifdef REPORT_PROGRESS
	std::cout << "Building BVH" << std::endl;
	Timer BVH_timer;
	#endif
	world.add(make_shared<BVH_node>(work1));
	world.add(make_shared<BVH_node>(work2));
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
	std::cout << "balalcning " << balacing.elapsed();
	map.build_kd_tree();
	Timer writing;
	std::ofstream out;																		//создать и открыть файл
	out.open("D:\\hello.ppm");
	out << "P3" << "\n" << image_width << " " << image_height << "\n" << 255 << "\n";

	for (int j = image_height - 1; j >= 0; --j)
	{
		int progress = static_cast<int>(float((float(image_height) - 1 - j)) / (float(image_height) - 1) * 100);
		#ifdef REPORT_PROGRESS
		std::cout << "Progress writing to image: ";
		std::cout << progress << "%" << "\r" << std::flush;
		#endif

		for (int i = 0; i < image_width; ++i)
		{

			//maprange(colors[j][i].r, 0.0, max, 0.0, 255.0)
			//out << colors[j][i].r << " " << colors[j][i].g << " " << colors[j][i].b << "\n";
			//color clamped = clamp(colors[j][i]);
			//out << clamped.r << " " << clamped.g << " " << clamped.b << "\n";
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
