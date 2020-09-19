#ifndef PHOTON_MAP_H
#define PHOTON_MAP_H
#include <vector>
#include <memory>
#include "photon.h"
#include "priority_queue.h"
#include "Color.h"
#include "aabb.h"

extern const float inf;
extern const float PI;
class Catcher;

struct KDTreeNode
{
	union
	{
		struct
		{
			// 4 bit 0 - if no objects on split
			//2 bits for split axis: 00-x 01-y 11-z
			//maxumum value = 268 435 456
			unsigned index;
			float split;
		} inner;
		struct
		{
			//higest bit 0 - leaf; 1 - inner
			unsigned index;
			unsigned start_index;
		} leaf;		
	} u;
};


struct temp_photon
{
	std::shared_ptr<photon> Photon;
	unsigned index;

	temp_photon(std::shared_ptr<photon> p, unsigned i) : Photon(p), index(i) {}
};

void update_kd_tree(bool was_changed, std::vector<std::shared_ptr<photon>>& photons, std::vector<KDTreeNode>& kdtree, const char* file_path);



class Photon_map
{
public:
	std::vector<std::shared_ptr<photon>> photons;
	std::vector<KDTreeNode> kdtree;
	std::vector<float> costheta;
	std::vector<float> sintheta;
	std::vector<float> cosphi;
	std::vector<float> sinphi;


public:
	Photon_map() {}

	void init_photon_map(int number_of_photons)
	{
		photons.reserve(number_of_photons);
		costheta.resize(256);
		sintheta.resize(256);
		cosphi.resize(256);
		sinphi.resize(256);
		for (int i = 0; i < 256; ++i)
		{
			float angle = float(i) * (1.0f / 256.0f) * PI;
			costheta[i] = std::cos(angle);
			sintheta[i] = std::sin(angle);
			cosphi[i] = std::cos(2.0f * angle);
			sinphi[i] = std::sin(2.0f * angle);
		}
	}


	void add(const vec3& p, const colorf power, const vec3& direction)
	{
		photons.emplace_back(std::make_shared<photon>(p, power, direction));
	}
	void erase_photons()
	{
		photons.clear();
		photons.shrink_to_fit();
	}

	void find_closest_photons(const vec3 &point, float& search_d, Priority_queue &closest_photons, int element);

	void gather_photons(hit_rec& rec, Catcher* material, float search_distance, int number_of_closest_photons, float* pixel_color);

	void update_kdtree(bool was_changed, const char* file_path)
	{
		update_kd_tree(was_changed, photons, kdtree, file_path);
	}

	void photon_direction(vec3& dir, unsigned char phi, unsigned char theta);
	

};

#endif