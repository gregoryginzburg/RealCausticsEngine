#ifndef PHOTON_MAP_H
#define PHOTON_MAP_H
#include <vector>
#include <memory>
#include "photon.h"
#include "priority_queue.h"
#include "Color.h"
#include "aabb.h"

extern const float inf;

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

//void find_photons(KDTreeNode* root, vec3& point, float search_d, Priority_queue& closest_photons);
void update_kd_tree(bool was_changed, std::vector<std::shared_ptr<photon>>& photons, std::vector<KDTreeNode>& kdtree, const char* file_path);

void find_photons(std::vector<KDTreeNode>& kdtree, std::vector<std::shared_ptr<photon>>& photons, const vec3& point, float search_d, Priority_queue& closest_photons, int element);


class Photon_map
{
public:
	std::vector<std::shared_ptr<photon>> photons;
	std::vector<KDTreeNode> kdtree;

	Photon_map() {}
	Photon_map(int number_of_photons)
	{
		photons.reserve(number_of_photons);
	}

	void add(const vec3& p, const colorf power)
	{
		photons.emplace_back(std::make_shared<photon>(p, power));
	}
	void erase_photons()
	{
		photons.clear();
		photons.shrink_to_fit();
	}
	void find_closest_photons(const vec3& point, float search_d, Priority_queue& closest_photons)
	{
		return find_photons(kdtree, photons, point, search_d, closest_photons, 1);
	}
	void update_kdtree(bool was_changed, const char* file_path)
	{
		update_kd_tree(was_changed, photons, kdtree, file_path);
	}
	

};

#endif