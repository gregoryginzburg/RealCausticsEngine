#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>
#include <sstream>
#include <random>
#include <cmath>
#include "Color.h"
#include "matrix.h"




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
inline int maprange(double value, double low1, double high1, double low2, double high2)
{
	return static_cast<int>(low2 + ((value - low1) * (high2 - low2)) / (high1 - low1));
}
inline std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}
inline double halton_sequnce(int n, int base)
{
	double q = 0, bk = (double)1 / base;

	while (n > 0) {
		q += (n % base) * bk;
		n /= base;
		bk /= base;
	}

	return q;
}
inline size_t Ruler(size_t n)
{
    size_t ret = 0;
    while (n != 0 && (n & 1) == 0)
    {
        n /= 2;
        ++ret;
    }
    return ret;
}
inline void TestSobol2D(std::vector<vec2>& points)
{
    // calculate the sample points

    // x axis
    //std::array<std::array<float, 2>, NUM_SAMPLES> samples;
    size_t sampleInt = 0;
    for (size_t i = 0; i < points.size(); ++i)
    {
        size_t ruler = Ruler(i + 1);
        size_t direction = size_t(size_t(1) << size_t(31 - ruler));
        sampleInt = sampleInt ^ direction;
        points[i].x = float(sampleInt) / std::pow(2.0f, 32.0f);
    }

    // y axis
    // Code adapted from http://web.maths.unsw.edu.au/~fkuo/sobol/
    // uses numbers: new-joe-kuo-6.21201

    // Direction numbers
    std::vector<size_t> V;
    V.resize((size_t)std::ceil(std::log((double)points.size()) / std::log(2.0)));
    V[0] = size_t(1) << size_t(31);
    for (size_t i = 1; i < V.size(); ++i)
        V[i] = V[i - 1] ^ (V[i - 1] >> 1);

    // Samples
    sampleInt = 0;
    for (size_t i = 0; i < points.size(); ++i)
    {
        size_t ruler = Ruler(i + 1);
        sampleInt = sampleInt ^ V[ruler];
        points[i].y = float(sampleInt) / std::pow(2.0f, 32.0f);
    }

}
inline double clamp(double n, double min, double max)
{
    if (n < min)
    {
        return min;
    }
    if (n > max)
    {
        return max;
    }
    return n;
}
//camera_matrix - obj.matrix_world.normalized().inverted()
inline bool visible_in_frustum(const matrix_4x4& camera_matrix, std::vector<vec3>& frame, bool is_perspective, const vec3& point, double treshold)
{
    vec3 co_local = camera_matrix * point;
    double z = -co_local.z;
    if (is_perspective)
    {
        if (z == 0.)
        {
            return true;
        }
        else
        {
            for (int i = 0; i < 3; ++i)
            {
                frame[i] = -(frame[i] / (frame[i].z / z));
            }
        }
    }
    double min_x = frame[2].x;
    double max_x = frame[1].x;
    double min_y = frame[1].y;
    double max_y = frame[0].y;
    double x = (co_local.x - min_x) / (max_x - min_x);
    double y = (co_local.y - min_y) / (max_y - min_y);
    if (x < 0. + treshold or x > 1. - treshold or y < 0. + treshold or y > 1. - treshold)
    {
        return false;
    }
    return true;
}



#endif
