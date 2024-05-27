#pragma once

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

struct BAOelem {
	glm::vec3 vPos;
	glm::vec3 vNormal;
	glm::vec2 vTexCoord;

	BAOelem(const glm::vec3& pos_, const glm::vec3& normal_, const glm::vec2& texCoord_)
		:vPos({ pos_[0], pos_[1], pos_[2] }),
		vNormal({ normal_[0], normal_[1], normal_[2] }),
		vTexCoord({ texCoord_[0], texCoord_[1] }) {}
	BAOelem(const float* pos_, const float* normal_, const float* texCoord_)
		:vPos({ pos_[0], pos_[1], pos_[2] }),
		vNormal({ normal_[0], normal_[1], normal_[2] }),
		vTexCoord({ texCoord_[0], texCoord_[1] }) {}
	BAOelem() = default;
};

enum StaticObjVertParam {
	vPos, vNormal, vtexCoord, vEnd,
	tr = 0, texture = 4
};

void vertexAttribPointerForStaticObj(GLuint vao);