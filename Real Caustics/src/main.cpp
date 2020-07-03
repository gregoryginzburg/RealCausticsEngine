#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>
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
#include <mutex>
#include <future>

#define REPORT_PROGRESS

//#define UNIFORM_GRID
#define HALTON_SEQUENCE

#define MULTI_THREADED

extern const float inf = std::numeric_limits<float>::infinity();
extern const float PI = 3.14159265359f;

extern const int image_width = 2000;
extern const int image_height = 2000;


void trace_photon(std::vector<photon>& photons, hittable_list& world, ray& r, int depth);


int main()
{
	
	Timer Summary;
	hittable_list world;
	Mesh ocean;
	Mesh plane;
	Area_Light light(vec3(0., 0., 4.), 2., 2., 0);

	#ifdef REPORT_PROGRESS
	Timer parser;
	std::cout << "Parsing Started" << std::endl;
	#endif
	parse("floor.obj", plane, std::make_shared<Catcher>());
	parse("poool.obj", ocean, std::make_shared<Glass>(1.4));
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
	
	int height = 1000;
	int width = 1000;
	
	

	
	for (int j = height - 1; j >= 0; --j)
	{
		int progress = static_cast<int>(float((float(height) - 1 - j)) / (float(height) - 1) * 100);
		#ifdef REPORT_PROGRESS
		std::cout << "Progress rendering: ";
		std::cout << progress << "%" << "\r" << std::flush;
		#endif
		for (int i = 0; i < width; ++i)
		{
			ray r = light.get_ray(j, i);
		}
	}
	

	
	
	#ifdef REPORT_PROGRESS
	std::cout << "\n";
	std::cout << "Done  :  " << rendering.elapsed() << std::endl;
	#endif

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
