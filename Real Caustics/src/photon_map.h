#ifndef PHOTON_MAP_H
#define PHOTON_MAP_H
#include <vector>
#include <memory>
#include "photon.h"
#include "priority_queue.h"
#include "Color.h"
#include "aabb.h"

extern const float inf;

aabb construct_bbox(std::vector<std::shared_ptr<photon>>& points);


struct Points_on_median
{
	std::vector<std::shared_ptr<photon>> points;
};
struct KDTreeNode 
{
	virtual bool IsLeaf() = 0; // pure virtual
	virtual ~KDTreeNode() {}
};

struct KDTreeInner : KDTreeNode 
{
	KDTreeNode* _left = nullptr;
	KDTreeNode* _right = nullptr;
	float split = 0.f;
	char axis = 0;
	Points_on_median* points_on_median;
	virtual bool IsLeaf() { return false; }
	KDTreeInner()
	{
		points_on_median = new Points_on_median;
	}
	virtual ~KDTreeInner()
	{
		delete points_on_median;
		delete _left;
		delete _right;
	}
};

struct KDTreeLeaf : KDTreeNode 
{
	std::vector<std::shared_ptr<photon>> points;
	virtual bool IsLeaf() { return true; }
	virtual ~KDTreeLeaf() {}
};


KDTreeNode* build(std::vector<std::shared_ptr<photon>>& points);

void find_photons(KDTreeNode* root, vec3& point, float search_d, Priority_queue& closest_photons);

int CountBoxes(KDTreeNode* root);

void delete_kd_tree(KDTreeNode* root);

class Photon_map
{
public:
	std::vector<std::shared_ptr<photon>> photons;
	int capacity = 0;
	KDTreeNode* root = nullptr;

	Photon_map() {}
	Photon_map(int number_of_photons)
	{
		photons.reserve(number_of_photons);
	}
	bool is_full()
	{
		return capacity == photons.capacity();
	}
	void add(const vec3& p, const colorf power)
	{
		photons.emplace_back(std::make_shared<photon>(p, power));
		++capacity;
	}
	void erase_photons()
	{
		capacity = 0;
		photons.clear();
		photons.shrink_to_fit();
	}
	void find_closest_photons(vec3& point, float search_d, Priority_queue& closest_photons)
	{
		return find_photons(root, point, search_d, closest_photons);
	}
	void build_kd_tree()
	{
		root = build(photons);
		erase_photons();
	}

};

#endif
