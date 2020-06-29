#ifndef BVH_H
#define BVH_H

#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include "hittable_list.h"
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include "aabb.h"

class Hittable;

/*class BVH_node : public Hittable
{
public:
	BVH_node(Mesh& list) : BVH_node(list.triangles, 0, list.number_of_tris()) {}
	BVH_node(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end);

	virtual bool hit(const ray& r, double tmin, double tmax, hit_rec& hit_inf) const;
	virtual bool bounding_box(aabb& output_box) const;

public:
	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	aabb box;

};

bool BVH_node::hit(const ray& r, double tmin, double tmax, hit_rec& hit_inf) const
{
	if (!box.hit(r, tmin, tmax))
	{
		return false;
	}
	bool left_hit = left->hit(r, tmin, tmax, hit_inf);
	bool right_hit = right->hit(r, tmin, left_hit ? hit_inf.t : tmax, hit_inf);

	return left_hit || right_hit;

}
bool BVH_node::bounding_box(aabb& output_box) const
{
	output_box = box;
	return true;
}
inline bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis)
{
	aabb box_a;
	aabb box_b;
	a->bounding_box(box_a);
	b->bounding_box(box_b);

	if (axis == 0) return box_a.min.x < box_b.min.x;
	if (axis == 1) return box_a.min.y < box_b.min.y;
	return box_a.min.z < box_b.min.z;
}

inline bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
	return box_compare(a, b, 0);
}
inline bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
	return box_compare(a, b, 1);
}
inline bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
	return box_compare(a, b, 2);
}


BVH_node::BVH_node(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
{
	int axis = random_int(2);
	auto comparator = axis == 0 ? box_x_compare :
		axis == 1 ? box_y_compare :
		box_z_compare;
	size_t object_span = end - start;

	if (object_span == 1)
	{
		left = right = objects[start];
	}
	else if (object_span == 2)
	{
		if (comparator(objects[start], objects[start + 1]))
		{
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else
	{
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		auto mid = start + object_span / 2;

		left = std::make_shared<BVH_node>(objects, start, mid);
		right = std::make_shared<BVH_node>(objects, mid, end);
	}

	aabb box_left;
	aabb box_right;

	left->bounding_box(box_left);
	right->bounding_box(box_right);
	box = surrounding_box(box_left, box_right);
}*/


#endif
