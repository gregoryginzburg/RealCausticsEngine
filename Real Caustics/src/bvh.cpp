#include "vec3.h"
#include "ray.h"
#include "mesh.h"

void make_list_for_bvh(Mesh& mesh, std::vector<aabb>& working_list)
{
	working_list.reserve(mesh.number_of_tris());
	for (size_t i = 0; i < mesh.number_of_tris(); ++i)
	{
		aabb temp;
		mesh.triangles[i]->bounding_box(temp);
		temp.triangle = mesh.triangles[i];
		temp.center = (temp.max + temp.min) * 0.5;
		working_list.push_back(temp);
	}
	mesh.triangles.clear();
	mesh.triangles.shrink_to_fit();
}

