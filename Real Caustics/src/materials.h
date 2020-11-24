#ifndef MATERIALS_H
#define MATERIALS_H
#include "ray.h"
#include "vec3.h"
#include "Color.h"
#include "BXDF.h"
#include <cmath>


enum MaterialType
{
	SpecularReflection,
	SpecularTransmission,
	FresnelSpecular,
	LambertianReflection,
	MicrofacetReflectionGGX,
	FresnelMicrofacetGGX,
	FresnelSpecularMicrofacetGGX,
};

class Python_Material
{
public:
	// "P"(80) - Plastic, "G"(71) - Glass, "M"(77) - Caustics Metal
	char type;

	float ior;
	float roughness;
	float specular;
	colorf color;

};


struct MaterialInfo
{
	MaterialInfo(const vec3& c, float i, float s, float r, MaterialType t) : color(c), ior(i), specular(s), roughness(r), type(t) {}

	vec3 color;
	float ior;
	float specular;
	float roughness;

	MaterialType type;
};

class Isect;

class Material
{
public:
	Material() {}
public:
	virtual void compute_scattering_functions(BxDF** brdf, const Isect& intersection, TransportMode mode) = 0;

	virtual MaterialInfo MaterialProperties() const = 0;
};

class Plastic_Material : public Material
{
public:
	Plastic_Material(const vec3& color, float r, float s) : R(color), roughness(r * r), specular(s * 0.08f) {}
public:
	virtual void compute_scattering_functions(BxDF** brdf, const Isect& intersection, TransportMode mode)
	{
		if (roughness < 0.01f)
		{
			*brdf = new Fresnel_Specular_Microfacet_GGX(R, specular, intersection, mode);		
		}
		else
		{
			*brdf = new Fresnel_Microfacet_GGX(R, roughness, specular, intersection, mode);
		}

		//*brdf = new Lambertian_Reflection(R, intersection, mode);
	}

	virtual MaterialInfo MaterialProperties() const
	{
		if (roughness < 0.01f)
			return MaterialInfo(R, -1, specular, -1, MaterialType::FresnelSpecularMicrofacetGGX);
		else
			return MaterialInfo(R, -1, specular, roughness, MaterialType::FresnelMicrofacetGGX);
	}

public:
	vec3 R;
	float roughness;
	float specular;
};


class Glass_Material : public Material
{
public:
	Glass_Material(const vec3& color, float i) : T(color), ior(i) 
	{
		std::cout << "IOR     " << ior << "\n";
	}
public:
	virtual void compute_scattering_functions(BxDF** brdf, const Isect& intersection, TransportMode mode)
	{
		//*brdf = new Specular_Transmission(T, ior, intersection, mode);
		*brdf = new Fresnel_Specular(T, ior, intersection, mode);
	}

	virtual MaterialInfo MaterialProperties() const
	{
		return MaterialInfo(T, ior, -1, -1, MaterialType::FresnelSpecular);
	}

public:
	vec3 T;
	float ior;
};

class Metal_Material : public Material
{
public:
	Metal_Material(const vec3& color, float r) : R(color), roughness(r) {}

public:
	virtual void compute_scattering_functions(BxDF** brdf, const Isect& intersection, TransportMode mode)
	{
		if (roughness < 0.01f)
		{
			*brdf = new Specular_Reflection(R, intersection, mode);
		}
		else
		{
			*brdf = new Microfacet_Reflection_GGX(R, roughness, intersection, mode);
		}
	}

	virtual MaterialInfo MaterialProperties() const
	{
		if (roughness < 0.01f)
			return MaterialInfo(R, -1, -1, -1, MaterialType::SpecularReflection);
		else
			return MaterialInfo(R, -1, -1, roughness, MaterialType::MicrofacetReflectionGGX);
	}

public:
	vec3 R;
	float roughness;
};











#endif
