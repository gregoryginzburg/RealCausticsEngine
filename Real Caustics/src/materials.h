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
		if (!rec.front_face) ior = 1.0f / ior;
		vec3 direction_normalized = normalize(r.direction);
		float cos_theta = std::fmin(dot(-direction_normalized, rec.normal), 1.0);
		float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);
		if (ior * sin_theta > 1.0f)
		 {
		 	vec3 reflected = reflect(direction_normalized, rec.normal);
		 	scattered = ray(rec.p, reflected);
			return true;
		}

		vec3 refracted = refract(direction_normalized, rec.normal, ior);
		scattered = ray(rec.p, refracted);

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
