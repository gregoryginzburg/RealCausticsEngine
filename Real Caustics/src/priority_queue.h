#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include <vector>
#include <algorithm>
#include "photon.h"
//max heap
class Priority_queue
{
public:
	std::vector<std::shared_ptr<photon>> photons;
	std::vector<float> priorities;
	//current size
	int capacity = 0;
	//max size
public:
	Priority_queue() {}
public:
	void insert_element(std::shared_ptr<photon> insertion_photon, float priority)
	{
		++capacity;
		photons.push_back(insertion_photon);
		priorities.push_back(priority);
		increase_key(capacity - 1);		
	}
	void increase_key(int i)
	{
		while (i > 0 && priorities[i / 2] < priorities[i])
		{
			std::swap(priorities[i / 2], priorities[i]);
			std::swap(photons[i / 2], photons[i]);
			i /= 2;
		}
		

	}

};







#endif
