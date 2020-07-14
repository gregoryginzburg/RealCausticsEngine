#ifndef MESH_H
#define MESH_H

#include "vec3.h"
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include "Hit_rec.h"
#include "vec2.h"
#include "materials.h"
#include "aabb.h"
#include "Triangle.h"

struct BVHNode_mesh;
extern const float inf;
class Mesh 
{
public:
	Mesh() {}
	Mesh(std::vector<Triangle> tris) : triangles(tris) {}

public:
	std::vector<Triangle> triangles;
	aabb bounding_box;
	BVHNode_mesh* root = nullptr;
	aabb create_bvh()
	{
		aabb bbox(vec3(inf, inf, inf), vec3(-inf, -inf, -inf));
		for (size_t i = 0; i < triangles.size(); ++i)
		{
			bbox = surrounding_box(bbox, triangles[i]->bounding_box());
		}
		return bbox;
	}
};

#endif
