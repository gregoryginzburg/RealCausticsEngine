#include "vec3.h"
#include "ray.h"
#include "Color.h"
#include "photon_map.h"
#include "2D\UV_Map.h"
#include "priority_queue.h"
#include <cmath>

extern const int image_width;
extern const int image_height;
extern const float PI;
extern const float E;
color gather_photons(vec2 pixel, UV_Map& uv_map, Photon_map& photon_map, float search_distance, int number_of_closest_photons)
{
	static float delta_x = 0.5f / image_width;
	static float delta_y = 0.5f / image_height;
	vec2 u_v = vec2(pixel.x / image_width + delta_x, pixel.y / image_height + delta_y);
	vec3 point = uv_map.get_point_from_uv(u_v);
	Priority_queue closest_photons;
	photon_map.find_closest_photons(point, search_distance * search_distance, closest_photons);
	if (closest_photons.photons.size() == 0)
	{
		return color(0, 0, 0);
	}
	else
	{
		colorf flux(0.f, 0.f, 0.f);
		float r = closest_photons.priorities[0];
		float delta_A = PI * r * r;
		for (int i = 0; i < closest_photons.photons.size(); ++i)
		{
			float weight = 0.918f * (1.f - (1.f - std::powf(E, -1.953f * closest_photons.priorities[i] * closest_photons.priorities[i] / 2.f / r / r)) / 0.85815f);
			flux += closest_photons.photons[i]->power * weight;
		}
		flux /= delta_A;
		int red = static_cast<int>(flux.r / 10000.f);
		int green = static_cast<int>(flux.g / 10000.f);
		int blue = static_cast<int>(flux.b / 10000.f);
		/*int red = static_cast<int>(flux.r / 260000.f);
		int green = static_cast<int>(flux.g / 260000.f);
		int blue = static_cast<int>(flux.b / 260000.f);*/
		return color(red, green, blue);
	}
	

}