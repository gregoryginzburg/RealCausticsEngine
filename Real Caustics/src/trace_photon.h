#ifndef TRACE_PHOTON_H
#define TRACE_PHOTON_H

void trace_photon(Photon_map& photon_map, hittable_list& world, ray& r, bool& was_refracted, int depth);
bool trace_ray(const ray& r, hittable_list& world, hit_rec& rec1, int depth);

#endif 

