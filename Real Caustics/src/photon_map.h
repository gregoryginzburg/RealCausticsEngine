#ifndef PHOTON_MAP_H
#define PHOTON_MAP_H
#include <vector>
#include <memory>
#include "photon.h"
void calculate_bbox(std::vector<std::shared_ptr<photon>>& points);

class Photon_map
{
public:
	std::vector<std::shared_ptr<photon>> photons;
	Photon_map(int number_of_photons) 
	{
		photons.resize(number_of_photons);
	}
	void balance(int element, std::vector<std::shared_ptr<photon>> points)
	{
		//static Photon_map balanced(photon_map.photons.size());


	}
};



/*void find_photons(vec3& x, float serach_distance, std::vector<photon>& photons)
{
	if (element * 2 + 1 < photons.size())
	{
		if (points[element]->splitting_plane == 0)
		{
			dist_to_plane = 0;
		}

	};

}*/
struct sorting_photon
{
	std::shared_ptr<photon> point;
	int index;
	sorting_photons(std::shared_ptr<photon> p, int i) : point(p), index(i) {}
};

#endif
