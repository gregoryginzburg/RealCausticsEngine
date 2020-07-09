#ifndef HITTABLE_LIST_H
#define	HITTABLE_LIST_H
#include <memory>
#include <vector>
#include "vec3.h"
#include "ray.h"
#include "Hit_rec.h"
using std::shared_ptr;
using std::make_shared;

class hittable_list 
{
public:
public:
	hittable_list() {}

	bool hit(const ray& r, float tmin, float tmax, hit_rec& hit_inf) const
	{
		/*hit_rec temp_rec;
		bool hit_anything = false;
		auto closest_so_far = tmax;
		//тут проходим по указателям на каждый объект класса hittable
		for (const auto& object : objects)   
		{

			if (object->hit(r, tmin, closest_so_far, temp_rec))
			{
				hit_anything = true;
				closest_so_far = temp_rec.t;
				hit_inf = temp_rec;
			}
		}*/
		return true;
	}
	
};

#endif
