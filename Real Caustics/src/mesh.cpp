#include "mesh.h"

#include <fstream>
#include <iostream>
#include "Blender_definitions.h"
#include "Triangle.h"
#include "BVH/BVH_mesh.h"
#include "aabb.h"
#include "Transform.h"


Mesh::Mesh(Mesh_blender *mesh_blender_data, unsigned int number_of_vertices, unsigned int number_of_tris, const Matrix4x4& matrix,
	int* material_indices, bool smooth_shade) : shade_smooth(smooth_shade)
{
	number_of_triangles = number_of_tris;
	vertices = mesh_blender_data->mvert;
	WorldTransformation = Transform(matrix);
	triangles = new Triangle[number_of_tris];
	
	for (int i = 0; i < number_of_tris; ++i)
	{
        triangles[i] = Triangle(mesh_blender_data->mloop[mesh_blender_data->mpoly[i].loopstart].v,
                                mesh_blender_data->mloop[mesh_blender_data->mpoly[i].loopstart + 1].v,
                                mesh_blender_data->mloop[mesh_blender_data->mpoly[i].loopstart + 2].v,
								material_indices[mesh_blender_data->mpoly[i].mat_nr]);
    }
}




void Mesh::update_bvh(bool was_changed, const char *file_path)
{
	if (was_changed)
	{
		BVHNode_mesh *root = make_default_bvh();
		aabb mesh_bbox;
		BVHInner_mesh *test_root_cast = dynamic_cast<BVHInner_mesh *>(root);

		if (test_root_cast == nullptr)
		{
			BVHLeaf_mesh* casted_root = dynamic_cast<BVHLeaf_mesh *>(root);
			mesh_bbox = casted_root->bbox;
		}		 
		else
		{
			mesh_bbox = test_root_cast->bbox;
		}

		bounding_box = mesh_bbox;
		create_cache_friendly_bvh(root, BVH, file_path);

		BVH.cost = CostOfTraversing(BVH, 0);
	}
	else
	{
		/*
		static std::ifstream file(file_path, std::ios::binary);
		int size1;
		int size2;
		file.read((char *)&size1, sizeof(int));
		BVH.tris_indices.resize(size1);
		file.read((char *)&BVH.tris_indices[0], sizeof(int) * (size_t)size1);
		file.read((char *)&size2, sizeof(int));
		BVH.bvh_nodes.resize(size2);
		file.read((char *)&BVH.bvh_nodes[0], sizeof(CacheBVHNode_mesh) * size2);*/
	}
}
//index = 0
bool Mesh::hit(const ray &r, float tmin, float tmax, Isect &hit_inf, int index) const
{
	//1 - leaf node
	
	constexpr int BVH_STACK_SIZE = 64;

	int stack[BVH_STACK_SIZE];

	int stackIdx = 0;
	stack[stackIdx++] = 0;

	bool hit = false;
	while (stackIdx)
	{
		int boxID = stack[stackIdx - 1];

		stackIdx--;

		const CacheBVHNode_mesh& node = BVH.bvh_nodes[boxID];

		if (node.bounding_box.hit(r, tmin, hit_inf.distance))
		{
			//Leaf
			if (node.u.leaf.count & 0x80000000)
			{
				hit |= node.hit(r, tmin, hit_inf.distance, hit_inf, BVH, triangles, vertices, WorldTransformation);
			}
			else
			{
				stack[stackIdx++] = node.u.inner.idxRight;
				stack[stackIdx++] = node.u.inner.idxLeft;
			}
		}

	}
	return hit;

	/**
	const CacheBVHNode_mesh& node = BVH.bvh_nodes[index];

	if (node.bounding_box.hit(r, tmin, tmax))
	{
		if (node.u.leaf.count & 0x80000000)
		{
			return node.hit(r, tmin, tmax, hit_inf, BVH, triangles, vertices, WorldTransformation);
		}
		else
		{
			bool left_hit = hit(r, tmin, tmax, hit_inf, node.u.inner.idxLeft);
			bool right_hit = hit(r, tmin, left_hit ? hit_inf.distance : tmax, hit_inf, node.u.inner.idxRight);

			return left_hit || right_hit;
		}
	}
	else
	{
		return false;
	}
	*/
	
}

