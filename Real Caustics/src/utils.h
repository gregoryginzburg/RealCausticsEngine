#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>
#include <sstream>
#include <random>
#include <cmath>
#include <algorithm>
#include "Color.h"
#include "vec3.h"
#include "matrix.h"
#include "photon.h"
#include <memory>

inline void clamp_color(color& c)
{
    if (c.r > 255)
        c.r = 255;
    if (c.g > 255)
        c.g = 255;
    if (c.b > 255)
        c.b = 255;
}
inline int maprange(float value, float low1, float high1, float low2, float high2)
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
inline float halton_sequnce(int n, int base)
{
	float q = 0, bk = (float)1 / base;

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
    V.resize((size_t)std::ceil(std::log((float)points.size()) / std::log(2.0)));
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
inline float clamp(float n, float min, float max)
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
inline bool visible_in_frustum(const matrix_4x4& camera_matrix, std::vector<vec3>& frame, bool is_perspective, const vec3& point, float treshold)
{
    vec3 co_local = camera_matrix * point;
    float z = -co_local.z;
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
    float min_x = frame[2].x;
    float max_x = frame[1].x;
    float min_y = frame[1].y;
    float max_y = frame[0].y;
    float x = (co_local.x - min_x) / (max_x - min_x);
    float y = (co_local.y - min_y) / (max_y - min_y);
    if (x < 0. + treshold or x > 1. - treshold or y < 0. + treshold or y > 1. - treshold)
    {
        return false;
    }
    return true;
}



#endif
