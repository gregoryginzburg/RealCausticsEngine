#include <vector>
#include <algorithm>
#include <cmath>
#include "vec3.h"
#include "ray.h"
#include "hittable_list.h"
#include "hittable.h"
#include "Color.h"
#include "materials.h"
#include "utils.h"
#include "matrix.h"
extern const int image_width;
extern const int image_height;
extern const double inf;

matrix_4x4 m(vec4(0.7524, -0.3568, 0.5537, -0.0), vec4(0.6587, 0.4076, -0.6325, 0.0), vec4(-0.0000, 0.8406, 0.5417, -0.0), vec4(0.5533, 1.1565, -0.8142, 1.0));
std::vector<vec3> frame{ vec3(0.5, 0.28125, -1.3888888359069824), vec3(0.5, -0.28125, -1.3888888359069824), vec3(-0.5, -0.28125, -1.3888888359069824) };
void ray_fill_color(std::vector<vec3>& hits, hittable_list& world, ray& r, int depth);

void get_x_y_rays(vec3& width, vec3& height, int number_of_rays, int& width_rays_number, int& height_rays_number)
{
	bool width_greater_height = true;
	double ratio = width.length() / height.length();
	if (ratio < 1)
	{
		ratio += 1;
		width_greater_height = false;
	}
	//k - коэффицент пропорциональности
	double k = std::sqrt(number_of_rays / ratio);
	int first_rays = static_cast<int>(k);
	int second_rays = static_cast<int>(ratio * k);
	if (width_greater_height)
	{
		height_rays_number = std::fmin(first_rays, second_rays);
		width_rays_number = std::fmax(first_rays, second_rays);
	}
	else
	{
		height_rays_number = std::fmax(first_rays, second_rays);
		width_rays_number = std::fmin(first_rays, second_rays);
	}
}
void calculate_influence(hittable_list& world, ray& r, hit_rec& rec, bool& was_refracted, bool& caugth_by_cather, int depth)
{
	if (depth == 0)
	{
		return;
	}
	ray scattered_ray;
	if (world.hit(r, 0.000001, inf, rec))
	{
		if (rec.mat_ptr->scatter(r, rec, scattered_ray))
		{
			was_refracted = true;
			return calculate_influence(world, scattered_ray, rec, was_refracted, caugth_by_cather, depth - 1);
		}
		else
		{
			caugth_by_cather = true;
			return;
		}
	}
}

void rays_arealigth(std::vector<vec3>& hits, hittable_list& world, vec3 width, vec3 height, vec3 bottom_left_corner, int number_of_rays,  int depth)
{
	if (number_of_rays == 0)
	{
		return;
	}
	if (depth == 0)
	{
		int width_rays_number;
		int height_rays_number;
		get_x_y_rays(width, height, number_of_rays, width_rays_number, height_rays_number);
		for (double j = height_rays_number - 0.5; j > 0; --j)
		{
			for (double i = 0.5; i < width_rays_number; ++i)
			{
				auto u = i / (double(width_rays_number) );
				auto v = j / (double(height_rays_number) );
				ray r = ray(bottom_left_corner + u * width + v * height, vec3(0, 0, -1));
				ray_fill_color(hits, world, r, 2);
			}
		}
		return;
	}
	else
	{
		int width_rays_number;
		int height_rays_number;
		get_x_y_rays(width, height, number_of_rays, width_rays_number, height_rays_number);
		for (double j = height_rays_number - 0.5; j > 0; --j)
		{

			for (double i = 0.5; i < width_rays_number; ++i)
			{
				auto u = i / (double(width_rays_number) );
				auto v = j / (double(height_rays_number) );
				ray r = ray(bottom_left_corner + u * width + v * height, vec3(0, 0, -1));
				bool was_refracted = false;
				bool caugth_by_cather = false;
				bool visible_by_camera = false;
				hit_rec rec;
				calculate_influence(world, r, rec, was_refracted, caugth_by_cather, 2);
				if (visible_in_frustum(m, frame, true, rec.p, -0.09))
				{
					visible_by_camera = true;
				}
				double weight = was_refracted && caugth_by_cather && visible_by_camera ? 1. : 0.;
				vec3 width_new = width / (width_rays_number ) ;
				vec3 height_new = height / (height_rays_number) ;
				vec3 bottom_left_corner_new = r.origin - width_new / 2. - height_new / 2.;
				rays_arealigth(hits, world, width_new, height_new, bottom_left_corner_new, weight * 25, depth - 1);
			}
		}
	}
}

/*for (int j = height_rays_number; j >= 0; --j)
{
	for (int i = 0; i <= width_rays_number; ++i)
	{*/

/*for (double j = height_rays_number - 0.5; j > 0; --j)
{
	for (double i = 0.5; i < width_rays_number; ++i)*/