#ifndef HDRI_H
#define HDRI_H
class Scene;
#include "vec3.h"

struct hdri_pixel
{
	vec3 bottom_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 normal;
};
void trace_photons_from_hdri(Scene* scene);


#endif // !HDRI_H