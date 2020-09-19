#include "materials.h"
#include "vec3.h"
#include <math.h>

extern const float PI;

class colorf;
inline float G(float v, float a)
{
	return v / (a - a * v + v);
}

inline float clamp_max(float value, float max)
{
	if (value > max)
		return max;
	return value;
}
Catcher::Catcher(const colorf& color, float r, float s) : catcher_color(color), roughness(r), specular(s)
{

}
// light dir - photon dir
colorf Catcher::brdf(const vec3& view_dir, const vec3& photon_dir, const vec3& normal)
{
	/*
	float a = 0.8;
	vec3 h = normalize(view_dir + photon_dir);
	float t = dot(normal, h);
	float v = dot(normal, view_dir);
	float v_1 = dot(normal, photon_dir);
	float specular = (1.0f - G(v, a) * G(v_1, a)) / PI + ((G(v, a) * G(v_1, a)) / (4.0f * PI * v * v_1)) *
		(a / std::pow((1.0f + a * t * t - t * t), 2));
	return colorf(specular);// +colorf(1, 1, 1) / PI;*/
	
	vec3 h = normalize(photon_dir + view_dir);
	float NdotH = dot(normal, h);
	float NdotV = dot(normal, view_dir);
	float NdotL = dot(normal, photon_dir);
	float VdotH = dot(view_dir, h);
	
	// Normal distribution - D()
	float a2 = roughness * roughness;
	float d = ((NdotH * a2 - NdotH) * NdotH + 1.0f);
	float D = a2 / (d * d * PI);

	// Geometric Shadowing - G()
	float g1_v = 2 * NdotV / (NdotV + std::sqrt(a2 + NdotV * NdotV - a2 * NdotV * NdotV));
	float g1_l = 2 * NdotL / (NdotL + std::sqrt(a2 + NdotL * NdotL - a2 * NdotL * NdotL));
	//float G = g1_v * g1_l
	
	// Fresnel - F()

	float fresnel = specular + (1 - specular) * (1 - VdotH * VdotH * VdotH * VdotH * VdotH);
	
	float brdf = ((D * g1_l * g1_v * fresnel) / (4.0f * NdotL * NdotV)) * specular;
	//clamp_max(brdf, 0.08f)
	return catcher_color/ PI;

	/*
	float cos_i = photon_dir.z;
	float cos_o = view_dir.z;
	float a2 = roughness * roughness;

	float denom_i = std::sqrt(a2 + (1.0 - a2) * cos_i * cos_i);
	float denom_o = std::sqrt(a2 + (1.0 - a2) * cos_o * cos_o);

	float G = (cos_i * (denom_o + cos_o)) / (cos_o * denom_i + cos_i * denom_o); 
	return G + catcher_color;*/
	

}