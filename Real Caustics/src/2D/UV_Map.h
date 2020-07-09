#ifndef UV_MAP_H
#define UV_MAP_H
#include <vector>
#include <memory>
#include "2D_triangle.h"
#include "2D_BVH.h"
#include "../vec2.h"
class UV_Map
{
public:
	std::vector<std::shared_ptr<triangle2>> triangles;
	BVHNode2* root = nullptr;
public:
	void build_bvh()
	{
		root = build_bvh_2(*this);
		triangles.clear();
		triangles.shrink_to_fit();
	}
	void get_u_v(vec2 point, hit_rec_2& rec)
	{
		hit(root, point, rec);
	}
};




#endif
