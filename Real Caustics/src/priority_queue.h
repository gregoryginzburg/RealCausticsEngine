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
	int number_of_photons = 0;
	int size = 2147483647;

public:
	Priority_queue(int s) 
	{
		photons.reserve(s + 1);
		priorities.reserve(s + 1);
		size = s;
		photons.resize(1);
		priorities.resize(1);
	}
	Priority_queue()
	{

	}
public:
	void insert_element(std::shared_ptr<photon> insertion_photon, float priority)
	{
		if (number_of_photons == size)
		{
			if (priority < priorities[1])
			{
				photons[1] = insertion_photon;
				priorities[1] = priority;
				heapify(1);
			}
		}
		else
		{
			++number_of_photons;
			photons.push_back(insertion_photon);
			priorities.push_back(priority);
			increase_key(number_of_photons);
		}
			
			
	}
	void increase_key(int i)
	{
		while (i > 1 && priorities[i / 2] < priorities[i])
		{
			std::swap(priorities[i / 2], priorities[i]);
			std::swap(photons[i / 2], photons[i]);
			i /= 2;
		}
	}
	void heapify(int i)
	{
		int left = 2 * i;
		int right = 2 * i + 1;
		int largest;
		if (left < (size + 1) && priorities[left] > priorities[i])
		{
			largest = left;
		}	
		else
		{
			largest = i;
		}
		if (right < (size + 1) && priorities[right] > priorities[largest])
		{
			largest = right;
		}
			
		if (largest != i)
		{
			std::swap(photons[i], photons[largest]);
			std::swap(priorities[i], priorities[largest]);
			heapify(largest);
		}
	}
	bool is_full()
	{
		return number_of_photons == size;
	}
};







#endif
