#ifndef UV_MAP_H
#define UV_MAP_H
#include <vector>
#include <memory>
#include "../vec3.h"
#include "2D_triangle.h"
#include "2D_BVH.h"
#include "../vec2.h"
#include "../mesh.h"
#include "../Triangle.h"

class UV_Map
{
public:
	std::vector<std::shared_ptr<triangle2>> triangles;
	std::shared_ptr<Mesh> mesh;
	BVHNode2* root = nullptr;
public:
	void make_uv_map()
	{
		triangles.reserve(mesh->triangles.size());
		for (int i = 0; i < mesh->triangles.size(); ++i)
		{

			triangles.emplace_back(std::make_shared<triangle2>(mesh->triangles[i]->texture_vert0, mesh->triangles[i]->texture_vert1, mesh->triangles[i]->texture_vert2));
		}
	}

	void build_bvh()
	{
		root = build_bvh_2(*this);
		triangles.clear();
		triangles.shrink_to_fit();
	}
	void get_u_v(vec2 point, hit_rec_2& rec)
	{
		hit(root, point, rec);
	}
	vec3 get_point_from_uv(vec2 point)
	{
		hit_rec_2 rec;
		get_u_v(point, rec);
		return mesh->triangles[rec.index]->vertice0 * (1.f - rec.v - rec.u) + mesh->triangles[rec.index]->vertice1 * rec.u +
			mesh->triangles[rec.index]->vertice2 * rec.v;
	}
};





#endif
