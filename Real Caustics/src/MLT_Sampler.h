#ifndef MLT_SAMPLER_H
#define MLT_SAMPLER_H
#include <vector>
#include "vec2.h"
#include "vec3.h"


struct Primary_Sample
{
	float value;

	void backup()
	{
		back_up = value;
	}
	void restore()
	{
		value = back_up;
	}

	float back_up;
};


class MLT_Sampler
{

public:
	MLT_Sampler(int max_path_length, float mut_size) : mutation_size(mut_size)
	{
		index = 0;
		samples.resize((max_path_length + 2) * 3 + 2);
		for (int i = 0; i < samples.size(); ++i)
		{
			float e = random_float_0_1();
			samples[i].value = e;
			samples[i].back_up = e;
		}
		is_last_uniform = true;
	}
public:
	float Get1D();
	vec2 Get2D();
	vec3 Get3D();
	void Mutate();
	void Init_Uniform();
	void Reject();
	void Start_Iteration();
	void skip(int s)
	{
		index += s;
	}

public:
	std::vector<Primary_Sample> samples;
	int index;
	float mutation_size;
	bool is_last_uniform;
	int generating_small_mutations = 0;
	int generating_big_mutations = 0;
};



#endif // !MLT_SAMPLER_H

