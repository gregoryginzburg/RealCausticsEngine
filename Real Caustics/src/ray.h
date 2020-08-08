#ifndef RAY_H
#define RAY_H
#include "vec3.h"
#include "Color.h"

class ray 
{
public:
    ray() {}

    ray(const point3& orig, const vec3& dir) : origin(orig), direction(dir) {}

    ray(const point3& orig, const vec3& dir, const colorf& pow) : origin(orig), direction(dir), power(pow) {}
       
    inline point3 at(float t)
    {
        return origin + direction * t;
    }
    inline ray& operator*=(const colorf& other)
    {
        power *= other;
        return *this;
    }
public:
    point3 origin;
    vec3 direction;
    colorf power;

};

inline bool operator==(const ray& r1, const ray& r2)
{
    return r1.origin == r2.origin && r1.direction == r2.direction;
}
inline ray operator*(const colorf& t, const ray& r)
{
    return ray(r.origin, r.direction, r.power * t);
}
inline ray operator*(const ray& r, const colorf& t)
{
    return t * r;
}

#endif

