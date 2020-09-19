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



/*ray Lights_list::emit_photon(helper_light_emit& h) const
{
	float numbers_photons_light = (int)(weights[h.light] * scene->number_of_photons); 

	float current_power = lights[h.light]->get_power() / (float)(numbers_photons_light - 1);
	int x, y;
	
	lights[h.light]->geyt_i_j(numbers_photons_light, x, y);

	if (h.current_call > numbers_photons_light - 2)
	{
		h.light += 1;
		if (h.light > lights.size() - 1)
		{
			return ray(vec3(0, 0, 0), vec3(0, 0, 0));
		}
		numbers_photons_light = static_cast<int>(weights[h.light] * scene->number_of_photons);
		current_power = lights[h.light]->get_power() / (float)(numbers_photons_light - 1);
		h.current_call = 0;
		return lights[h.light]->emit_photon(h.current_call, h.current_call % x, h.current_call / y, current_power, x, y);
	}

	h.current_call += 1;
	return lights[h.light]->emit_photon(h.current_call, h.current_call % x, h.current_call / x, current_power, x, y);
}*/

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


// : r - in radians (gets converted into rotation - radians)
// : a - angle in radians
Sun_Light::Sun_Light(vec3 p, vec3 r, float a, float pow, colorf c) : position(p), power(pow), rotation(r), color(c)
{
	//spread = 1.0f - (cos(a) + 1.0f) / 2.0f;
	rotate_vec_rad(normal, vec3(0, 0, 0), r);
	matrix_3x3 matrix(rotation);
	matrix_3x3 inverted_matrix = matrix;
	transpose(inverted_matrix);
	std::vector<vec3> projected_bbox_points;
	projected_bbox_points.resize(8);
	std::vector<vec3> projected_bbox_points_local_space;
	projected_bbox_points_local_space.resize(8);

	aabb bbox = scene->bounding_box;

	// project scene bounding box points onto sun's plane
	projected_bbox_points[0] = project_onto_plane(bbox.min, position, normal);
	projected_bbox_points[1] = project_onto_plane(vec3(bbox.max.x, bbox.min.y, bbox.min.z), position, normal);
	projected_bbox_points[2] = project_onto_plane(vec3(bbox.min.x, bbox.max.y, bbox.min.z), position, normal);
	projected_bbox_points[3] = project_onto_plane(vec3(bbox.max.x, bbox.max.y, bbox.min.z), position, normal);

	projected_bbox_points[4] = project_onto_plane(vec3(bbox.min.x, bbox.min.y, bbox.max.z), position, normal);
	projected_bbox_points[5] = project_onto_plane(vec3(bbox.max.x, bbox.min.y, bbox.max.z), position, normal);
	projected_bbox_points[6] = project_onto_plane(vec3(bbox.min.x, bbox.max.y, bbox.max.z), position, normal);
	projected_bbox_points[7] = project_onto_plane(bbox.max, position, normal);

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
	bottom_left_corner = min * matrix + position;

	vec3 bottom_right_corner = bottom_right_local * matrix + position;
	vec3 top_left_corner = top_left_local * matrix + position;

	horizontal = bottom_right_corner - bottom_left_corner;
	vertical = top_left_corner - bottom_left_corner;

	width = horizontal.length();
	height = vertical.length();

}

ray Sun_Light::emit_photon(size_t ii, int total_number_of_photons) const
{
	//vec3 origin = bottom_left_corner + (i / total_i) * horizontal + (j/ total_j) * vertical;
	//vec3 origin = bottom_left_corner + random_float_0_1() * horizontal + random_float_0_1() * vertical;
	vec3 origin = bottom_left_corner + halton_sequnce(ii, 2) * horizontal + halton_sequnce(ii, 3) * vertical;
	//vec3 target = origin + normal + random_in_hemisphere();
	return ray(origin, normal, color * (power / total_number_of_photons));
}

float Sun_Light::get_power() const
{
	return power;
}


void Sun_Light::geyt_i_j(int number_of_rays, int &i, int &j)
{
	float k = std::sqrt(number_of_rays / (width / height));
	i = (int)k;
	j = (int)(number_of_rays / k);
}


// :rot in radians
Area_Light::Area_Light(vec3 p, vec3 rot, float w, float h, float pow, float spr, colorf c) : color(c), power(pow), width(w), height(h), position(p), spread(spr)
{
	// :rot - in degress
	bottom_left_corner = p - vec3(w / 2.f, h / 2.f, 0.f);
	horizontal = vec3(w, 0., 0.);
	vertical = vec3(0., h, 0.);
	rotate(rot);
}


ray Area_Light::emit_photon(size_t ii, int total_number_of_photons) const
{
	vec3 origin = bottom_left_corner + halton_sequnce(ii, 2) * horizontal + halton_sequnce(ii, 3) * vertical;
	vec3 target = origin + normal + spread * random_in_hemisphere();

	return ray(origin, target - origin, color * (power / total_number_of_photons));
}

void Area_Light::rotate(vec3 rotation)
{
	// :rotation - in degress
	vec3 top_left_corner = bottom_left_corner + vertical;
	vec3 bottom_rigth_corner = bottom_left_corner + horizontal;
	rotate_vec_rad(bottom_left_corner, position, rotation);
	rotate_vec_rad(top_left_corner, position, rotation);
	rotate_vec_rad(bottom_rigth_corner, position, rotation);
	rotate_vec_rad(normal, vec3(0, 0, 0), rotation);
	horizontal = bottom_rigth_corner - bottom_left_corner;
	vertical = top_left_corner - bottom_left_corner;
}

void Area_Light::geyt_i_j(int number_of_rays, int& i, int& j)
{
	float k = std::sqrt(number_of_rays / (1.0f + width / height));
	i = (int)k;
	j = (int)(number_of_rays / k);
}




Point_Light::Point_Light(const vec3& p, float r, float pow, colorf c) : position(p), radius(r), power(pow), color(c)
{

}


ray Point_Light::emit_photon(size_t ii, int total_number_of_photons) const
{
	return ray(position + random_point_in_sphere(radius), random_in_hemisphere(), color * (power / total_number_of_photons));
}

float Point_Light::get_power() const
{
	return power;
}

void Point_Light::geyt_i_j(int number_of_rays, int& i, int& j)
{
	i = 10;
	j = 10;
}
