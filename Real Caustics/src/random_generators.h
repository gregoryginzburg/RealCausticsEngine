#ifndef RANDOM_GENERATORS_H
#define RANDOM_GENERATORS_H
#include <random>
/*inline float random_float(float min, float max)
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mt);
}*/
/*inline int random_int(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(min, max);
	return dist(mt);
}*/
inline unsigned int random_int(uint32_t range)
{
	static std::random_device rd;
	static std::mt19937 rng(rd());
	uint32_t x, r;
	do
	{
		x = rng();
		r = x % range;
	} while (x - r > (~range + 1));

	return r;
}
inline float random_float_0_1()
{
	return random_int(10000000) / 9999999.f;
}
inline float random_float(float a, float b)
{
	return a + (b - a) * random_float_0_1();
}


inline float r1(int n)
{
	static double g1 = 1.6180339887498948482;
	double a1 = 1. / g1;
	float answer = (float)(0.5 + a1 * n);
	return answer - (int)answer;
}

inline float r2(int n)
{
	static double g2 = 1.32471795724474602596;
	double a1 = 1. / g2;
	double a2 = 1. / (g2 * g2);
	float answer = (float)(0.5 + a1 * n);
	return answer - (int)answer;
}
#endif