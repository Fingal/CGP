#pragma once
#include <vector>
#include "glm.hpp"

bool loadObj(const char * path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec3>& out_normals, std::vector<glm::vec2>& out_uvs);