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

class Mesh 
{
public:
	Mesh() {}
	Mesh(std::vector<std::shared_ptr<Triangle>> tris) : triangles(tris) {}

public:
	std::vector<std::shared_ptr<Triangle>> triangles;
	aabb bounding_box;
	BVHNode_mesh* root = nullptr;
};

#endif
