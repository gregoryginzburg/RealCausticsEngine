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
#include <mutex>
#include <future>

#define REPORT_PROGRESS

//#define UNIFORM_GRID
//#define HALTON_SEQUENCE
//#define SOBOL_SEQUENCE
#define MULTI_THREADED

extern const double inf = std::numeric_limits<double>::infinity();
extern const double PI = 3.14159265359;

extern const int image_width = 2000;
extern const int image_height = 2000;




void rays_arealigth(std::vector<vec3>& hits, hittable_list& world, vec3 width, vec3 height, vec3 bottom_left_corner, int number_of_rays, int depth);

void ray_fill_color(std::vector<vec3>& hits, hittable_list& world, ray& r, int depth)
{
	if (depth == 0)
	{
		return;
	}
	hit_rec rec;
	ray scattered_ray;
	if (world.hit(r, 0.000001, inf, rec))
	{
		if (rec.mat_ptr->scatter(r, rec, scattered_ray))
		{
			return ray_fill_color(hits, world, scattered_ray, depth - 1);
		}
		else
		{
			hits.push_back(rec.p);
			return;
		}
	}
	return;

}

ray get_ray(double u, double v)
{
	vec3 bottom_left_corner = vec3(-2, -2, 4);
	vec3 vertical = vec3(0, 4, 0);
	vec3 horizontal = vec3(4, 0, 0);
	return ray(bottom_left_corner + u * horizontal + v * vertical, vec3(0, 0, -1));
}


int main()
{
	
	Timer Summary;
	hittable_list world;
	Mesh ocean;
	Mesh plane;
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
	for (int i = 0; i < plane.number_of_tris(); ++i)
	{
		aabb temp;
		plane.triangles[i]->bounding_box(temp);
		temp.triangle = plane.triangles[i];
		temp.center = (temp.max + temp.min) * 0.5;
		work1.push_back(temp);
	}
	std::vector<aabb> work2;
	for (int i = 0; i < ocean.number_of_tris(); ++i)
	{
		aabb temp;
		ocean.triangles[i]->bounding_box(temp);
		temp.triangle = ocean.triangles[i];
		temp.center = (temp.max + temp.min) * 0.5;
		work2.push_back(temp);
	}
	ocean.triangles.clear();
	ocean.triangles.shrink_to_fit();
	plane.triangles.clear();
	plane.triangles.shrink_to_fit();
	#ifdef REPORT_PROGRESS
	std::cout << "Building BVH" << std::endl;
	Timer BVH_timer;
	#endif
	world.add(make_shared<BVH_node>(work1));
	world.add(make_shared<BVH_node>(work2));
	#ifdef REPORT_PROGRESS
	std::cout << "BVH Built  :  " << BVH_timer.elapsed() << std::endl;
	#endif

	std::vector<std::vector<color>> colors(image_height, std::vector<color>(image_width));
	Timer rendering;
	
	int height = 1000;
	int width = 1000;


	#ifdef UNIFORM_GRID
	for (int j = height - 1; j >= 0; --j)
	{
		int progress = static_cast<int>(double((double(height) - 1 - j)) / (double(height) - 1) * 100);
		#ifdef REPORT_PROGRESS
		std::cout << "Progress rendering: ";
		std::cout << progress << "%" << "\r" << std::flush;
		#endif
		for (int i = 0; i < width; ++i)
		{
			auto u = double(i) / (double(width) - 1);
			auto v = double(j) / (double(height) - 1);
			ray r = get_ray(u, v);
			ray_fill_color(colors, world, r, 2);
		}
	}
	#endif

	#ifdef HALTON_SEQUENCE
	for (int i = height * width; i > 0; --i)
	{
		double u = halton_sequnce(i, 2);
		double v = halton_sequnce(i, 3);
		ray r = get_ray(u, v);
		ray_fill_color(colors, world, r, 2);
	}
	#endif

	#ifdef SOBOL_SEQUENCE
	std::vector<vec2> points(height * width);
	TestSobol2D(points);
	for (int i = height * width; i > 0; --i)
	{
		double u = points[i].x;
		double v = points[i].y;
		ray r = get_ray(u, v);
		ray_fill_color(colors, world, r, 2);
	}
	#endif
	std::vector<vec3> hits;
	hits.reserve((unsigned int)height * width);
	rays_arealigth(hits, world, vec3(4, 0, 0), vec3(0, 4, 0), vec3(-2, -2, 4), 4000, 2);
	
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
		int progress = static_cast<int>(double((double(image_height) - 1 - j)) / (double(image_height) - 1) * 100);
		#ifdef REPORT_PROGRESS
		std::cout << "Progress writing to image: ";
		std::cout << progress << "%" << "\r" << std::flush;
		#endif

		for (int i = 0; i < image_width; ++i)
		{

			//maprange(colors[j][i].r, 0.0, max, 0.0, 255.0)
			//out << colors[j][i].r << " " << colors[j][i].g << " " << colors[j][i].b << "\n";
			color clamped = clamp(colors[j][i]);
			out << clamped.r << " " << clamped.g << " " << clamped.b << "\n";
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
