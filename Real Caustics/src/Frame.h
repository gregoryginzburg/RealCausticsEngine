#ifndef FRAME_H
#define FRAME_H
#include <cmath>
#include "vec3.h"

class Frame
{
public:
	Frame() : mX(0.0f), mY(0.0f), mZ(0.0f) {}
public:

	void set_from_z(const vec3& z)
	{
		vec3 tmpZ = mZ = z;
		vec3 tmpX = (std::abs(tmpZ.x) > 0.99f) ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);
		mY = normalize(cross(tmpZ, tmpX));
		mX = cross(mY, tmpZ);
	}
	
	vec3 to_world(const vec3& a) const
	{
		return mX * a.x + mY * a.y + mZ * a.z;
	}

	vec3 to_local(const vec3& a) const
	{
		return vec3(dot(a, mX), dot(a, mY), dot(a, mZ));
	}
public:
	vec3 mX;
	vec3 mY;
	vec3 mZ;
};



#endif // !FRAME_H

