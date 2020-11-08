#ifndef BXDF_H
#define BXDF_H

#include "vec3.h"
#include "Frame.h"
#include "MLT_Sampler.h"
#include <vector>

extern const float PI;
extern const float Inv_PI;
struct Isect;



enum class TransportMode { Radiance, Importance };
enum BxDFType {
	BSDF_REFLECTION = 1 << 0,
	BSDF_TRANSMISSION = 1 << 1,
	BSDF_DIFFUSE = 1 << 2,
	BSDF_GLOSSY = 1 << 3,
	BSDF_SPECULAR = 1 << 4,
	BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR |
	BSDF_REFLECTION | BSDF_TRANSMISSION,
};
inline BxDFType operator|(BxDFType a, BxDFType b)
{
	return static_cast<BxDFType>(static_cast<int>(a) | static_cast<int>(b));
}



class BxDF
{
public:
	// wo - view direction //wi - light direction
	virtual vec3 sample(vec3& wi, 
		const vec3& geometric_normal, 
		float& forward_pdf, 
		float& reverse_pdf, 
		float& cos_theta_out,
		float& contination_prob,
		BxDFType& sampled_event, 
		MLT_Sampler& Sampler) const = 0;
	
	virtual vec3 evaluate(const vec3& wi, const vec3& geometric_normal) const = 0;

	virtual bool Is_delta() const = 0;

	virtual void pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const = 0;

	virtual float cos_theta_in() const = 0;

	virtual float reverse_pdf(const vec3& wi) const = 0;

	virtual float ContinuationProb(const vec3& wi) const = 0;

};


class Specular_Reflection : public BxDF
{
public:
	Specular_Reflection(const vec3& color, const Isect& intersection, TransportMode m);


public:
	virtual vec3 sample(vec3& wi, 
		const vec3& geometric_normal, 
		float& forward_pdf, 
		float& reverse_pdf, 
		float& cos_theta_out, 
		float& contination_prob, 
		BxDFType& sampled_event,
		MLT_Sampler& Sampler) const;

	virtual vec3 evaluate(const vec3& wi, const vec3& geometric_normal) const
	{
		return vec3(0.0f);
	}
	virtual bool Is_delta() const
	{
		return true;
	}
	virtual void pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const;

	virtual float cos_theta_in() const
	{
		return std::abs(wo_local.z);
	}
	virtual float reverse_pdf(const vec3& wi) const
	{
		return 0.0f;
	}
	virtual float ContinuationProb(const vec3& wi) const
	{
		return mContiniationProb;
	}
	
public:
	// Reflection coeeficient(color)
	const vec3 R;
	const TransportMode mode;
	float mContiniationProb;

	vec3 wo_local;
	Frame frame;

};

class Specular_Transmission : public BxDF
{
public:
	Specular_Transmission(const vec3& color, float i, const Isect& intersection, TransportMode m);
public:
	virtual vec3 sample(vec3& wi, 
		const vec3& geometric_normal, 
		float& forward_pdf, 
		float& reverse_pdf, 
		float& cos_theta_out, 
		float& contination_prob, 
		BxDFType& sampled_event,
		MLT_Sampler& Sampler) const;

	virtual vec3 evaluate(const vec3& wi, const vec3& geometric_normal) const
	{
		return vec3(0.0f);
	}
	virtual bool Is_delta() const
	{
		return true;
	}
	virtual void pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const;

	virtual float cos_theta_in() const
	{
		return std::abs(wo_local.z);
	}
	virtual float reverse_pdf(const vec3& wi) const
	{
		return 0.0f;
	}
	virtual float ContinuationProb(const vec3& wi) const
	{
		return mContiniationProb;
	}
public:
	// Transmissions coeeficient(color)
	const vec3 T;
	float mContiniationProb;

	float ior;
	const TransportMode mode;

	vec3 wo_local;
	Frame frame;
};

class Fresnel_Specular : public BxDF
{
public:
	Fresnel_Specular(const vec3& color, float i, const Isect& intersection, TransportMode m);

public:
	virtual vec3 sample(vec3& wi, 
		const vec3& geometric_normal, 
		float& forward_pdf, 
		float& reverse_pdf, 
		float& cos_theta_out, 
		float& contination_prob, 
		BxDFType& sampled_event,
		MLT_Sampler& Sampler) const;

	virtual vec3 evaluate(const vec3& wi, const vec3& geometric_normal) const
	{
		return 0.0f;
	}
	virtual bool Is_delta() const
	{
		return true;
	}
	virtual void pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const;

	virtual float cos_theta_in() const
	{
		return std::abs(wo_local.z);
	}
	virtual float reverse_pdf(const vec3& wi) const
	{
		return 0.0f;
	}
	virtual float ContinuationProb(const vec3& wi) const
	{
		return mContiniationProb;
	}
public:
	// Transmissions coeeficient(color)
	const vec3 T;
	float mContiniationProb;

	const float ior;
	const TransportMode mode;

	vec3 wo_local;
	Frame frame;
};


class Lambertian_Reflection : public BxDF
{
public:
	Lambertian_Reflection(const vec3& color, const Isect& intersection, TransportMode m);
public:
	virtual vec3 sample(vec3& wi, 
		const vec3& geometric_normal, 
		float& forward_pdf, 
		float& reverse_pdf, 
		float& cos_theta_out, 
		float& contination_prob, 
		BxDFType& sampled_event,
		MLT_Sampler& Sampler) const;

	virtual vec3 evaluate(const vec3& wi, const vec3& geometric_normal) const
	{
		// check if signs are different -> different hemispheres
		if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f)
			return 0.0f;
		return R * Inv_PI * abs_dot(wi, frame.mZ);
	}
	virtual bool Is_delta() const
	{
		return false;
	}

	virtual void pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const;

	virtual float cos_theta_in() const
	{
		return std::abs(wo_local.z);
	}

	virtual float reverse_pdf(const vec3& wi) const
	{
		// cos theta view dir
		return std::abs(wo_local.z) * Inv_PI;
	}
	virtual float ContinuationProb(const vec3& wi) const
	{
		return mContiniationProb;
	}
public:
	// albedo
	const vec3 R;
	const TransportMode mode;
	float mContiniationProb;

	vec3 wo_local;
	Frame frame;
};


class Microfacet_Reflection_GGX : public BxDF
{
public:
	Microfacet_Reflection_GGX(const vec3& color, float roughness, const Isect& intersection, TransportMode m);

public:
	virtual vec3 sample(vec3& wi, 
		const vec3& geometric_normal, 
		float& forward_pdf, 
		float& reverse_pdf, 
		float& cos_theta_out, 
		float& contination_prob, 
		BxDFType& sampled_event,
		MLT_Sampler& Sampler) const;


	virtual vec3 evaluate(const vec3& wi, const vec3& geometric_normal) const;

	virtual bool Is_delta() const
	{
		return false;
	}

	virtual void pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const;

	virtual float reverse_pdf(const vec3& wi) const
	{
		float forward_pdf, reverse_pdf;
		pdf(wi, forward_pdf, reverse_pdf);
		return reverse_pdf;
	}

	virtual float cos_theta_in() const
	{
		return std::abs(wo_local.z);
	}
	virtual float ContinuationProb(const vec3& wi) const
	{
		return mContiniationProb;
	}
public:
	const vec3 R;
	const float a2;
	const TransportMode mode;
	float mContiniationProb;

	vec3 wo_local;
	Frame frame;
};



class Fresnel_Microfacet_GGX : public BxDF
{
public:
	Fresnel_Microfacet_GGX(const vec3& color, float roughness, float specular, const Isect& intersection, TransportMode m);

public:
	virtual vec3 sample(vec3& wi, 
		const vec3& geometric_normal, 
		float& forward_pdf, 
		float& reverse_pdf, 
		float& cos_theta_out, 
		float& contination_prob, 
		BxDFType& sampled_event,
		MLT_Sampler& Sampler) const;

	virtual vec3 evaluate(const vec3& wi, const vec3& geometric_normal) const;

	virtual bool Is_delta() const
	{
		return false;
	}

	virtual void pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const;

	virtual float reverse_pdf(const vec3& wi) const
	{
		float forward_pdf, reverse_pdf;
		pdf(wi, forward_pdf, reverse_pdf);
		return reverse_pdf;
	}

	virtual float cos_theta_in() const
	{
		return std::abs(wo_local.z);
	}
	virtual float ContinuationProb(const vec3& wi) const
	{
		return mContiniationProb;
	}

public:
	const vec3 R;
	const float a2;
	const float s;
	const TransportMode mode;
	float mContiniationProb = 1.0f;

	vec3 wo_local;
	Frame frame;
};



class Fresnel_Specular_Microfacet_GGX : public BxDF
{
public:
	Fresnel_Specular_Microfacet_GGX(const vec3& color, float specular, const Isect& intersection, TransportMode m);

public:
	virtual vec3 sample(vec3& wi, 
		const vec3& geometric_normal, 
		float& forward_pdf, 
		float& reverse_pdf, 
		float& cos_theta_out, 
		float& contination_prob, 
		BxDFType& sampled_event,
		MLT_Sampler& Sampler) const;

	virtual vec3 evaluate(const vec3& wi, const vec3& geometric_normal) const;

	virtual bool Is_delta() const
	{
		return false;
	}

	virtual void pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const;

	virtual float reverse_pdf(const vec3& wi) const
	{
		float forward_pdf, reverse_pdf;
		pdf(wi, forward_pdf, reverse_pdf);
		return reverse_pdf;
	}

	virtual float cos_theta_in() const
	{
		return std::abs(wo_local.z);
	}
	virtual float ContinuationProb(const vec3& wi) const
	{
		return mContiniationProb;
	}
public:
	const vec3 R;
	const float s;
	const TransportMode mode;
	float mContiniationProb = 1.0f;

	vec3 wo_local;
	Frame frame;
};




float correct_shading_normal(const vec3& wo, const vec3& wi, const Isect& intersection);


#endif // !BXDF_H

