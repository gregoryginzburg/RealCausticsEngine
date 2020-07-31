#ifndef TRACE_PHOTON_H
#define TRACE_PHOTON_H

void trace_photon(Photon_map& photon_map, hittable_list& world, ray& r, int depth);
bool trace_ray(const ray& r, const hittable_list& world, hit_rec& hit_inf, int depth);

#endif 

