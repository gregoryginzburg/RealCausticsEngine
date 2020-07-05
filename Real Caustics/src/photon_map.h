#ifndef PHOTON_MAP_H
#define PHOTON_MAP_H
#include <vector>
#include <memory>
#include "photon.h"

class Photon_map
{

public:
	std::vector<std::shared_ptr<photon>> photons;
};




#endif
