#ifndef _BVH2_H
#define _BVH2_H

#include <vector>
#include <memory>
#include <iostream>
#include "2D_triangle.h"
#include "2D_aabb.h"
#include "2D_Hit_rec.h"
#include "../vec2.h"

struct BVHNode2 {
	virtual bool IsLeaf() = 0;
	virtual ~BVHNode2() {}
};

struct BVHInner2 : BVHNode2 
{
	BVHNode2* _left = nullptr;
	BVHNode2* _right = nullptr;
	aabb2 bbox;
	BVHInner2() {}
	virtual bool IsLeaf() { return false; }
	virtual ~BVHInner2()
	{
		delete _left;
		delete _right;
	}
};

struct BVHLeaf2 : BVHNode2
{
	std::vector<std::shared_ptr<triangle2>> triangles;
	BVHLeaf2() {}
	virtual bool IsLeaf() { return true; }	
	bool hit(vec2& point, hit_rec_2& rec)
	{
		hit_rec_2 temp_rec;
		int count = 0;
		for (size_t i = 0; i < triangles.size(); ++i)
		{
			if (!(count > 1))
			{
				if (triangles[i]->hit(point, temp_rec))
				{
					rec = temp_rec;
					++count;
				}
			}
			else
			{
				std::cout << "Triangles are overlapping!" << std::endl;
				return true;
			}
		}
		return count > 0;
	}
	virtual ~BVHLeaf2() {}
};
struct aabb2_temp
{
	aabb2 bbox;
	vec2 center;
	std::shared_ptr<triangle2> triangle;
	aabb2_temp(aabb2& aabb, vec2& c, std::shared_ptr<triangle2> t) : bbox(aabb), center(c), triangle(t) {}
};
class UV_Map;
BVHNode2* build_bvh_2(UV_Map& uv_map);
bool hit(BVHNode2* root, vec2 point, hit_rec_2& rec);
void delete_bvh2(BVHNode2* root);

#endif

