#ifndef RANDOM_GENERATORS_H
#define RANDOM_GENERATORS_H
#include <random>
inline double random_double(double min, double max)
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(min, max);
	return dist(mt);
}
inline int random_int(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(min, max);
	return dist(mt);
}

#endif