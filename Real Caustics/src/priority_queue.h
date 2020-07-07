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
	int size;
public:
	Priority_queue(int s) : size(s) 
	{
		photons.resize(s);
		priorities.resize(s);
	}
public:
	void insert_element(std::shared_ptr<photon> insertion_photon, float priority)
	{
		if (capacity == size)
		{
			if (priority < priorities[0])
			{
				photons[0] = insertion_photon;
				priorities[0] = priority;
				heapify(1);
			}
		}
		else
		{
			++capacity;
			photons[capacity - 1] = insertion_photon;
			priorities[capacity - 1] = priority;
			increase_key(capacity - 1);
		}
			
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
	void heapify(int i)
	{
		int left = 2 * i;
		int right = 2 * i + 1;
		int largest;
		if (left < (size + 1) && priorities[left - 1] > priorities[i - 1])
			largest = left;
		else
			largest = i;
		if (right < (size + 1) && priorities[right - 1] > priorities[i - 1])
			largest = right;
		if (largest != i)
		{
			std::swap(photons[i - 1], photons[largest - 1]);
			std::swap(priorities[i - 1], priorities[largest - 1]);
			heapify(largest);
		}
	}
};







#endif
