#pragma once
#include <array>

#include "GL/glew.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

class Sun {
	Sun();
	std::array<glm::vec3, 65> verts_;
	GLuint vaoID_;

public:
	static Sun& instance();
	void drawSunIter(const glm::mat4& intrinsicMat, const glm::mat4& exTrinsicMat);
};