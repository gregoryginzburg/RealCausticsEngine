#include "Vertex_Merging.h"
#include "Scene.h"
#include "vec3.h"
#include "ray.h"
#include "BXDF.h"
#include "Path_Vertices.h"
#include "Camera.h"
#include <cmath>
#include <fstream>
#include "BVH\BVH_mesh.h"
#include "BVH\BVH_world.h"
#include "mesh.h"
#include <string>


extern const float PI;
extern const float inf;
extern Scene* scene;


void Vertex_Merging::run_debug_normals()
{

	const int resX = scene->camera.pixel_width;
	const int resY = scene->camera.pixel_height;
	const int camera_path_count = resX * resY;


	image_buffer.init(resX, resY);


	for (int path_idx = 0; path_idx < camera_path_count; ++path_idx)
	{
		if (path_idx % 100000 == 0)
		{
			std::cout << (path_idx / (float)camera_path_count) * 100.0f << "%" << std::flush << "\r";
		}

		Camera_State_Debug camera_state;

		const float x = path_idx % resX;
		const float y = path_idx / resX;

		// Jitter pixel
		vec2 sample = vec2(x, y);

		ray r = scene->camera.get_ray(sample.x, sample.y);
		camera_state.origin = r.origin;
		camera_state.direction = r.direction;

		for (;;)
		{
			ray r = ray(camera_state.origin, camera_state.direction);

			Isect hit_inf;

			if (!scene->hit(r, 0.0001f, inf, hit_inf, 0))
				break;

			image_buffer.add_color(sample, hit_inf.shade_normal);
			break;
		}

	}
}

void Vertex_Merging::run_debug_facing()
{

	const int resX = scene->camera.pixel_width;
	const int resY = scene->camera.pixel_height;
	const int camera_path_count = resX * resY;


	image_buffer.init(resX, resY);


	for (int path_idx = 0; path_idx < camera_path_count; ++path_idx)
	{
		if (path_idx % 100000 == 0)
		{
			std::cout << (path_idx / (float)camera_path_count) * 100.0f << "%" << std::flush << "\r";
		}

		Camera_State_Debug camera_state;

		const float x = path_idx % resX;
		const float y = path_idx / resX;

		// Jitter pixel
		vec2 sample = vec2(x, y);

		ray r = scene->camera.get_ray(sample.x, sample.y);
		camera_state.origin = r.origin;
		camera_state.direction = r.direction;

		for (;;)
		{
			ray r = ray(camera_state.origin, camera_state.direction);

			Isect hit_inf;

			if (!scene->hit(r, 0.0001f, inf, hit_inf, 0))
				break;

			vec3 color;
			if (hit_inf.front_face)
				color = vec3(0, 0, 1);
			else
				color = vec3(1, 0, 0);

			float factor = std::abs(dot(r.direction, hit_inf.shade_normal));
			image_buffer.add_color(sample, color * factor);
			break;
		}

	}
}


void Vertex_Merging::run_debug_brdf()
{
	
	Isect intersection;
	intersection.geometric_normal = vec3(0, 0, 1); 
	intersection.shade_normal = vec3(0, 0, 1);
	intersection.direction = -debug.direction;
	BxDF* brdf = new Fresnel_Specular(vec3(1, 1, 1), debug.specular, intersection, TransportMode::Importance);
	Fresnel_Specular * new_brdf = dynamic_cast<Fresnel_Specular*>(brdf);

	std::ofstream o;
	o.open("D:\\BRDF\\brdf.obj");


	float f_pdf;
	float r_pdf;
	float cos_thet_out;
	BxDFType sampled_event;

	for (int i = 0; i < debug.number_of_points; ++i)
	{
		vec3 wi;
		vec3 half_vector;
		MLT_Sampler Sampler(5, 1.0f);
		float cont;
		vec3 color = new_brdf->sample(wi, intersection.geometric_normal, f_pdf, r_pdf, cos_thet_out, cont, sampled_event, Sampler);
		if (color.x != 0.0f)
			o << "v " << wi.x << " " << wi.y << " " << wi.z << "\n";

	}


}


void Vertex_Merging::run_debug_materials()
{

	const int resX = scene->camera.pixel_width;
	const int resY = scene->camera.pixel_height;
	const int camera_path_count = resX * resY;

	
	image_buffer.init(resX, resY);


	for (int path_idx = 0; path_idx < camera_path_count; ++path_idx)
	{
		if (path_idx % 100000 == 0)
		{
			std::cout << (path_idx / (float)camera_path_count) * 100.0f << "%" << std::flush << "\r";
		}

		Camera_State_Debug camera_state;

		const float x = path_idx % resX;
		const float y = path_idx / resX;

		// Jitter pixel
		vec2 sample = vec2(x, y);

		ray r = scene->camera.get_ray(sample.x, sample.y);
		camera_state.origin = r.origin;
		camera_state.direction = r.direction;

		for (;;)
		{
			ray r = ray(camera_state.origin, camera_state.direction);

			Isect hit_inf;

			if (!scene->hit(r, 0.0001f, inf, hit_inf, 0))
				break;

			vec3 color;

			Frame frame;
			frame.set_from_z(hit_inf.shade_normal);
			vec3 dir_local = frame.to_local(-hit_inf.direction);

			float ior = debug.specular;
			float etai_over_etat = ior;
			// if entering from normal side(in the substance)

			//float u = random_float(0.0f, 1.0f);
			
			//fresnel_dielectric_cos(std::abs(dir_local.z), etai_over_etat);
			if (dir_local.z > 0)
				etai_over_etat = 1.0f / ior;
			float f = fresnel_dielectric_eta(fabs(dir_local.z), etai_over_etat);
			image_buffer.add_color(sample, f);
			break;
		}

	}
}
inline std::ofstream& write_bounding_box(std::ofstream& out, const vec3& min, const vec3& max, int i)
{
	vec3 v1 = vec3(max.x, max.y, max.z);
	vec3 v2 = vec3(max.x, max.y, min.z);
	vec3 v3 = vec3(max.x, min.y, max.z);
	vec3 v4 = vec3(max.x, min.y, min.z);
	vec3 v5 = vec3(min.x, max.y, max.z);
	vec3 v6 = vec3(min.x, max.y, min.z);
	vec3 v7 = vec3(min.x, min.y, max.z);
	vec3 v8 = vec3(min.x, min.y, min.z);

	out << "v " << v1.x << " " << v1.y << " " << v1.z << "\n";
	out << "v " << v2.x << " " << v2.y << " " << v2.z << "\n";
	out << "v " << v3.x << " " << v3.y << " " << v3.z << "\n";
	out << "v " << v4.x << " " << v4.y << " " << v4.z << "\n";
	out << "v " << v5.x << " " << v5.y << " " << v5.z << "\n";
	out << "v " << v6.x << " " << v6.y << " " << v6.z << "\n";
	out << "v " << v7.x << " " << v7.y << " " << v7.z << "\n";
	out << "v " << v8.x << " " << v8.y << " " << v8.z << "\n";

	out << "f " << 1 + i << "/ " << 5 + i << "/ " << 7 + i << "/ " << 3 + i << "/\n";
	out << "f " << 4 + i << "/ " << 3 + i << "/ " << 7 + i << "/ " << 8 + i << "/\n";
	out << "f " << 8 + i << "/ " << 7 + i << "/ " << 5 + i << "/ " << 6 + i << "/\n";
	out << "f " << 6 + i << "/ " << 2 + i << "/ " << 4 + i << "/ " << 8 + i << "/\n";
	out << "f " << 2 + i << "/ " << 1 + i << "/ " << 3 + i << "/ " << 4 + i << "/\n";
	out << "f " << 6 + i << "/ " << 5 + i << "/ " << 1 + i << "/ " << 2 + i << "/\n";
	return out;
}

void write_bvh(BVH_mesh& BVH, int index, int depth, std::vector<std::ofstream>& files, std::vector<int>& indices)
{
	//1 - leaf node
	if (BVH.bvh_nodes[index].u.leaf.count & 0x80000000)
	{
		//leaf
		aabb& bbox = BVH.bvh_nodes[index].bounding_box;		
		write_bounding_box(files[depth], bbox.min, bbox.max, indices[depth]);
		indices[depth]+=8;
	}
	else
	{
		aabb& bbox = BVH.bvh_nodes[index].bounding_box;
		write_bounding_box(files[depth], bbox.min, bbox.max, indices[depth]);
		indices[depth]+=8;
		write_bvh(BVH, BVH.bvh_nodes[index].u.inner.idxLeft, depth + 1, files, indices);
		write_bvh(BVH, BVH.bvh_nodes[index].u.inner.idxRight, depth + 1, files, indices);
	}
}


void calc_max_depth(BVH_mesh& BVH, int index, int depth, int& max_depth)
{
	//1 - leaf node
	if (BVH.bvh_nodes[index].u.leaf.count & 0x80000000)
	{
		//leaf
		if (depth > max_depth)
			max_depth = depth;
	}
	else
	{
		calc_max_depth(BVH, BVH.bvh_nodes[index].u.inner.idxLeft, depth + 1, max_depth);
		calc_max_depth(BVH, BVH.bvh_nodes[index].u.inner.idxRight, depth + 1, max_depth);
	}
}


void Vertex_Merging::run_debug_bvh()
{
	auto& bvh = scene->BVH;
	int bbox = 0;

	std::vector<std::vector<std::ofstream>> files;
	int mesh_idx = 0;
	files.resize(scene->number_of_meshes);

	for (int i = 0; i < bvh.bvh_nodes.size(); ++i)
	{
		auto& bvh_node = bvh.bvh_nodes[i];
		vec3 min_world = bvh_node.bounding_box.min;
		vec3 max_world = bvh_node.bounding_box.max;

		int end_index = bvh_node.u.leaf.startIndex + (bvh_node.u.leaf.count & 0x7fffffff) + 1;
		for (int i = bvh_node.u.leaf.startIndex + 1; i < end_index; ++i)
		{
			BVH_mesh& bvh_mesh = scene->meshes[bvh.mesh_indices[i]].BVH;
			int max_depth = -1;
			calc_max_depth(bvh_mesh, 0, 1, max_depth);
			files[mesh_idx].resize(max_depth);
			for (int i = 0; i < files[mesh_idx].size(); ++i)
			{
				std::string s = "D:\\bvh\\Mesh_";
				s += std::to_string(mesh_idx + 1);
				s += "_";
				s += std::to_string(i + 1);
				s += ".obj";
				files[mesh_idx][i].open(s);
			}
			std::vector<int> indices;
			indices.resize(max_depth, 0);
			write_bvh(bvh_mesh, 0, 0, files[mesh_idx], indices);

			mesh_idx++;

		}
		//write_vec3(meshes, min_world);
		//write_vec3(meshes, max_world);
		
	}
	



}
/*
for (int j = 0; j < bvh_mesh.bvh_nodes.size(); ++j)
{
	auto& bvh_mesh_node = bvh_mesh.bvh_nodes[j];
	vec3 min_mesh = bvh_mesh_node.bounding_box.min;
	vec3 max_mesh = bvh_mesh_node.bounding_box.max;

	write_bounding_box(meshes, min_mesh, max_mesh, bbox);

	bbox += 8;
}*/