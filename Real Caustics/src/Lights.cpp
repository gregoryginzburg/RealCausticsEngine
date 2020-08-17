#include "Lights.h"

#include <vector>
#include <memory>
#include "vec3.h"
#include "ray.h"
#include "utils.h"
#include "random_generators.h"
#include "Color.h"
#include "Scene.h"
#include <cmath>
extern Scene* scene;
extern const float inf;

ray Area_Light::emit_photon(size_t i, size_t j, float power) const
{
	vec3 origin = bottom_left_corner + halton_sequnce(i, 2) * horizontal + halton_sequnce(j, 3) * vertical;
	vec3 target = origin + normal + random_in_hemisphere();

	return ray(origin, target - origin, colorf(power, power, power));
}

void Area_Light::rotate(vec3 rotation)
{
	vec3 top_left_corner = bottom_left_corner + vertical;
	vec3 bottom_rigth_corner = bottom_left_corner + horizontal;
	rotate_vec(bottom_left_corner, position, rotation);
	rotate_vec(top_left_corner, position, rotation);
	rotate_vec(bottom_rigth_corner, position, rotation);
	rotate_vec(normal, vec3(0, 0, 0), rotation);
	horizontal = bottom_rigth_corner - bottom_left_corner;
	vertical = top_left_corner - bottom_left_corner;
}

void Area_Light::geyt_i_j(int number_of_rays, int& i, int& j)
{
	float k = std::sqrt(number_of_rays / (1.0f + width / height));
	i = (int)k;
	j = (int)(number_of_rays / k);
}

ray Lights_list::emit_photon() const
{
	static int light = 0;
	static int numbers_photons_light = (int)(weights[light] * scene->number_of_photons);
	static float current_power = lights[light]->get_power() / (float)(numbers_photons_light - 1);
	static int current_call = -1;
	static int x;
	static int y;
	lights[light]->geyt_i_j(numbers_photons_light, x, y);
	if (current_call > numbers_photons_light - 2)
	{
		light += 1;
		if (light > lights.size() - 1)
		{
			return ray(vec3(0, 0, 0), vec3(0, 0, 0));
		}
		numbers_photons_light = static_cast<int>(weights[light] * scene->number_of_photons);
		current_power = lights[light]->get_power() / (float)(numbers_photons_light - 1);
		current_call = 0;
		return lights[light]->emit_photon(current_call % x, current_call / y, current_power);
	}

	current_call += 1;
	return lights[light]->emit_photon(current_call % x, current_call / y, current_power);
}

void Lights_list::calculate_weights()
{
	weights.reserve(lights.size());
	float sum_power = 0.f;
	for (int i = 0; i < lights.size(); ++i)
	{
		if (lights[i]->get_power() > negative_inf)
		{
			sum_power += lights[i]->get_power();
		}
		weights.push_back(lights[i]->get_power());
	}
	for (int i = 0; i < weights.size(); ++i)
	{
		weights[i] /= sum_power;
	}
}


// : r - in degrees (gets converted into rotation - radians)
Sun_Light::Sun_Light(vec3 p, vec3 r, float a, float pow) : position(p), angle(a), power(pow)  
{
	rotation = vec3(r.x * PI / 180.0f, r.y * PI / 180.0f, r.z * PI / 180.0f);
	rotate_vec(normal, vec3(0, 0, 0), r);
	matrix_3x3 matrix(rotation);
	matrix_3x3 inverted_matrix = matrix;
	invert_matrix(inverted_matrix);
	std::vector<vec3> projected_bbox_points;
	projected_bbox_points.resize(8);
	std::vector<vec3> projected_bbox_points_local_space;
	projected_bbox_points_local_space.resize(8);
	aabb bbox = scene->bounding_box;
	std::cout << "LIGHT PASSED IN COONSTRUCTOR" << std::endl;
	// project scene bounding box points onto sun's plane
	projected_bbox_points[0] = project_onto_plane(bbox.min, position, normal);
	projected_bbox_points[1] = project_onto_plane(vec3(bbox.max.x, bbox.min.y, bbox.min.z), position, normal);
	projected_bbox_points[2] = project_onto_plane(vec3(bbox.min.x, bbox.max.y, bbox.min.z), position, normal);
	projected_bbox_points[3] = project_onto_plane(vec3(bbox.max.x, bbox.max.y, bbox.min.z), position, normal);

	projected_bbox_points[4] = project_onto_plane(vec3(bbox.min.x, bbox.min.y, bbox.max.z), position, normal);
	projected_bbox_points[5] = project_onto_plane(vec3(bbox.max.x, bbox.min.y, bbox.max.z), position, normal);
	projected_bbox_points[6] = project_onto_plane(vec3(bbox.min.x, bbox.max.y, bbox.max.z), position, normal);
	projected_bbox_points[7] = project_onto_plane(bbox.max, position, normal);
	std::cout << "LIGHT PASSED IN COONSTRUCTOR" << std::endl;
	vec3 max(-inf, -inf, -inf);
	vec3 min(inf, inf, inf);
	// -position: go to world origin coordinates 
	// *inverted_matrix: transform from global coordinates to local space (z = 0)
	for (int i = 0; i < 8; ++i)
	{
		projected_bbox_points_local_space[i] = (projected_bbox_points[i] - position) * inverted_matrix;
		if (max.x > projected_bbox_points_local_space[i].x)
			max.x = projected_bbox_points_local_space[i].x;
		if (max.y > projected_bbox_points_local_space[i].y)
			max.y = projected_bbox_points_local_space[i].y;
		if (max.z > projected_bbox_points_local_space[i].z)
			max.z = projected_bbox_points_local_space[i].z;
		
		if (max.x > projected_bbox_points_local_space[i].x)
			max.x = projected_bbox_points_local_space[i].x;
		if (max.y > projected_bbox_points_local_space[i].y)
			max.y = projected_bbox_points_local_space[i].y;
		if (max.z > projected_bbox_points_local_space[i].z)
			max.z = projected_bbox_points_local_space[i].z;
	}
	std::cout << "LIGHT PASSED IN COONSTRUCTOR" << std::endl;
	//calculate all points in local space 
	vec3 bottom_right_local(max.x, min.y, 0.0f);
	vec3 top_left_local(min.x, max.y, 0.0f);
	//transform back to world space
	bottom_left_corner = min * matrix + position;
	std::cout << "LIGHT PASSED IN COONSTRUCTOR" << std::endl;
	vec3 bottom_right_corner = bottom_right_local * matrix + position;
	vec3 top_left_corner = top_left_local * matrix + position;
	std::cout << "LIGHT PASSED IN COONSTRUCTOR" << std::endl;
	horizontal = bottom_right_corner - bottom_left_corner;
	vertical = top_left_corner - bottom_left_corner;
	std::cout << "LIGHT PASSED IN COONSTRUCTOR" << std::endl;
	width = horizontal.length();
	height = vertical.length();
	std::cout << "LIGHT PASSED IN COONSTRUCTOR" << std::endl;
}

ray Sun_Light::emit_photon(size_t i, size_t j, float power) const
{
	vec3 origin = bottom_left_corner + halton_sequnce(i, 2) * horizontal + halton_sequnce(j, 3) * vertical;
	//vec3 target = origin + normal + random_in_hemisphere(angle);
	return ray(origin, normal, colorf(power, power, power));
}

float Sun_Light::get_power() const
{
	return power;
}


void Sun_Light::geyt_i_j(int number_of_rays, int &i, int &j)
{
	float k = std::sqrt(number_of_rays / (1.0f + width / height));
	i = (int)k;
	j = (int)(number_of_rays / k);
}