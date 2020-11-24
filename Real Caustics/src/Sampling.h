#ifndef SAMPLING_H
#define SAMPLING_H

#include "vec3.h"
#include "vec2.h"
#include "random_generators.h"
#include <cmath>

extern const float PI;
extern const float Inv_PI;
extern const float PI2;
extern const float PI_OVER_2;
extern const float PI_OVER_4;

inline vec2 sample_uniform_disk(const vec2& e)
{
	vec2 u_offset = 2.0f * e - 1.0f;
	if (u_offset.x == 0.0f && u_offset.y == 0.0f)
	{
		return vec2(0.0f, 0.0f);
	}
	float theta, r;
	if (std::abs(u_offset.x) > std::abs(u_offset.y))
	{
		r = u_offset.x;
		theta = PI_OVER_4 * (u_offset.y / u_offset.x);
	}
	else
	{
		r = u_offset.y;
		theta = PI_OVER_2 - PI_OVER_4 * (u_offset.x / u_offset.y);
	}
	return r * vec2(std::cos(theta), std::sin(theta));

	/*float r = std::sqrt(e.x);
	float theta = PI2 * e.y;
	return vec2(r * std::cos(theta), r * std::sin(theta));*/
}

inline vec3 sample_cos_hemisphere(const vec2& e)
{
	vec2 d = sample_uniform_disk(e);
	float z = std::sqrt(std::max(0.0f, 1.0f - d.x * d.x - d.y * d.y));
	// z = cos(theta)
	return vec3(d.x, d.y, z);
}




#endif
