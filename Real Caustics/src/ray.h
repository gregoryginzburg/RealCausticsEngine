#ifndef RAY_H
#define RAY_H
#include "vec3.h"

class ray 
{
public:
    ray() {}
    ray(const point3& orig, const vec3& dir)
        : origin(orig), direction(dir) {}
       
    inline point3 at(float t)
    {
        return origin + direction * t;
    }
public:
    point3 origin;
    vec3 direction;
    bool was_refracted = false;

};

inline bool operator==(const ray& r1, const ray& r2)
{
    return r1.origin == r2.origin && r1.direction == r2.direction;
}

#endif

