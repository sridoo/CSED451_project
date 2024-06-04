#pragma once

#include "GL/glew.h"
#include "glm/mat4x4.hpp"

class SkyBox {
	SkyBox();
	GLuint vaoID_, aboLen_;

public:
	static SkyBox& instance();
	void drawSkyBoxIter(const glm::mat4& intrinsicMat);
};