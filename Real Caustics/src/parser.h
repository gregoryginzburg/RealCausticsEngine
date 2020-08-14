#ifndef PARSER_H
#define PARSER_H
#include "mesh.h"
#include <memory>
#include "materials.h"

void parse(std::string path, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material_idx);


#endif
