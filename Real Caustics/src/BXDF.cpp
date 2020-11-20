#include "BXDF.h"
#include "vec3.h"
#include "Sampling.h"
#include "Hit_rec.h"
#include "vec2.h"
#include <cmath>


// used when tracing particles (importance)
float correct_shading_normal(const vec3& wo, const vec3& wi, const Isect& intersection)
{
	return (fabs(dot(wo, intersection.shade_normal)) * fabs(dot(wi, intersection.geometric_normal))) /
		(fabs(dot(wo, intersection.geometric_normal)) * fabs(dot(wi, intersection.shade_normal)));
}

float fresnel_dielectric_eta(float cos_theta, float etai_over_etat)
{
	/* compute fresnel reflectance without explicitly computing
	 * the refracted direction */
	float g = etai_over_etat * etai_over_etat - 1.0f + cos_theta * cos_theta;
	float result;

	if (g > 0.0f) {
		g = sqrt(g);
		float A = (g - cos_theta) / (g + cos_theta);
		float B = (cos_theta * (g + cos_theta) - 1.0f) / (cos_theta * (g - cos_theta) + 1.0f);
		result = 0.5f * A * A * (1.0f + B * B);
	}
	else
		result = 1.0f; /* TIR (no refracted component) */

	return result;
}

float fresnel_dielectric_reflectance(float cos_theta, float reflectance)
{
	/* compute fresnel reflectance without explicitly computing
	 * the refracted direction */
	if (reflectance >= 1.0f)
		return 1.0f;
	float n = (1.0f + sqrt(reflectance)) / (1.0f - sqrt(reflectance));
	float g = n * n - 1 + cos_theta * cos_theta;

	if (g > 0.0f) {
		g = sqrt(g);
		float A = (g - cos_theta) / (g + cos_theta);
		float B = (cos_theta * (g + cos_theta) - 1.0f) / (cos_theta * (g - cos_theta) + 1.0f);
		return 0.5f * A * A * (1.0f + B * B);
	}
	else
		return 1.0f; /* TIR (no refracted component) */

}


vec3 convert_from_spherical_coordinates(float theta, float phi)
{
	float sin_theta = sin(theta);
	return vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos(theta));
}
// -------------------------------------
// SPECULAR REFLECTION BRDF
// -------------------------------------

Specular_Reflection::Specular_Reflection(const vec3& color, const Isect& intersection, TransportMode m) : R(color), mode(m)
{
	frame.set_from_z(intersection.shade_normal);
	wo_local = frame.to_local(-intersection.direction);
	if (std::abs(wo_local.z) < 1e-6f)
		mIsValid = false;
	else
		mIsValid = true;
	mContiniationProb = std::max(0.05f, std::max(std::max(color.x, color.y), color.z));
}

vec3 Specular_Reflection::sample(vec3& wi, 
	const vec3& geometric_normal, 
	float& forward_pdf, 
	float& reverse_pdf, 
	float& cos_theta_out, 
	float& contination_prob, 
	BxDFType& sampled_event,
	MLT_Sampler& Sampler) const
{
	Sampler.skip(3);

	sampled_event = BSDF_REFLECTION | BSDF_GLOSSY | BSDF_SPECULAR;
	// Perfect Specular reflection in local frame
	vec3 wi_local = vec3(-wo_local.x, -wo_local.y, wo_local.z);

	cos_theta_out = fabs(wi_local.z);
	if (cos_theta_out < 1e-6f)
		return 0.0f;
	wi = frame.to_world(wi_local);

	if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f)
		return 0.0f;

	forward_pdf = 1.0f;
	reverse_pdf = 1.0f;

	contination_prob = mContiniationProb;
	return R;
}

void Specular_Reflection::pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const
{
	forward_pdf = 0.0f;
	reverse_pdf = 0.0f;
}

// -------------------------------------
// SPECULAR TRANSMISSION BRDF
// -------------------------------------

Specular_Transmission::Specular_Transmission(const vec3& color, float i, const Isect& intersection, TransportMode m) : T(color), ior(i), mode(m)
{
	frame.set_from_z(intersection.shade_normal);
	wo_local = frame.to_local(-intersection.direction);
	if (std::abs(wo_local.z) < 1e-6f)
		mIsValid = false;
	else
		mIsValid = true;
	mContiniationProb = std::max(0.05f, std::max(std::max(color.x, color.y), color.z));
}

vec3 Specular_Transmission::sample(vec3& wi, 
	const vec3& geometric_normal, 
	float& forward_pdf, 
	float& reverse_pdf, 
	float& cos_theta_out, 
	float& contination_prob, 
	BxDFType& sampled_event,
	MLT_Sampler& Sampler) const
{
	Sampler.skip(3);

	sampled_event = BSDF_TRANSMISSION | BSDF_GLOSSY | BSDF_SPECULAR;
	float etai_over_etat = ior;
	// if entering from normal side(in the substance)
	if (wo_local.z > 0)
		etai_over_etat = 1.0f / ior;

	vec3 wi_local;
	if (!refract(wo_local, wo_local.z > 0 ? vec3(0, 0, 1) : vec3(0, 0, -1), etai_over_etat, wi_local))
		return 0.0f;

	cos_theta_out = fabs(wi_local.z);
	if (cos_theta_out < 1e-6f)
		return 0.0f;

	wi = frame.to_world(wi_local);
	if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) >= 0.0f)
		return 0.0f;

	forward_pdf = 1.0f;
	reverse_pdf = 1.0f;

	contination_prob = mContiniationProb;

	if (mode == TransportMode::Radiance)
		return T * etai_over_etat * etai_over_etat;
	return T;
}

void Specular_Transmission::pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const
{
	forward_pdf = 0.0f;
	reverse_pdf = 0.0f;
}

// -------------------------------------
// FRESNEL SPECULAR TRANSMISSION BRDF
// -------------------------------------

Fresnel_Specular::Fresnel_Specular(const vec3& color, float i, const Isect& intersection, TransportMode m) : T(color), ior(i), mode(m)
{
	frame.set_from_z(intersection.shade_normal);
	wo_local = frame.to_local(-intersection.direction);
	if (std::abs(wo_local.z) < 1e-6f)
		mIsValid = false;
	else
		mIsValid = true;
	mContiniationProb = std::max(0.05f, std::max(std::max(color.x, color.y), color.z));
}

vec3 Fresnel_Specular::sample(vec3& wi, 
	const vec3& geometric_normal, 
	float& forward_pdf, 
	float& reverse_pdf, 
	float& cos_theta_out, 
	float& contination_prob, 
	BxDFType& sampled_event,
	MLT_Sampler& Sampler) const
{
	vec3 sample = Sampler.Get3D();

	float etai_over_etat = ior;
	// if entering from normal side(in the substance)
	if (wo_local.z > 0)
		etai_over_etat = 1.0f / ior;

	float u = sample.x;
	float f = fresnel_dielectric_eta(fabs(wo_local.z), 1 / etai_over_etat);

	if (u < f)
	{
		//Reflection
		sampled_event = BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION;

		vec3 wi_local = vec3(-wo_local.x, -wo_local.y, wo_local.z);

		cos_theta_out = fabs(wi_local.z);
		if (cos_theta_out < 1e-6f)
			return 0.0f;

		wi = frame.to_world(wi_local);

		if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f)
			return 0.0f;

		forward_pdf = f;
		reverse_pdf = f;

		contination_prob = mContiniationProb;

		return T;
	}
	else
	{
		//Refraction
		sampled_event = BSDF_TRANSMISSION | BSDF_GLOSSY | BSDF_SPECULAR;

		vec3 wi_local;
		if (!refract(wo_local, wo_local.z > 0 ? vec3(0, 0, 1) : vec3(0, 0, -1), etai_over_etat, wi_local))
			return 0.0f;

		cos_theta_out = fabs(wi_local.z);
		if (cos_theta_out < 1e-6f)
			return 0.0f;

		wi = frame.to_world(wi_local);

		if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) >= 0.0f)
			return 0.0f;

		forward_pdf = 1.0f - f;
		reverse_pdf = 1.0f - f;

		contination_prob = mContiniationProb;

		if (mode == TransportMode::Radiance)
			return T * etai_over_etat * etai_over_etat;
		return T;
	}
}

void Fresnel_Specular::pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const
{
	forward_pdf = 0.0f;
	reverse_pdf = 0.0f;
}

// -------------------------------------
// LAMBERTIAN BRDF
// -------------------------------------

Lambertian_Reflection::Lambertian_Reflection(const vec3& color, const Isect& intersection, TransportMode m) : R(color), mode(m)
{
	frame.set_from_z(intersection.shade_normal);
	wo_local = frame.to_local(-intersection.direction);
	if (std::abs(wo_local.z) < 1e-6f)
		mIsValid = false;
	else
		mIsValid = true;
	mContiniationProb = std::max(0.05f, std::max(std::max(color.x, color.y), color.z));
}

vec3 Lambertian_Reflection::sample(vec3& wi, 
	const vec3& geometric_normal, 
	float& forward_pdf, 
	float& reverse_pdf, 
	float& cos_theta_out, 
	float& contination_prob, 
	BxDFType& sampled_event,
	MLT_Sampler& Sampler) const
{
	vec3 sample = Sampler.Get3D();

	sampled_event = BSDF_DIFFUSE;

	vec2 e = vec2(sample.y, sample.z);

	vec3 wi_local = sample_cos_hemisphere(e);
	cos_theta_out = wi_local.z;
	if (cos_theta_out < 1e-6f)
		return 0.0f;

	
	// If we hit from the other side of normal invert sampled direction to remain in the same hemisphere
	if (wo_local.z < 0)
		wi_local.z = -wi_local.z;

	wi = frame.to_world(wi_local);
	forward_pdf = cos_theta_out * Inv_PI;
	// wo.local.z - cos a with normal of incoming(view vector)
	reverse_pdf = fabs(wo_local.z) * Inv_PI;

	contination_prob = mContiniationProb;

	return R;
}

void Lambertian_Reflection::pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const
{
	forward_pdf = fabs(frame.to_local(wi).z) * Inv_PI;
	reverse_pdf = fabs(wo_local.z) * Inv_PI;
}

// -------------------------------------
// MICROFACET REFLECTION GGX BRDF
// -------------------------------------


Microfacet_Reflection_GGX::Microfacet_Reflection_GGX(const vec3& color, float roughness, const Isect& intersection, TransportMode m) :
	R(color), a2(roughness* roughness), mode(m)
{
	frame.set_from_z(intersection.shade_normal);
	wo_local = frame.to_local(-intersection.direction);
	if (std::abs(wo_local.z) < 1e-6f)
		mIsValid = false;
	else
		mIsValid = true;
	mContiniationProb = std::max(0.05f, std::max(std::max(color.x, color.y), color.z));
}

float D(float NdotH, float sqr_a)
{
	float den = NdotH * NdotH * (sqr_a - 1.0f) + 1.0f;
	return sqr_a / (PI * den * den);
}


float G2(float NdotW, float NdotL, float sqr_a)
{

	float denomA = NdotW * sqrtf(sqr_a + (1.0f - sqr_a) * NdotL * NdotL);
	float denomB = NdotL * sqrtf(sqr_a + (1.0f - sqr_a) * NdotW * NdotW);

	return 2.0f * NdotL * NdotW / (denomA + denomB);
}

vec3 Microfacet_Reflection_GGX::sample(vec3& wi, const vec3& geometric_normal, float& forward_pdf, float& reverse_pdf, float& cos_theta_out, float& contination_prob, BxDFType& sampled_event,
	MLT_Sampler& Sampler) const
{
	vec3 sample = Sampler.Get3D();

	sampled_event = BSDF_GLOSSY | BSDF_REFLECTION;

	float e1 = sample.y;

	float theta = std::acos(std::sqrt((1.0f - e1) / (e1 * (a2 - 1.0f) + 1.0f)));
	float phi = sample.z * PI2;

	vec3 half_vector = convert_from_spherical_coordinates(theta, phi);

	if (wo_local.z < 0)
		half_vector.z = -half_vector.z;
	vec3 wi_local = reflect(-wo_local, half_vector);

	cos_theta_out = fabs(wi_local.z);
	if (cos_theta_out < 1e-6f)
		return 0.0f;

	wi = frame.to_world(wi_local);
	if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f || wi_local.z * wo_local.z <= 0.0f)
		return 0.0f;

	float cos_h_n = fabs(half_vector.z);
	float d = D(cos_h_n, a2);

	forward_pdf = reverse_pdf = d * cos_h_n * 0.25f / abs_dot(half_vector, wo_local);

	contination_prob = mContiniationProb;

	float g2 = G2(fabs(wo_local.z), fabs(wi_local.z), a2);
	return R * g2 * abs_dot(wo_local, half_vector) / (fabs(wo_local.z) * cos_h_n);
}

vec3 Microfacet_Reflection_GGX::evaluate(const vec3& wi, const vec3& geometric_normal) const
{
	if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f)
		return 0.0f;

	//wo_local.z = dot(normal, wo)
	vec3 wi_local = frame.to_local(wi);
	if (wi_local.z * wo_local.z <= 0.0f)
		return 0.0f;

	float g2 = G2(fabs(wo_local.z), fabs(wi_local.z), a2);
	vec3 half_vector = normalize(wo_local + wi_local);

	float cos_h_n = fabs(half_vector.z);
	float d = D(cos_h_n, a2);

	return R * d * g2 * 0.25f / fabs(wo_local.z);
}

void Microfacet_Reflection_GGX::pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const
{
	vec3 wi_local = frame.to_local(wi);
	vec3 half_vector = normalize(wo_local + wi_local);

	float cos_h_n = fabs(half_vector.z);
	float d = D(cos_h_n, a2);

	forward_pdf = reverse_pdf = d * cos_h_n * 0.25f / abs_dot(half_vector, wo_local);
}

// -------------------------------------
// FRESNEL MICROFACET REFLECTION GGX BRDF
// -------------------------------------

Fresnel_Microfacet_GGX::Fresnel_Microfacet_GGX(const vec3& color, float roughness, float specular, const Isect& intersection, TransportMode m) : 
	R(color), a2(roughness * roughness), s(specular), mode(m)
{
	frame.set_from_z(intersection.shade_normal);
	wo_local = frame.to_local(-intersection.direction);
	if (std::abs(wo_local.z) < 1e-6f)
		mIsValid = false;
	else
		mIsValid = true;
}

vec3 Fresnel_Microfacet_GGX::sample(vec3& wi, const vec3& geometric_normal, float& forward_pdf, float& reverse_pdf, float& cos_theta_out, float& contination_prob, BxDFType& sampled_event,
	MLT_Sampler& Sampler) const
{
	vec3 sample = Sampler.Get3D();

	float u = sample.x;

	float f = fresnel_dielectric_reflectance(fabs(wo_local.z), s);


	if (u < f)
	{
		// Reflect microfacet
		sampled_event = BSDF_GLOSSY | BSDF_REFLECTION;

		float e1 = sample.y;

		float theta = std::acos(std::sqrt((1.0f - e1) / (e1 * (a2 - 1.0f) + 1.0f)));
		float phi = sample.z * PI2;

		vec3 half_vector = convert_from_spherical_coordinates(theta, phi);

		if (wo_local.z < 0)
			half_vector.z = -half_vector.z;
		vec3 wi_local = reflect(-wo_local, half_vector);

		cos_theta_out = fabs(wi_local.z);
		if (cos_theta_out < 1e-6f)
			return 0.0f;

		wi = frame.to_world(wi_local);
		if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f || wi_local.z * wo_local.z <= 0.0f)
			return 0.0f;

		float g2 = G2(fabs(wo_local.z), fabs(wi_local.z), a2);

		contination_prob = std::max(0.05f, f);

		float forward_diffuse_pdf = fabs(wi_local.z) * Inv_PI;
		float reverse_diffuse_pdf = fabs(wo_local.z) * Inv_PI;

		float cos_h_n = fabs(half_vector.z);
		float d = D(cos_h_n, a2);
		float specular_pdf = d * cos_h_n * 0.25f / abs_dot(wo_local, half_vector);
		if (f > 1.0f)
		{
			forward_pdf = reverse_pdf = specular_pdf;
		}
		else
		{
			forward_pdf = forward_diffuse_pdf * (1.0f - f) + specular_pdf * f;
			reverse_pdf = reverse_diffuse_pdf * (1.0f - f) + specular_pdf * f;
		}
		
		return g2 * abs_dot(wo_local, half_vector) / (fabs(wo_local.z) * cos_h_n);

	}
	else
	{
		sampled_event = BSDF_DIFFUSE;

		vec2 e = vec2(sample.y, sample.z);

		vec3 wi_local = sample_cos_hemisphere(e);
		cos_theta_out = wi_local.z;
		if (cos_theta_out < 1e-6f)
			return 0.0f;

		// If we hit from the other side of normal invert sampled direction to remain in the same hemisphere
		if (wo_local.z < 0)
		{
			wi_local.z = -wi_local.z;
		}
		wi = frame.to_world(wi_local);
			
		if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f || wi_local.z * wo_local.z <= 0.0f)
			return 0.0f;


		contination_prob = std::max(0.05f, (R * (1.0 - f)).Max());

		float forward_diffuse_pdf = fabs(wi_local.z) * Inv_PI;
		float reverse_diffuse_pdf = fabs(wo_local.z) * Inv_PI;

		vec3 half_vector = normalize(wo_local + wi_local);
		float cos_h_n = fabs(half_vector.z);
		float d = D(cos_h_n, a2);
		float specular_pdf = d * cos_h_n * 0.25f / abs_dot(wo_local, half_vector);
		if (f > 1.0f)
		{
			forward_pdf = reverse_pdf = specular_pdf;
		}
		else
		{
			forward_pdf = forward_diffuse_pdf * (1.0f - f) + specular_pdf * f;
			reverse_pdf = reverse_diffuse_pdf * (1.0f - f) + specular_pdf * f;
		}
		
		auto pow5 = [](float v) { return (v * v) * (v * v) * v; };
		float D90 = 0.5f + cos_h_n * a2 - 1.0f;
			
		return R; //* (1.0f + D90 * pow5(1.0f - fabs(wo_local.z))) * (1.0f + D90 * pow5(1.0f - fabs(wi_local.z)));
	}


}

vec3 Fresnel_Microfacet_GGX::evaluate(const vec3& wi, const vec3& geometric_normal) const
{
	if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f)
		return 0.0f;
	float f = fresnel_dielectric_reflectance(fabs(wo_local.z), s);

	auto pow5 = [](float v) { return (v * v) * (v * v) * v; };

	vec3 wi_local = frame.to_local(wi);
	if (wi_local.z * wo_local.z <= 0.0f)
		return 0.0f;
	// Evaluate Diffuse
 
	vec3 half_vector = normalize(wi_local + wo_local);
	float D90 = 0.5f + fabs(half_vector.z) * a2 - 1.0f;
	vec3 diffuse = R * Inv_PI;//* (1.0f - f) * (1.0f + D90 * pow5(1.0f - fabs(wo_local.z))) * (1.0f + D90 * pow5(1.0f - fabs(wi_local.z))) * (1.0f - f) * fabs(wi_local.z) * Inv_PI;

	// Evaluate specular
	//wo_local.z = dot(normal, wo)
	float g2 = G2(fabs(wo_local.z), fabs(wi_local.z), a2);

	float cos_h_n = fabs(half_vector.z);
	float d = D(cos_h_n, a2);

	float specular = f * d * g2 * 0.25f / fabs(wo_local.z);

	return diffuse + specular;

}

void Fresnel_Microfacet_GGX::pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const
{
	vec3 wi_local = frame.to_local(wi);
	// Diffuse pdf
	float forward_diffuse_pdf = fabs(wi_local.z) * Inv_PI;
	float reverse_diffuse_pdf = fabs(wo_local.z) * Inv_PI;

	// Specular pdf
	vec3 half_vector = normalize(wo_local + wi_local);

	float cos_h_n = fabs(half_vector.z);
	float d = D(cos_h_n, a2);

	float forward_specular_pdf = d * cos_h_n * 0.25f / abs_dot(half_vector, wo_local);

	// Or dot(wi_local, half_vector) ????????????????????????????
	float f = fresnel_dielectric_reflectance(fabs(wo_local.z), s);
	if (f > 1.0f)
	{
		forward_pdf = reverse_pdf = forward_specular_pdf;	
	}
	else
	{
		forward_pdf = forward_diffuse_pdf * (1.0f - f) + forward_specular_pdf * f;
		reverse_pdf = reverse_diffuse_pdf * (1.0f - f) + forward_specular_pdf * f;
	}
	
}


// -------------------------------------
// FRESNEL SPECULAR MICROFACET REFLECTION GGX BRDF
// -------------------------------------


Fresnel_Specular_Microfacet_GGX::Fresnel_Specular_Microfacet_GGX(const vec3& color, float specular, const Isect& intersection, TransportMode m) :
	R(color), s(specular), mode(m)
{
	frame.set_from_z(intersection.shade_normal);
	wo_local = frame.to_local(-intersection.direction);
	if (std::abs(wo_local.z) < 1e-6f)
		mIsValid = false;
	else
		mIsValid = true;
}


vec3 Fresnel_Specular_Microfacet_GGX::sample(vec3& wi, const vec3& geometric_normal, float& forward_pdf, float& reverse_pdf, float& cos_theta_out, float& contination_prob, BxDFType& sampled_event,
	MLT_Sampler& Sampler) const
{
	vec3 sample = Sampler.Get3D();

	float u = sample.x;

	float f = fresnel_dielectric_reflectance(fabs(wo_local.z), s);

	if (u < f)
	{
		// Reflect microfacet
		sampled_event = BSDF_SPECULAR | BSDF_REFLECTION | BSDF_GLOSSY;

		vec3 wi_local = vec3(-wo_local.x, -wo_local.y, wo_local.z);

		cos_theta_out = fabs(wi_local.z);
		if (cos_theta_out < 1e-6f)
			return 0.0f;
		wi = frame.to_world(wi_local);

		if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f || wi_local.z * wo_local.z <= 0.0f)
			return 0.0f;

		forward_pdf = f;
		reverse_pdf = f;

		contination_prob = std::max(0.05f, f);

		return 1.0f;

	}
	else
	{
		sampled_event = BSDF_DIFFUSE;

		vec2 e = vec2(sample.y, sample.z);

		vec3 wi_local = sample_cos_hemisphere(e);
		cos_theta_out = wi_local.z;
		if (cos_theta_out < 1e-6f)
			return 0.0f;
	
		// If we hit from the other side of normal invert sampled direction to remain in the same hemisphere
		if (wo_local.z < 0)
			wi_local.z = -wi_local.z;
		wi = frame.to_world(wi_local);

		if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f || wi_local.z * wo_local.z <= 0.0f)
			return 0.0f;

		forward_pdf = fabs(wi_local.z) * Inv_PI;
		reverse_pdf = fabs(wo_local.z) * Inv_PI;
		//auto pow5 = [](float v) { return (v * v) * (v * v) * v; };
		//vec3 h = normalize(wi_local + wo_local);

		//float D90 = -0.5f;

		contination_prob = std::max(0.05f, (R * (1.0 - f)).Max());

		return R; //* (1.0f + D90 * pow5(1.0f - fabs(wo_local.z))) * (1.0f + D90 * pow5(1.0f - fabs(wi_local.z)));
	}


}

vec3 Fresnel_Specular_Microfacet_GGX::evaluate(const vec3& wi, const vec3& geometric_normal) const
{
	if (dot(wi, geometric_normal) * dot(frame.to_world(wo_local), geometric_normal) <= 0.0f)
		return 0.0f;
	if (frame.to_local(wi).z * wo_local.z <= 0.0f)
		return 0.0f;
	auto pow5 = [](float v) { return (v * v) * (v * v) * v; };

	//vec3 wi_local = frame.to_local(wi);
	// Evaluate Diffuse
	// 0.3875076875280 = 28 / (23 * pi) - from http://www.pbr-book.org/3ed-2018/Reflection_Models/Fresnel_Incidence_Effects.html

	return R * Inv_PI;

}

void Fresnel_Specular_Microfacet_GGX::pdf(const vec3& wi, float& forward_pdf, float& reverse_pdf) const
{
	float f = fresnel_dielectric_reflectance(fabs(wo_local.z), s);
	if (f > 1.0f)
	{
		forward_pdf = reverse_pdf = 0.0f;
		return;
	}

	vec3 wi_local = frame.to_local(wi);

	// Diffuse pdf
	float forward_diffuse_pdf = fabs(wi_local.z) * Inv_PI;
	float reverse_diffuse_pdf = fabs(wo_local.z) * Inv_PI;

	forward_pdf = forward_diffuse_pdf * (1.0f - f);
	reverse_pdf = reverse_diffuse_pdf * (1.0f - f);
}