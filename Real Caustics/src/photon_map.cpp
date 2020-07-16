#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <fstream>
#include "photon_map.h"
#include "photon.h"
#include "random_generators.h"
#include "aabb.h"

//initialize with element = 1

extern const float inf;
int aproximate_biggest_axis(std::vector<temp_photon>& photons, int start_index, int count)
{
	vec3 bottom(inf, inf, inf);
	vec3 top(-inf, -inf, -inf);
	if (count < 500)
	{
		int endindex = start_index + count;
		for (int i = start_index; i < endindex; ++i)
		{
			if (photons[i].Photon->position.x < bottom.x)
			{
				bottom.x = photons[i].Photon->position.x;
			}
			if (photons[i].Photon->position.y < bottom.y)
			{
				bottom.y = photons[i].Photon->position.y;
			}
			if (photons[i].Photon->position.z < bottom.z)
			{
				bottom.z = photons[i].Photon->position.z;
			}
			if (photons[i].Photon->position.x > top.x)
			{
				top.x = photons[i].Photon->position.x;
			}
			if (photons[i].Photon->position.y > top.y)
			{
				top.y = photons[i].Photon->position.y;
			}
			if (photons[i].Photon->position.z > top.z)
			{
				top.z = photons[i].Photon->position.z;
			}
		}
	}
	else
	{
		for (int i = 0; i < 400; ++i)
		{
			int random = random_int11(start_index, start_index + count - 1);
			if (photons[random].Photon->position.x < bottom.x)
			{
				bottom.x = photons[random].Photon->position.x;
			}
			if (photons[random].Photon->position.y < bottom.y)
			{
				bottom.y = photons[random].Photon->position.y;
			}
			if (photons[random].Photon->position.z < bottom.z)
			{
				bottom.z = photons[random].Photon->position.z;
			}
			if (photons[random].Photon->position.x > top.x)
			{
				top.x = photons[random].Photon->position.x;
			}
			if (photons[random].Photon->position.y > top.y)
			{
				top.y = photons[random].Photon->position.y;
			}
			if (photons[random].Photon->position.z > top.z)
			{
				top.z = photons[random].Photon->position.z;
			}
		}
	}
	float x = top.x - bottom.x;
	float y = top.y - bottom.y;
	float z = top.z - bottom.z;
	float biggest = std::fmaxf(std::fmaxf(x, y), z);
	return biggest == x ? 0 : biggest == y ? 1 : 2;
}

bool x_comparator(const temp_photon& a, const temp_photon& b)
{
	return a.Photon->position.x < b.Photon->position.x;
}
bool y_comparator(const temp_photon& a, const temp_photon& b)
{
	return a.Photon->position.y < b.Photon->position.y;
}
bool z_comparator(const temp_photon& a, const temp_photon& b)
{
	return a.Photon->position.z < b.Photon->position.z;
}

void recurse_kd_tree(std::vector<temp_photon>& photons, std::vector<KDTreeNode>& kdtree, int start_index, int count, int element)
{
	if (count < 2)
	{
		kdtree[element].u.leaf.index = photons[start_index].index;
	}
	else
	{
		int axis = aproximate_biggest_axis(photons, start_index, count);
		auto comparator = axis == 0 ? x_comparator : axis == 1 ? y_comparator : z_comparator;
		unsigned mask;
		//нечетное
		if (count & 1)
		{
			mask = axis == 0 ? 0x90000000 : axis == 1 ? 0xb0000000 : 0xf0000000;
			std::nth_element(photons.begin() + start_index, photons.begin() + start_index + (count / 2),
				photons.begin() + start_index + count, comparator);
			if (axis == 0)
			{ 
				kdtree[element].u.inner.index = photons[start_index + (count / 2)].index | mask;
				kdtree[element].u.inner.split = photons[start_index + (count / 2)].Photon->position.x;
			}
			else if (axis == 1)
			{ 
				kdtree[element].u.inner.index = photons[start_index + (count / 2)].index | mask;
				kdtree[element].u.inner.split = photons[start_index + (count / 2)].Photon->position.y;
			}
			else
			{
				kdtree[element].u.inner.index = photons[start_index + (count / 2)].index | mask;
				kdtree[element].u.inner.split = photons[start_index + (count / 2)].Photon->position.z;
			}
			recurse_kd_tree(photons, kdtree, start_index, count / 2, 2 * element);
			recurse_kd_tree(photons, kdtree, start_index + count / 2 + 1, count / 2, 2 * element + 1);
				
		}
		else
		{
			std::nth_element(photons.begin() + start_index, photons.begin() + start_index + (count / 2),
				photons.begin() + start_index + count, comparator);
			mask = axis == 0 ? 0x80000000 : axis == 1 ? 0xa0000000 : 0xe0000000;
			if (axis == 0)
			{
				kdtree[element].u.inner.index = mask;
				kdtree[element].u.inner.split = photons[start_index + (count / 2)].Photon->position.x + 1.175494e-38f;

			}
			else if (axis == 1)
			{
				kdtree[element].u.inner.index = mask;
				kdtree[element].u.inner.split = photons[start_index + (count / 2)].Photon->position.y + 1.175494e-38f;
			}
			else
			{
				kdtree[element].u.inner.index = mask;
				kdtree[element].u.inner.split = photons[start_index + (count / 2)].Photon->position.z + 1.175494e-38f;
			}
			recurse_kd_tree(photons, kdtree, start_index, count / 2, 2 * element);
			recurse_kd_tree(photons, kdtree, start_index + count / 2, count / 2, 2 * element + 1);
		}
		
	}


}




void create_kd_tree(std::vector<std::shared_ptr<photon>>& photons, std::vector<KDTreeNode>& kdtree)
{
	std::vector<temp_photon> temp_photons;
	temp_photons.reserve(photons.size());
	for (int i = 0; i < photons.size(); ++i)
	{
		temp_photons.emplace_back(photons[i], i);
	}
	kdtree.resize(std::pow(2, (int)(std::log2f(photons.size()) + 1)));
	recurse_kd_tree(temp_photons, kdtree, 0, photons.size(), 1);	
}
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


void find_photons(std::vector<KDTreeNode>& kdtree, std::vector<std::shared_ptr<photon>>& photons, const vec3& point, float search_d, Priority_queue& closest_photons, int element)
{
	static float search_distance_squared = search_d;
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
			find_photons(kdtree, photons, point, search_distance_squared, closest_photons, 2 * element);
			if (dist_to_plane * dist_to_plane < search_distance_squared)
			{
				if (kdtree[element].u.inner.index & 0x10000000)
				{
					int index = kdtree[element].u.inner.index & 0xfffffff;
					float dist_to_point_squared = (point - photons[index]->position).length_squared();
					if (dist_to_point_squared < search_distance_squared)
						closest_photons.insert_element(photons[index], dist_to_point_squared);
				}
				find_photons(kdtree, photons, point, search_distance_squared, closest_photons, 2 * element + 1);
			}

		}
		else
		{
			find_photons(kdtree, photons, point, search_distance_squared, closest_photons, 2 * element + 1);
			if (dist_to_plane * dist_to_plane < search_distance_squared)
			{
				if (kdtree[element].u.inner.index & 0x10000000)
				{
					int index = kdtree[element].u.inner.index & 0xfffffff;
					float dist_to_point_squared = (point - photons[index]->position).length_squared();
					if (dist_to_point_squared < search_distance_squared)
						closest_photons.insert_element(photons[index], dist_to_point_squared);
				}
				find_photons(kdtree, photons, point, search_distance_squared, closest_photons, 2 * element);
			}

		}
	}
	else
	{

		float dist_to_point_squared = (point - photons[kdtree[element].u.leaf.index]->position).length_squared();
		if (dist_to_point_squared < search_distance_squared)
		{
			closest_photons.insert_element(photons[kdtree[element].u.leaf.index], dist_to_point_squared);
			/*if (closest_photons.capacity == closest_photons.size)
			{
				search_distance_squared = (point - photons_leaf->points[i]->position).length_squared();
			}*/
		}
	}
}