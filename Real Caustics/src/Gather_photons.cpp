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
extern int number_of_photons;
color gather_photons(vec3 point, Photon_map& photon_map, float search_distance, int number_of_closest_photons)
{
	Priority_queue closest_photons(number_of_closest_photons);
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
		//flux /= closest_photons.photons.size();
		int red = static_cast<int>(flux.r);
		int green = static_cast<int>(flux.g);
		int blue = static_cast<int>(flux.b);
		/*int red = static_cast<int>(flux.r / 260000.f);
		int green = static_cast<int>(flux.g / 260000.f);
		int blue = static_cast<int>(flux.b / 260000.f);*/
		return color(red, green, blue);
	}
	

}