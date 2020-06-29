#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <memory>
#include "vec3.h"
#include "mesh.h"
#include "hittable_list.h"
#include "utils.h"
#include "vec2.h"
#include "materials.h"
void parse(std::string path, Mesh& object, std::shared_ptr<Material> material)
{
	std::ifstream in;
	in.open(path);
	if (!in.is_open())
	{
		std::cout << "Error";
	}


	std::vector<vec3> vertices;
	std::vector<vec2> texture_coordinates_verts;
	std::vector<vec3> vertex_normals;
	std::vector<int> face_indices;
	std::vector<int> texture_coordinate_indices;
	std::vector<int> vertex_normals_indices;
	// str - одна строка
	std::string str;
	while (!in.eof())
	{
		//обнуление строки
		str = "";
		std::getline(in, str);	
		//вершина треугольника
		vec3 point;
		//точка texture coordinate вершины
		vec2 texture_coordinate;
		//нормаль вершины
		vec3 vertex_normal;
		//32 - пробел 
		std::vector<std::string> word = split(str, 32);

		std::string first_symbol = word[0];
		if (first_symbol == "v")
		{
			point.x = std::stod(word[1]);
			point.y = std::stod(word[2]);
			point.z = std::stod(word[3]);
			vertices.push_back(point);
			continue;

		}
		if (first_symbol == "vt")
		{
			texture_coordinate.x = std::stod(word[1]);
			texture_coordinate.y = std::stod(word[2]);
			texture_coordinates_verts.push_back(texture_coordinate);
			continue;
		}
		if (first_symbol == "f")
		{
			for (int i = 1; i < 4; ++i)
			{
				//   47 - /  indices - a/b with f in the start
				std::vector<std::string> indicies = split(word[i], 47);
				face_indices.push_back(std::stoi(indicies[0]));
				texture_coordinate_indices.push_back(std::stoi(indicies[1]));
				vertex_normals_indices.push_back(std::stoi(indicies[2]));
			}
			continue;
		}
		if (first_symbol == "vn")
		{
			vertex_normal.x = std::stod(word[1]);
			vertex_normal.y = std::stod(word[2]);
			vertex_normal.z = std::stod(word[3]);
			vertex_normals.push_back(vertex_normal);
			continue;
		}
	}
	//закрыть файл
	in.close();

	//количество треугольников
	int size = face_indices.size() ;
	point3 vertice0;
	point3 vertice1;
	point3 vertice2;
	vec2 t_vert0;
	vec2 t_vert1;
	vec2 t_vert2;
	vec3 v_n0;
	vec3 v_n1;
	vec3 v_n2;

	
	
	for (int i = 0; i < size; ++i)
	{
		//cas - case, добавление вершин в структуру треугольника
		int cas = i % 3;
		if (cas == 0) 
		{
			vertice0 = vertices[face_indices[i] - 1];
			t_vert0 = texture_coordinates_verts[texture_coordinate_indices[i] - 1]; 
			v_n0 = vertex_normals[vertex_normals_indices[i] - 1];
			continue;
		}
	
		if (cas == 1) 
		{ 
			vertice1 = vertices[face_indices[i] - 1]; 
			t_vert1 = texture_coordinates_verts[texture_coordinate_indices[i] - 1];
			v_n1 = vertex_normals[vertex_normals_indices[i] - 1];
			continue; 
		}
		if (cas == 2)
		{ 
			vertice2 = vertices[face_indices[i] - 1];
			t_vert2 = texture_coordinates_verts[texture_coordinate_indices[i] - 1];
			v_n2 = vertex_normals[vertex_normals_indices[i] - 1];
			object.add(std::make_shared<Triangle>(vertice0, vertice1, vertice2, t_vert0, t_vert1, t_vert2, material, v_n0, v_n1, v_n2));
		}
	}
	face_indices.clear();
	face_indices.shrink_to_fit();
	texture_coordinate_indices.clear();
	texture_coordinate_indices.shrink_to_fit();
	vertex_normals_indices.clear();
	vertex_normals_indices.shrink_to_fit();

}