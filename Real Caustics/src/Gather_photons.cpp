#include "vec3.h"
#include "ray.h"
#include "Color.h"
#include "photon_map.h"
#include "2D\UV_Map.h"
#include "priority_queue.h"

extern const int image_width;
extern const int image_height;
extern const float PI;

color gather_photons(vec2 pixel, UV_Map& uv_map, Photon_map& photon_map, float search_distance, int number_of_closest_photons)
{
	static float delta_x = 0.5f / image_width;
	static float delta_y = 0.5f / image_height;
	vec2 u_v = vec2(pixel.x / image_width + delta_x, pixel.y / image_height + delta_y);
	vec3 point = uv_map.get_point_from_uv(u_v);
	Priority_queue closest_photons(number_of_closest_photons);
	photon_map.find_closest_photons(point, search_distance * search_distance, closest_photons);
	if (closest_photons.photons.size() == 0)
	{
		return color(0, 0, 0);
	}
	else
	{
		colorf flux(0.f, 0.f, 0.f);
		float delta_A = PI * closest_photons.priorities[0] * closest_photons.priorities[0];
		for (int i = 0; i < closest_photons.photons.size(); ++i)
		{
			flux += closest_photons.photons[i]->power;
			
		}
		flux /= delta_A;
		int r = (int)(flux.r * 1);
		int g = (int)(flux.g * 1);
		int b = (int)(flux.b * 1);
		return color(r, g, b);
	}
	

}