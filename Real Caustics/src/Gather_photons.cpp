#include "vec3.h"
#include "ray.h"
#include "Color.h"
#include "photon_map.h"
#include "2D\UV_Map.h"
#include "priority_queue.h"
#include <cmath>

#include <iostream>
#include <fstream>

extern const int image_width;
extern const int image_height;
extern const float PI;
extern const float E;
extern int number_of_photons;
extern int no_photons = 0;

float scale = 1.5;
color gather_photons(vec3 point, Photon_map& photon_map, float search_distance, int number_of_closest_photons, std::ofstream& out)
{
	Priority_queue closest_photons(number_of_closest_photons);
	photon_map.find_closest_photons(point, search_distance * search_distance, closest_photons);
	if (closest_photons.photons.size() == 0)
	{
		no_photons += 1;
		// out << "v " << point.x << " " << point.y << " " << point.z << "\n";
		return color(0, 0, 0);	
	}
	else
	{
		colorf flux(0.f, 0.f, 0.f);
		float r = closest_photons.priorities[0];
		float delta_A = PI * r;
		for (int i = 0; i < closest_photons.photons.size(); ++i)
		{
			float weight = 0.918f * (1.f - (1.f - std::powf(E, -1.953f * closest_photons.priorities[i] / 2.f / r)) / 0.85815f);
			flux += closest_photons.photons[i]->power * weight;
			//out << "v "<< closest_photons.photons[i]->position.x << " " << closest_photons.photons[i]->position.y << " " << closest_photons.photons[i]->position.z << "\n";

		}
		flux /= delta_A;
		//flux /= closest_photons.photons.size();
		int red = static_cast<int>(flux.r * 255.999);
		int green = static_cast<int>(flux.g * 255.999);
		int blue = static_cast<int>(flux.b * 255.999);
		/*int red = static_cast<int>(flux.r / 260000.f);
		int green = static_cast<int>(flux.g / 260000.f);
		int blue = static_cast<int>(flux.b / 260000.f);*/
		// out << flux.r << " " << flux.g << " " << flux.b << "\n";
		return color(red, green, blue);
	}
	

}