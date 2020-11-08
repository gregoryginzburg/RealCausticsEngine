#include "Light_list.h"
#include "Vertex_Merging.h"
#include "random_generators.h"
#include "utils.h"
#include "MLT_Sampler.h"

#include <memory>
#include <fstream>

void Lights_list::sample_lights(Light_Sub_Path_State& light_state, MLT_Sampler& Sampler) 
{
	float rng = Sampler.Get1D();
	int idx = find_interval(cdf, rng);
	float pick_light_pdf = lights[idx]->get_power() / (light_count * integral);

	float pdf;
	light_state.throughput = lights[idx]->sample_light(light_state.origin, light_state.direction, pdf, Sampler);

	light_state.throughput /= pick_light_pdf;
	/*
	if (random_float_0_1() < 0.05)
	{
		vec3 sampl = light_state.direction;
		vec3 p = light_state.origin;
		o << "v " << sampl.x << " " << sampl.y << " " << sampl.z << "\n";
		are_out<< "v " << p.x << " " << p.y << " " << p.z << "\n";
	}*/
	

}

void Lights_list::update_pdf_distribution()
{
	cdf.resize(light_count + 1);
	cdf[0] = 0.0f;
	for (int i = 1; i < light_count + 1; ++i)
	{
		cdf[i] = cdf[i - 1] + lights[i - 1]->get_power();
	}
	integral = cdf[light_count];
	for (int i = 1; i < light_count + 1; ++i)
	{
		cdf[i] /= integral;
	}
}