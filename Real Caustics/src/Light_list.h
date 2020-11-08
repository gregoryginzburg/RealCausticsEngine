#ifndef LIGHTS_LIST_H
#define LIGHTS_LIST_H

#include <vector>
#include <memory>
#include <fstream>
#include "Lights.h"

class ray;
class Light;
class Light_Sub_Path_State;

class Lights_list
{
public:
	Lights_list() 
	{
		o.open("D:\\Light_Samples.obj");
		are_out.open("D:\\Area_Samples.obj");
	}

	void sample_lights(Light_Sub_Path_State& path, MLT_Sampler& Sampler);

	void add(std::shared_ptr<Light> l)
	{
		lights.push_back(l);
	}

public:
	void update_pdf_distribution();

public:
	std::vector<std::shared_ptr<Light>> lights;
	std::vector<float> cdf;
	int light_count;

	float integral;

	std::ofstream o;
	std::ofstream are_out;
	//std::ofstream uniform_o;
};





#endif
