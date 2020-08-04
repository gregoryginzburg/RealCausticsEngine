#include "photon.h"
#include "vec3.h"
#include "ray.h"
#include "Hit_rec.h"
#include "hittable_list.h"
#include "materials.h"
#include "photon_map.h"
#include "Color.h"
#include <memory>

extern const float inf;

void trace_photon(Photon_map& photon_map, hittable_list& world, ray& r, int depth)
{
	if (depth == 0)
	{
		return;
	}
	hit_rec rec;
	ray scattered_ray;
	if (world.hit(r, 0.0001f, inf, rec))
	{
		if (rec.mat_ptr->scatter(r, rec, scattered_ray))
		{
			scattered_ray.was_refracted = true;
			scattered_ray.power = r.power;
			scattered_ray *= rec.mat_ptr->get_color();
			return trace_photon(photon_map, world, scattered_ray, depth - 1);
		}
		else
		{
			if (r.was_refracted)
				photon_map.add(rec.p, r.power);
		}		
	}
}
bool trace_ray(const ray& r, hittable_list& world, hit_rec& rec, int depth)
{
	if (depth == 0)
	{
		return false;
	}
	ray scattered_ray;
	if (world.hit(r, 0.0001f, inf, rec))
	{
		if (rec.mat_ptr->scatter(r, rec, scattered_ray))
		{
			scattered_ray.was_refracted = true;
			return trace_ray(scattered_ray, world, rec, depth - 1);
		}
		else
		{
			// rec1 = rec;
			return true;
		}
	}
	return false;
}