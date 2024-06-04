#pragma once

#include "GL/glew.h"
#include "glm/mat4x4.hpp"

class Sun {
	Sun();
	GLuint vaoID_, aboLen_;

public:
	static Sun& instance();
	void drawSunIter(const glm::mat4& intrinsicMat, const glm::mat4& extrinsicMat);
};