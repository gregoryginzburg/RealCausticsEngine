#include "photon.h"
#include "vec3.h"
#include "ray.h"
#include "hittable.h"
#include "hittable_list.h"
#include "materials.h"
#include <memory>

extern const float inf;

void trace_photon(std::vector<photon>& photons, hittable_list& world, ray& r, int depth)
{
	if (depth == 0)
	{
		return;
	}
	hit_rec rec;
	ray scattered_ray;
	if (world.hit(r, 0.000001, inf, rec))
	{
		if (rec.mat_ptr->scatter(r, rec, scattered_ray))
		{
			return trace_photon(photons, world, scattered_ray, depth - 1);
		}
		else
		{
			//photons.push_back(rec.p);
			return;
		}
	}
	return;

}