#include "MLT_Sampler.h"
#include "random_generators.h"
#include <cmath>
extern const float E;
extern const float INV_E;

float MLT_Sampler::Get1D() 
{
	return samples[index++].value;
}
vec2 MLT_Sampler::Get2D()  
{
	return vec2(Get1D(), Get1D());
}
vec3 MLT_Sampler::Get3D() 
{
	return vec3(Get1D(), Get1D(), Get1D());
}
void MLT_Sampler::Mutate()
{
	for (int i = 0; i < samples.size(); ++i)
	{
		Primary_Sample& X = samples[i];

		X.backup();
		float delta_u;
		float e = random_float_0_1();
		/*
		if (random_float_0_1() < 0.5f)
			delta_u = std::powf(e, 1.0f / mutation_size + 1.0f);
		else
			delta_u = -std::powf(e, 1.0f / mutation_size + 1.0f);*/
		if (random_float_0_1() < 0.5f)
			delta_u = e * std::powf(INV_E, 1.0f / mutation_size);
		else
			delta_u = -e * std::powf(INV_E, 1.0f / mutation_size);

		X.value += delta_u;
		X.value -= std::floor(X.value);
	}
	is_last_uniform = false;
}
void MLT_Sampler::Init_Uniform()
{
	for (int i = 0; i < samples.size(); ++i)
	{
		samples[i].backup();
		samples[i].value = random_float_0_1();
	}
	is_last_uniform = true;

}
void MLT_Sampler::Reject()
{
	for (int i = 0; i < samples.size(); ++i)
	{
		samples[i].restore();
	}
}

void MLT_Sampler::Start_Iteration()
{
	index = 0;
}