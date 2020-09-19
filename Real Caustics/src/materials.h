#ifndef MATERIALS_H
#define MATERIALS_H
#include "ray.h"
#include "vec3.h"
#include "Hit_rec.h"
#include "Color.h"
#include <cmath>


class Python_Material
{
public:
	// "C"(67) - Catcher, "G"(71) - Caustics Glass, "M"(77) - Caustics Metal, "NULL"(0) - None 
	char type;

	float ior;
	float roughness;
	float specular;
	colorf color;
};


class Material
{
public:
	virtual bool scatter(const ray& r, const hit_rec& rec, ray& scattered, char& type) = 0;
	virtual colorf get_color() = 0;
};
class Metal : public Material
{
public:
	colorf color;
	float roughness;
public:
	Metal(const colorf& c, float r) : color(c), roughness(r) {}
public:
	virtual bool scatter(const ray& r, const hit_rec& rec, ray& scattered, char& type)
	{
		vec3 scattered_dir = reflect(r.direction, rec.normal);
		scattered = ray(rec.p, scattered_dir);

		type = 'M';
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
	float roughness;
	colorf color;
public:
	Glass() {}
	Glass(const colorf& c, float ref_idx, float r) : ior(ref_idx), color(c), roughness(r) {}
public:
	virtual bool scatter(const ray& r, const hit_rec& rec, ray& scattered, char& type)
	{
		float ior_local = ior;
		if (rec.front_face)
		{
			ior_local = 1.0f / ior;
		}
		vec3 direction_normalized = normalize(r.direction);
		float cos_theta = std::fmin(dot(-direction_normalized, rec.normal), 1.0);
		float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);
		if (ior_local * sin_theta > 1.0f)
		{
			vec3 reflected = reflect(direction_normalized, rec.normal);
			scattered = ray(rec.p, reflected);
			return true;
		}
		/*
		float reflection_prob = schlick(cos_theta, ior_local);

		if (random_float_0_1() < reflection_prob)
		{
			vec3 reflected = reflect(direction_normalized, rec.normal);
			scattered = ray(rec.p, reflected);
			return true;
		}*/
		vec3 refracted = refract(direction_normalized, rec.normal, ior_local);
		scattered = ray(rec.p, refracted);
		
		type = 'G';
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
	colorf catcher_color = colorf(1.0, 1.0, 1.0);
	float roughness = 1.0;
	float specular = 0.5;
public:
	Catcher(const colorf& color, float r, float s);
public:
	virtual bool scatter(const ray& r, const hit_rec& rec, ray& scattered, char& type)
	{
		type = 'C';
		return false;
	}
	virtual colorf get_color() 
	{
		return colorf(0, 0, 0);
	}
	colorf brdf(const vec3& light_dir, const vec3& view_dir, const vec3& normal);

};

class None : public Material
{
public:
	virtual bool scatter(const ray& r, const hit_rec& rec, ray& scattered, char& type)
	{
		type = 0;
		return false;
	}
	virtual colorf get_color()
	{
		return colorf(0, 0, 0);
	}
};










#endif
