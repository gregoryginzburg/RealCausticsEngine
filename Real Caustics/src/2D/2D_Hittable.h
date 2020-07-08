#ifndef _HITTABLE_H
#define _HITTABLE_H
struct hit_rec
{
	float u;
	float v;
	virtual bool hit(vec2 point, hit_rec rec) = 0;
};





#endif