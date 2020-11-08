#ifndef PHOTON_MAP_H
#define PHOTON_MAP_H
#include <vector>
#include <memory>
#include "Color.h"
#include "aabb.h"

extern const float inf;
extern const float PI;
class Catcher;
class Path_Vertex;

struct KDTreeNode
{
	union
	{
		struct
		{
			// 4 bit 0 - if no objects on split
			//2 bits for split axis: 00-x 01-y 11-z
			//maxumum value = 268 435 456
			unsigned index;
			float split;
		} inner;
		struct
		{
			//higest bit 0 - leaf; 1 - inner
			unsigned index;
			unsigned start_index;
		} leaf;		
	} u;
};


struct temp_path_vertex
{
	Path_Vertex* path_vertex;
	unsigned index;

	temp_path_vertex(Path_Vertex* path_vertex_ptr, unsigned i) : path_vertex(path_vertex_ptr), index(i) {}

	~temp_path_vertex()
	{
		path_vertex = nullptr;
	}
};


class KD_Tree
{
public:
	std::vector<KDTreeNode> kdtree;
public:
	KD_Tree() {}

	void build_kd_tree(std::vector<Path_Vertex>& path_vertices);

/**
	*	\brief Find closest path vertices in radius.
	*
	*	Find closest path vertices in radius.
	*
	*	\param point: Find closest path vertices from point.
	*	\param search_d: Search distance !squared!
	*	\param path_vertices: Vector of all path vertices
	*	\param element: Set to 1
**/
	void find_closest_in_radius(
		const vec3& point,
		float& search_d,
		std::vector<Path_Vertex>& path_vertices,
		std::vector<Path_Vertex>& closest_path_vertices,
		int element);
};

#endif