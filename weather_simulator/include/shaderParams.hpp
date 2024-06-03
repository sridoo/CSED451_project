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

namespace StaticObjProgramParam {
	enum StaticObjProgramParam : GLuint {
		vPos, vNormal, vtexCoord, vEnd,
		tr = 0, texture = 4
	};
}

namespace SunProgramPram {
	enum SunProgramPram : GLuint {
		vPos, vEnd,
		inTr = 0, exTr = 4, texture = 8, vViewHeight = 9
	};
}

void vertexAttribPointerForStaticObj(GLuint vao);

extern GLuint staticObjProgram;
extern GLuint sunProgram;