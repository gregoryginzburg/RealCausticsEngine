#ifndef BVH_SAH_CONSTRUCTION_H
#define BVH_SAH_CONSTRUCTION_H
#include "aabb.h"
#include <memory>
#include <vector>
#include "hittable.h"

class BVH_node
{
public:
	aabb box;
};

class BVH_inner : public BVH_node
{
public:
	std::shared_ptr<BVH_node> left;
	std::shared_ptr<BVH_node> right;

};
class BVHLeaf : public BVH_node
{
public:
	std::vector<std::shared_ptr<Hittable>> triangles;


};







#endif 

