#ifndef CAMERA_H
#define CAMERA_H
#include "matrix.h"
#include "vec3.h"
#include <cmath>
class Camera
{
public:
	int pixel_width;
	int pixel_height;

private:
	vec3 origin;
	vec3 bottom_left_corner;
	vec3 horizontal;
	vec3 vertical;

public:
	Camera() {}
	
	Camera(int pixel_x, int pixel_y, vec3 o, vec3 top_right, vec3 bottom_right, vec3 bottom_left, vec3 top_left) : 
		bottom_left_corner(bottom_left), pixel_width(pixel_x), pixel_height(pixel_y), origin(o)
	{
		horizontal = bottom_right - bottom_left;
		vertical = top_left - bottom_left;
	}

public:
	ray get_ray(float i, float j) const
	{
		vec3 dir = bottom_left_corner + (i / (float)pixel_width) * horizontal + (j / (float)pixel_height) * vertical;
		dir -= origin;
		return ray(origin, normalize(dir));
	}
	vec3 get_origin() const
	{
		return origin;
	}
};

#endif
