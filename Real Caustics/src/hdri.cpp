#include "hdri.h"
#include "vec3.h"
#include "utils.h"
#include <vector>
#include "Scene.h"
#include <iostream>
#include <fstream>
#include "Color.h"
#include "ray.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"


inline void convert_to_spherical(vec3& point, float u, float v)
{
	point.x = cos(PI * (0.5 - v)) * cos(PI2 * (u - 0.5));
	point.y = -cos(PI * (0.5 - v)) * sin(PI2 * (u - 0.5));
	point.z = -sin(PI * (0.5 - v));
}

hdri_pixel get_hdri_pixel(const vec3& point_on_sphere, Scene* scene)
{
	hdri_pixel pixel;
	pixel.normal = -point_on_sphere;
	float angle = acos(dot(vec3(0, 0, -1), pixel.normal));
	vec3 axis_vec = normalize(cross(vec3(0, 0, -1), pixel.normal));

	vec3 position = point_on_sphere * 100;


	matrix_3x3 matrix(angle, axis_vec);
	matrix_3x3 inverted_matrix = matrix;
	transpose(inverted_matrix);
	std::vector<vec3> projected_bbox_points;
	projected_bbox_points.resize(8);
	std::vector<vec3> projected_bbox_points_local_space;
	projected_bbox_points_local_space.resize(8);

	aabb bbox = scene->bounding_box;

	// project scene bounding box points onto sun's plane
	projected_bbox_points[0] = project_onto_plane(bbox.min, position, pixel.normal);
	projected_bbox_points[1] = project_onto_plane(vec3(bbox.max.x, bbox.min.y, bbox.min.z), position, pixel.normal);
	projected_bbox_points[2] = project_onto_plane(vec3(bbox.min.x, bbox.max.y, bbox.min.z), position, pixel.normal);
	projected_bbox_points[3] = project_onto_plane(vec3(bbox.max.x, bbox.max.y, bbox.min.z), position, pixel.normal);

	projected_bbox_points[4] = project_onto_plane(vec3(bbox.min.x, bbox.min.y, bbox.max.z), position, pixel.normal);
	projected_bbox_points[5] = project_onto_plane(vec3(bbox.max.x, bbox.min.y, bbox.max.z), position, pixel.normal);
	projected_bbox_points[6] = project_onto_plane(vec3(bbox.min.x, bbox.max.y, bbox.max.z), position, pixel.normal);
	projected_bbox_points[7] = project_onto_plane(bbox.max, position, pixel.normal);

	vec3 max(-inf, -inf, 0.0f);
	vec3 min(inf, inf, 0.0f);
	// -position: go to world origin coordinates 
	// *inverted_matrix: transform from global coordinates to local space (z = 0)
	for (int i = 0; i < 8; ++i)
	{
		projected_bbox_points_local_space[i] = (projected_bbox_points[i] - position) * inverted_matrix;

		if (projected_bbox_points_local_space[i].x > max.x)
			max.x = projected_bbox_points_local_space[i].x;
		if (projected_bbox_points_local_space[i].y > max.y)
			max.y = projected_bbox_points_local_space[i].y;


		if (projected_bbox_points_local_space[i].x < min.x)
			min.x = projected_bbox_points_local_space[i].x;
		if (projected_bbox_points_local_space[i].y < min.y)
			min.y = projected_bbox_points_local_space[i].y;

	}

	//calculate all points in local space 
	vec3 bottom_right_local(max.x, min.y, 0.0f);
	vec3 top_left_local(min.x, max.y, 0.0f);
	//transform back to world space
	pixel.bottom_left_corner = min * matrix + position;

	vec3 bottom_right_corner = bottom_right_local * matrix + position;
	vec3 top_left_corner = top_left_local * matrix + position;

	pixel.horizontal = bottom_right_corner - pixel.bottom_left_corner;
	pixel.vertical = top_left_corner - pixel.bottom_left_corner;
	return pixel;
}

void trace_photons_from_hdri(Scene* scene)
{
	int width;
	int height;
	int n;
	float* data = stbi_loadf("scene_hdri.hdr", &width, &height, &n, 3);
	if (!data)
	{
		std::cout << "ERROR OPENING HDRI";
		return;
	}
	std::vector<float> weights;

	int number_of_pixels = width * height;
	weights.reserve(number_of_pixels);
	float sum_power = 0.0f;
	int ii = 0;
	for (int i = 0; i < number_of_pixels * 3; i += 3)
	{
		float value = (data[i] + data[i + 1] + data[i + 2]) / 3.0f;
		value = std::pow(value, 1.25);
		weights[ii] = value;
		++ii;
		sum_power = sum_power + value;
	}
	//std::ofstream out;
	//out.open("data_weight.txt");

	std::ofstream ou;
	ou.open("D:\ALL_sphere_obj.obj");
	std::ofstream o;
	o.open("D:\sphere_obj.obj");
	float biggest = 0.0;
	int iii = 0;

	using std::cos;
	using std::sin;
	for (int i = 0; i < number_of_pixels; ++i)
	{
		weights[i] /= sum_power;
		//out << weights[i] << "\n";
		if (weights[i] > biggest)
		{
			iii = i;
			biggest = weights[i];
		}
	}
	int threshold = 100;

	int pixel_number = 0;
	float smallest_v = 1.0 / (float)height / 2.0;
	float smalles_u = 1.0 / (float)width / 2.0;

	int number_of_photons_emitted = 0;

	for (int j = height; j > 0; --j)
	{
		std::cout << "Progress writing to image: " << 100.0f - (j / (float)height) * 100.0f << "%" << std::flush << "\r";

		for (int i = 0; i < width; ++i)
		{
			float v = (j / (float)height) + smallest_v;
			float u = i / (float)width + smalles_u;
			vec3 point;
			convert_to_spherical(point, u, v);
			int number_of_points = weights[pixel_number] * scene->number_of_photons;
			if (number_of_points > threshold)
			{
				//o << "v " << point.x << " " << point.y << " " << point.z << "\n";
				hdri_pixel pixel = get_hdri_pixel(point, scene);
				for (int i = 0; i < number_of_points; ++i)
				{
					vec3 origin = pixel.bottom_left_corner + pixel.horizontal * halton_sequnce(i, 2) + pixel.vertical * halton_sequnce(i, 3);
					//ou << "v " << origin.x << " " << origin.y << " " << origin.z << "\n";
					ray r = ray(origin,
						pixel.normal,
						colorf(data[pixel_number * 3] / scene->number_of_photons,
							data[pixel_number * 3 + 1] / scene->number_of_photons,
							data[pixel_number * 3 + 2] / scene->number_of_photons));
					bool was_refracted = false;
					scene->trace_photon(r, 5, was_refracted);
					++number_of_photons_emitted;
				}
			}


			++pixel_number;
		}
	}
	std::cout << biggest << "   " << iii << "\n";
	std::cout << "Number of photons emitted  " << number_of_photons_emitted << "\n";
}