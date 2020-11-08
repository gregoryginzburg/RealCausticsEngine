
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include "random_generators.h"
#include "aabb.h"
#include "Color.h"
#include "materials.h"
#include "Path_Vertices.h"


extern const float inf;
extern const float E;

extern int radius_estimate_right;

extern std::ofstream test_out;



int aproximate_biggest_axis(std::vector<temp_path_vertex>& path_vertices, int start_index, int count)
{
	vec3 bottom(inf, inf, inf);
	vec3 top(-inf, -inf, -inf);
	if (count < 500)
	{
		int endindex = start_index + count;
		for (int i = start_index; i < endindex; ++i)
		{
			if (path_vertices[i].path_vertex->position.x < bottom.x)
			{
				bottom.x = path_vertices[i].path_vertex->position.x;
			}
			if (path_vertices[i].path_vertex->position.y < bottom.y)
			{
				bottom.y = path_vertices[i].path_vertex->position.y;
			}
			if (path_vertices[i].path_vertex->position.z < bottom.z)
			{
				bottom.z = path_vertices[i].path_vertex->position.z;
			}
			if (path_vertices[i].path_vertex->position.x > top.x)
			{
				top.x = path_vertices[i].path_vertex->position.x;
			}
			if (path_vertices[i].path_vertex->position.y > top.y)
			{
				top.y = path_vertices[i].path_vertex->position.y;
			}
			if (path_vertices[i].path_vertex->position.z > top.z)
			{
				top.z = path_vertices[i].path_vertex->position.z;
			}
		}
	}
	else
	{
		for (int i = 0; i < 400; ++i)
		{
			int random = random_int11(start_index, start_index + count - 1);
			if (path_vertices[random].path_vertex->position.x < bottom.x)
			{
				bottom.x = path_vertices[random].path_vertex->position.x;
			}
			if (path_vertices[random].path_vertex->position.y < bottom.y)
			{
				bottom.y = path_vertices[random].path_vertex->position.y;
			}
			if (path_vertices[random].path_vertex->position.z < bottom.z)
			{
				bottom.z = path_vertices[random].path_vertex->position.z;
			}
			if (path_vertices[random].path_vertex->position.x > top.x)
			{
				top.x = path_vertices[random].path_vertex->position.x;
			}
			if (path_vertices[random].path_vertex->position.y > top.y)
			{
				top.y = path_vertices[random].path_vertex->position.y;
			}
			if (path_vertices[random].path_vertex->position.y > top.z)
			{
				top.z = path_vertices[random].path_vertex->position.z;
			}
		}
	}
	float x = top.x - bottom.x;
	float y = top.y - bottom.y;
	float z = top.z - bottom.z;
	float biggest = std::fmaxf(std::fmaxf(x, y), z);
	return biggest == x ? 0 : biggest == y ? 1 : 2;
}

bool x_comparator(const temp_path_vertex& a, const temp_path_vertex& b)
{
	return a.path_vertex->position.x < b.path_vertex->position.x;
}
bool y_comparator(const temp_path_vertex& a, const temp_path_vertex& b)
{
	return a.path_vertex->position.y < b.path_vertex->position.y;
}
bool z_comparator(const temp_path_vertex& a, const temp_path_vertex& b)
{
	return a.path_vertex->position.z < b.path_vertex->position.z;
}

void recurse_kd_tree(std::vector<temp_path_vertex>& path_vertices, std::vector<KDTreeNode>& kdtree, int start_index, int count, int element)
{
	if (count < 2)
	{
		kdtree[element].u.leaf.index = path_vertices[start_index].index;
	}
	else
	{
		int axis = aproximate_biggest_axis(path_vertices, start_index, count);
		auto comparator = axis == 0 ? x_comparator : axis == 1 ? y_comparator : z_comparator;
		unsigned mask;
		//нечетное
		if (count & 1)
		{
			mask = axis == 0 ? 0x90000000 : axis == 1 ? 0xb0000000 : 0xf0000000;
			std::nth_element(path_vertices.begin() + start_index, path_vertices.begin() + start_index + (count / 2),
				path_vertices.begin() + start_index + count, comparator);
			if (axis == 0)
			{ 
				kdtree[element].u.inner.index = path_vertices[(long)start_index + (long)(count / 2)].index | mask;
				kdtree[element].u.inner.split = path_vertices[(long)start_index + (long)(count / 2)].path_vertex->position.x;
			}
			else if (axis == 1)
			{ 
				kdtree[element].u.inner.index = path_vertices[(long)start_index + (long)(count / 2)].index | mask;
				kdtree[element].u.inner.split = path_vertices[(long)start_index + (long)(count / 2)].path_vertex->position.y;
			}
			else
			{
				kdtree[element].u.inner.index = path_vertices[(long)start_index + (long)(count / 2)].index | mask;
				kdtree[element].u.inner.split = path_vertices[(long)start_index + (long)(count / 2)].path_vertex->position.z;
			}
			recurse_kd_tree(path_vertices, kdtree, start_index, count / 2, 2 * element);
			recurse_kd_tree(path_vertices, kdtree, start_index + count / 2 + 1, count / 2, 2 * element + 1);
				
		}
		else
		{
			std::nth_element(path_vertices.begin() + start_index, path_vertices.begin() + start_index + (count / 2),
				path_vertices.begin() + start_index + count, comparator);
			mask = axis == 0 ? 0x80000000 : axis == 1 ? 0xa0000000 : 0xe0000000;
			if (axis == 0)
			{
				kdtree[element].u.inner.index = mask;
				kdtree[element].u.inner.split = path_vertices[(long)start_index + (long)(count / 2)].path_vertex->position.x + 1.175494e-38f;

			}
			else if (axis == 1)
			{
				kdtree[element].u.inner.index = mask;
				kdtree[element].u.inner.split = path_vertices[(long)start_index + (long)(count / 2)].path_vertex->position.y + 1.175494e-38f;
			}
			else
			{
				kdtree[element].u.inner.index = mask;
				kdtree[element].u.inner.split = path_vertices[(long)start_index + (long)(count / 2)].path_vertex->position.z + 1.175494e-38f;
			}
			recurse_kd_tree(path_vertices, kdtree, start_index, count / 2, 2 * element);
			recurse_kd_tree(path_vertices, kdtree, start_index + count / 2, count / 2, 2 * element + 1);
		}
		
	}

}




void KD_Tree::build_kd_tree(std::vector<Path_Vertex>& path_vertices)
{
	std::vector<temp_path_vertex> temp_path_vertices;
	temp_path_vertices.reserve(path_vertices.size());
	for (int i = 0; i < path_vertices.size(); ++i)
	{
		temp_path_vertices.emplace_back(&path_vertices[i], i);
	}
	kdtree.resize(std::pow(2, (int)(std::log2f(path_vertices.size()) + 1)));

	recurse_kd_tree(temp_path_vertices, kdtree, 0, path_vertices.size(), 1);

}

/*
void update_kd_tree(bool was_changed, std::vector<std::shared_ptr<photon>>& photons, std::vector<KDTreeNode>& kdtree, const char* file_path)
{
	if (was_changed)
	{
		create_kd_tree(photons, kdtree);

		std::ofstream file(file_path, std::ios::binary);
		int size = kdtree.size();
		file.write((char*)&size, sizeof(int));
		file.write((char*)&kdtree[0], sizeof(KDTreeNode) * (size_t)size);
		file.close();
	}
	else
	{
		std::ifstream file(file_path, std::ios::binary);
		int size;
		file.read((char*)&size, sizeof(int));
		kdtree.resize(size);
		file.read((char*)&kdtree[0], sizeof(KDTreeNode) * (size_t)size);
	}
}
*/


void KD_Tree::find_closest_in_radius(
	const vec3& point, 
	float& search_d, 
	std::vector<Path_Vertex>& path_vertices, 
	std::vector<Path_Vertex>& closest_path_vertices,
	int element)
{
	if (kdtree[element].u.inner.index & 0x80000000)
	{
		float dist_to_plane;
		if ((kdtree[element].u.inner.index & 0x60000000) == 0)
			dist_to_plane = point.x - kdtree[element].u.inner.split;
		else if ((kdtree[element].u.inner.index & 0x60000000) == 0x20000000)
			dist_to_plane = point.y - kdtree[element].u.inner.split;
		else
			dist_to_plane = point.z - kdtree[element].u.inner.split;
		if (dist_to_plane < 0)
		{
			find_closest_in_radius(point, search_d, path_vertices, closest_path_vertices, 2 * element);
			if (dist_to_plane * dist_to_plane < search_d)
			{
				if (kdtree[element].u.inner.index & 0x10000000)
				{
					int index = kdtree[element].u.inner.index & 0xfffffff;
					float dist_to_point_squared = (point - path_vertices[index].position).length_squared();
					if (dist_to_point_squared < search_d)
					{
						closest_path_vertices.push_back(path_vertices[index]);
					}
						
				}
				find_closest_in_radius(point, search_d, path_vertices, closest_path_vertices, 2 * element + 1);
			}

		}
		else
		{
			find_closest_in_radius(point, search_d, path_vertices, closest_path_vertices, 2 * element + 1);
			if (dist_to_plane * dist_to_plane < search_d)
			{
				if (kdtree[element].u.inner.index & 0x10000000)
				{
					int index = kdtree[element].u.inner.index & 0xfffffff;
					float dist_to_point_squared = (point - path_vertices[index].position).length_squared();
					if (dist_to_point_squared < search_d)
					{
						closest_path_vertices.push_back(path_vertices[index]);
					}
						
				}
				find_closest_in_radius(point, search_d, path_vertices, closest_path_vertices, 2 * element);
			}

		}
	}
	else
	{
		float dist_to_point_squared = (point - path_vertices[kdtree[element].u.leaf.index].position).length_squared();
		if (dist_to_point_squared < search_d)
		{
			closest_path_vertices.push_back(path_vertices[kdtree[element].u.leaf.index]);
		}
	}
}

