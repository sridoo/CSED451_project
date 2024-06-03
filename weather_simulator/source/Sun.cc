#include <cmath>

#include "time.hpp"
#include "Sun.hpp"
#include "shaderParams.hpp"
#include "transmit.hpp"

using namespace glm;

static constexpr double sunRaidus = 10;
static const float PI = acosf(-1.f);

Sun& Sun::instance() {
	static Sun self;
	return self;
}

Sun::Sun() {
	verts_[0] = {};

	static const float angleStep = 2.f * PI / (verts_.size() - 1);

	for (size_t i = 1; i < verts_.size(); ++i) {
		float angle = PI * static_cast<float>(i) * angleStep;
		verts_[i] = { 0, sinf(angle), cosf(angle) };
	}

	glCreateVertexArrays(1, &vaoID_);
	glBindVertexArray(vaoID_);

	GLuint baoID;
	glCreateBuffers(1, &baoID);
	glBindBuffer(GL_ARRAY_BUFFER, baoID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts_), verts_.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(SunProgramPram::vPos, 3, GL_FLOAT, GL_FALSE, sizeof(BAOelem), (void*)offsetof(BAOelem, vPos));
	for (size_t i = 0; i < SunProgramPram::vEnd; ++i)
		glEnableVertexArrayAttrib(vaoID_, i);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &baoID);
}

void Sun::drawSunIter(const mat4& intrinsicMat, const mat4& exTrinsicMat) {
	glBindVertexArray(vaoID_);
	glUniformMatrix4fv(StaticObjProgramParam::tr, 1, GL_FALSE, value_ptr(intrinsicMat * exTrinsicMat));
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLE_FAN, 0, bao.size());
	glBindVertexArray(0);
}