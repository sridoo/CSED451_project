#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "GL/glew.h"

#include "shaderParams.hpp"

//fileName -> (bao, list((ebo, materialID), material))
std::tuple<std::vector<BAOelem>, std::vector<std::pair<std::vector<GLuint>, size_t>>, std::vector<std::string>> readObjFile(const std::string& objFilePath);