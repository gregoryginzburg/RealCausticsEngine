#ifndef LIGHTS_LIST_H
#define LIGHTS_LIST_H

#include <vector>
#include <memory>

class ray;
class Light;
struct helper_light_emit;
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
		std::cout << "Adding Light" << std::endl;
		lights.push_back(l);
	}

public:
	ray emit_photon(helper_light_emit& helper) const;

	void calculate_weights();

};





#endif
