﻿#include <iostream>
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



#define REPORT_PROGRESS

//#define UNIFORM_GRID
#define HALTON_SEQUENCE

#define MULTI_THREADED

extern const float inf = std::numeric_limits<float>::infinity();
extern const float negative_inf = -inf;
extern const float PI = 3.14159265359f;
extern const float PI2 = 6.28318530718f;
extern int number_of_photons = 1000000;

extern const int image_width = 1000;
extern const int image_height = 1000;





int main()
{
	Timer Summary;
	hittable_list world;
	std::shared_ptr<Mesh> ocean = std::make_shared<Mesh>();
	std::shared_ptr<Mesh> plane = std::make_shared<Mesh>();

	Lights_list ligths;
	Photon_map map(number_of_photons);
	UV_Map uv;
	ligths.add(std::make_shared<Area_Light>(vec3(0., 0, 4.), 2., 2., 0, 500));
	ligths.calculate_weights();

	#ifdef REPORT_PROGRESS
	Timer parser;
	std::cout << "Parsing Started" << std::endl;
	#endif
	parse("floor.obj", plane, std::make_shared<Catcher>());
	parse("poool.obj", ocean, std::make_shared<Glass>(1.4, colorf(1, 1, 1)));
	world.add(ocean);
	world.add(plane);
	uv.mesh = plane;
	uv.make_uv_map();
	#ifdef REPORT_PROGRESS
	std::cout << "Done  :  " << parser.elapsed() << std::endl;
	#endif

	
	#ifdef REPORT_PROGRESS
	std::cout << "Building BVH" << std::endl;
	Timer BVH_timer;
	#endif
	world.create_bvh();
	uv.build_bvh();
	#ifdef REPORT_PROGRESS
	std::cout << "BVH Built  :  " << BVH_timer.elapsed() << std::endl;
	#endif

	Timer rendering;
	std::cout << "Tracing started";
	std::ofstream out;																		//создать и открыть файл
	out.open("D:\\points.obj");
	for (int i = 0; i < number_of_photons; ++i)
	{
		ray r = ligths.emit_photon();
		trace_photon(map, world, r, 2);
	}	/*
	map.build_kd_tree();
	std::ofstream out;																		//создать и открыть файл
	out.open("D:\\points.obj");
	Priority_queue closest_photons(50);
	vec3 point11(0, 0, -1.35973f);
	map.find_closest_photons(point11, 0.5, closest_photons);
	for (int i = 0; i < closest_photons.photons.size(); ++i)
	{
		out << "v " << closest_photons.photons[i]->position.x << " " << closest_photons.photons[i]->position.y << " " << closest_photons.photons[i]->position.z << "\n";
	}*/
	
	/*for (int j = image_height - 1; j >= 0; --j)
	{
		for (int i = 0; i < image_width; ++i)
		{
			vec2 pixel = vec2(i, j);
			float delta_x = 0.5f / image_width;
			float delta_y = 0.5f / image_height;
			vec2 u_v = vec2(pixel.x / image_width + delta_x, pixel.y / image_height + delta_y);
			vec3 point = uv.get_point_from_uv(u_v);
			
			
		}
	}*/
	
	
	for (int i = 0; i < map.photons.size(); ++i)
	{
		out << "v " << map.photons[i]->position.x << " " << map.photons[i]->position.y << " " << map.photons[i]->position.z << "\n";
	}


	/*
	#ifdef REPORT_PROGRESS
	std::cout << "\n";
	std::cout << "Done  :  " << rendering.elapsed() << std::endl;
	#endif
	Timer balacing;
	std::cout << "Balacing Started" << std::endl;
	map.build_kd_tree();

	std::cout << "Done " << balacing.elapsed() << std::endl;
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
			vec2 pixel = vec2(i, j);
			color pixel_color = gather_photons(pixel, uv, map, 0.03, 50);
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
