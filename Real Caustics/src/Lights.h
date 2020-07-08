#ifndef LIGHTS_H
#define LIGHTS_H
#include <vector>
#include <memory>
#include "vec3.h"
#include "ray.h"
#include "utils.h"
#include "random_generators.h"
#include "Color.h"

extern const float negative_inf;
extern int number_of_photons;

class Light
{
public:
	virtual ray emit_photon(size_t i, float power) const = 0;
	virtual float get_power() const = 0;

};
class Area_Light : public Light
{
public:
	vec3 position;
	vec3 bottom_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 normal = vec3(0, 0, -1);
	float spread;
	float power;

public:
	Area_Light(vec3 p, float w, float h, float spr, float pow)  : spread(spr), position(p), power(pow)
	{
		bottom_left_corner = position - vec3(w / 2.f, h / 2.f, 0.f);
		horizontal = vec3(w, 0., 0.);
		vertical = vec3(0., h, 0.);	
	}

	Area_Light(vec3 p, vec3 rot, float w, float h, float spr, float pow) : Area_Light(p, w, h, spr, pow)
	{
		rotate(rot);
	}
	
public:
	virtual ray emit_photon(size_t i, float power) const
	{
		return ray(bottom_left_corner + r2(i).x * horizontal + r2(i).y * vertical, normal + spread*random_in_hemisphere(), colorf(power, power, power));
	}
	virtual float get_power() const
	{
		return power;
	}
	//rotation in degrees
	void rotate(vec3 rotation)
	{
		vec3 top_left_corner = bottom_left_corner + vertical;
		vec3 bottom_rigth_corner = bottom_left_corner + horizontal;
		rotate_vec(bottom_left_corner, position, rotation);
		rotate_vec(top_left_corner, position, rotation);
		rotate_vec(bottom_rigth_corner, position, rotation);
		rotate_vec(normal, position, rotation);
		horizontal = bottom_rigth_corner - bottom_left_corner;
		vertical = top_left_corner - bottom_left_corner;
	}
};

class Lights_list 
{
public:
	std::vector<std::shared_ptr<Light>> lights;
	std::vector<float> weights;
public:
	Lights_list() {}

	Lights_list(std::shared_ptr<Light> l)
	{
		add(l);
	}

	void add(std::shared_ptr<Light> l)
	{
		lights.push_back(l);
	}
public:
	ray emit_photon() const
	{
		static int light = 0;
		static int numbers_photons_light = (int)(weights[light] * number_of_photons);
		static float current_power = lights[light]->get_power() / (float)(numbers_photons_light - 1);
		static int ii = -1;
		if (ii > numbers_photons_light - 2)
		{
			light += 1;
			if (light > lights.size() - 1)
			{
				return ray(vec3(0, 0, 0), vec3(0, 0, 0));
			}
			numbers_photons_light = static_cast<int>(weights[light] * number_of_photons);
			current_power = lights[light]->get_power() / (float)(numbers_photons_light - 1);
			ii = 0;
			return lights[light]->emit_photon(ii, current_power);
		}

		ii += 1;
		return lights[light]->emit_photon(ii, current_power);
	}
	void calculate_weights()
	{
		weights.reserve(lights.size());
		float sum_power = 0.f;
		for (int i = 0; i < lights.size(); ++i)
		{
			if (lights[i]->get_power() > negative_inf)
			{
				sum_power += lights[i]->get_power();
			}
			weights.push_back(lights[i]->get_power());
		}
		for (int i = 0; i < weights.size(); ++i)
		{
			weights[i] /= sum_power;
		}
	}

};


#endif
