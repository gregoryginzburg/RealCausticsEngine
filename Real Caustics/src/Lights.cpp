#include "Lights.h"

#include <vector>
#include <memory>
#include "vec3.h"
#include "ray.h"
#include "utils.h"
#include "random_generators.h"
#include "Color.h"
#include "Scene.h"
#include "Sampling.h"
#include <cmath>
extern Scene* scene;
extern const float inf;

extern const float PI;

Area_Light::Area_Light(vec3 p, vec3 rotation, float w, float h, float pow, vec3 c) : color(c * pow), power(pow), position(p), area(w * h), inv_area(1.0f/(w*h))
{
	bottom_left_corner = position - vec3(w / 2.f, h / 2.f, 0.f);
	horizontal = vec3(w, 0., 0.);
	vertical = vec3(0., h, 0.);
	vec3 normal = vec3(0, 0, -1);
	//rotate 
	vec3 top_left_corner = bottom_left_corner + vertical;
	vec3 bottom_rigth_corner = bottom_left_corner + horizontal;
	rotate_vec(bottom_left_corner, position, rotation);
	rotate_vec(top_left_corner, position, rotation);
	rotate_vec(bottom_rigth_corner, position, rotation);
	rotate_vec(normal, vec3(0, 0, 0), rotation);
	horizontal = bottom_rigth_corner - bottom_left_corner;
	vertical = top_left_corner - bottom_left_corner;

	frame.set_from_z(normal);
}
float Area_Light::get_power() const
{
	return power;
}

vec3 Area_Light::sample_light(vec3& origin, vec3& direction, float& pdf, MLT_Sampler& Sampler) const
{
	vec2 area_sample = Sampler.Get2D();
	origin = bottom_left_corner + horizontal * area_sample.x + vertical * area_sample.y;

	float direction_pdf;
	vec3 local_dir = sample_cos_hemisphere(direction_pdf, Sampler.Get2D());
	direction = frame.to_world(local_dir);
	
	pdf = inv_area * direction_pdf;

	return color * PI;
	// divide by dot()?
}

// : r - in radians (gets converted into rotation - radians)
Sun_Light::Sun_Light(vec3 r, float a, float pow, vec3 c) : power(pow), rotation(r), color(c * pow)
{
	rotate_vec_rad(normal, vec3(0, 0, 0), r);
	matrix_3x3 matrix(rotation);
	matrix_3x3 inverted_matrix = matrix;
	transpose(inverted_matrix);
	std::vector<vec3> projected_bbox_points;
	projected_bbox_points.resize(8);
	std::vector<vec3> projected_bbox_points_local_space;
	projected_bbox_points_local_space.resize(8);

	aabb bbox = scene->bounding_box;
	position = (bbox.max - bbox.min).length() * (-normal) + ((bbox.max + bbox.min) * 0.5f);

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

float Sun_Light::get_power() const
{
	return power;
}

vec3 Sun_Light::sample_light(vec3& origin, vec3& direction, float& pdf, MLT_Sampler& Sampler) const
{
	vec2 area_sample = Sampler.Get2D();
	Sampler.skip(2);
	origin = bottom_left_corner + horizontal * area_sample.x + vertical * area_sample.y;
	direction = normal;

	pdf = 1.0f;

	return color;
}
































Point_Light::Point_Light(const vec3& p, float r, float pow, colorf c) : position(p), radius(r), power(pow), color(c)
{

}


float Point_Light::get_power() const
{
	return power;
}

