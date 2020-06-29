#ifndef PARSER_H
#define PARSER_H
#include "mesh.h"
#include <memory>
#include "materials.h"

void parse(std::string path, Mesh& object, std::shared_ptr<Material> material);


#endif
