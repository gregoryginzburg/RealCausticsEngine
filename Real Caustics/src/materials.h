#ifndef MATERIALS_H
#define MATERIALS_H
#include "ray.h"
#include "vec3.h"
#include "Hit_rec.h"
#include "Color.h"
#include <cmath>
class Material
{
public:
	virtual bool scatter(const ray& r, const hit_rec& rec, ray& scattered) = 0;
	virtual colorf get_color() = 0;
};
class Metal : public Material
{
public:
	colorf color;
public:
	Metal(colorf c) : color(c) {}
public:
	virtual bool scatter(const ray& r, const hit_rec& rec, ray& scattered)
	{
		vec3 scattered_dir = reflect(r.direction, rec.normal);
		scattered = ray(rec.p, scattered_dir);
		return true;
	}
	virtual colorf get_color()
	{
		return color;
	}
};
class Glass : public Material
{
public:
	float ior = 1.0;
	colorf color;
public:
	Glass() {}
	Glass(float ref_idx, colorf c) : ior(ref_idx), color(c) {}
public:
	virtual bool scatter(const ray& r, const hit_rec& rec, ray& scattered)
	{
		if (rec.front_face) ior = 1 / ior;
		vec3 direction_normalized = normalize(r.direction);
		vec3 scattered_dir = refract(direction_normalized, rec.normal, ior);
		scattered = ray(rec.p, scattered_dir);
		return true;
	}
	virtual colorf get_color()
	{
		return color;
	}
};
class Catcher : public Material
{
public:
	virtual bool scatter(const ray& r, const hit_rec& rec, ray& scattered)
	{
		return false;
	}
	virtual colorf get_color() 
	{
		return colorf(0, 0, 0);
	}
	

};










#endif
