#include <cmath>
#include <array>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Sun.hpp"
#include "shaderParams.hpp"
#include "atmTextures.hpp"
#include "globalParams.hpp"
#include "camera.hpp"

using namespace glm;
using namespace std;

static constexpr double sunRaidus = 5.0;
static const float PI = acosf(-1.f);

Sun& Sun::instance() {
	static Sun self;
	return self;
}

Sun::Sun() {
	array<vec3, 66> verts_;
	aboLen_ = verts_.size();
	verts_[0] = { mapWidthHalf, 0, 0};

	static const float angleStep = 2.f * PI / (verts_.size() - 2);

	for (size_t i = 1; i < verts_.size(); ++i) {
		float angle = static_cast<float>(i) * angleStep;
		verts_[i] = { mapWidthHalf - 1.f, sunRaidus * sinf(angle), sunRaidus * cosf(angle) };
	}

	glCreateVertexArrays(1, &vaoID_);
	glBindVertexArray(vaoID_);

	GLuint baoID;
	glCreateBuffers(1, &baoID);
	glBindBuffer(GL_ARRAY_BUFFER, baoID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts_), verts_.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(SunProgramParam::vPos, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	for (size_t i = 0; i < SunProgramParam::vEnd; ++i)
		glEnableVertexArrayAttrib(vaoID_, i);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &baoID);
}

void Sun::drawSunIter(const mat4& intrinsicMat, const mat4& exTrinsicMat) {
	glUseProgram(sunProgram);
	float angle = (curTime - 6.f) / 12.f * PI;
	mat4 tr = glm::rotate(exTrinsicMat, angle, { 0, 0, 1.f });
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, transmitTableTexID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, intensityTableTexID[0]);

	int i;

	glBindVertexArray(vaoID_);
	glUniformMatrix4fv(SunProgramParam::inTr, 1, GL_FALSE, value_ptr(intrinsicMat));
	glUniformMatrix4fv(SunProgramParam::exTr, 1, GL_FALSE, value_ptr(tr));
	glUniform1f(SunProgramParam::viewHeightN, camPos.y / 8000.f);
	glUniform1f(SunProgramParam::sunCos, sunCos);
	glUniform1i(SunProgramParam::scatterTex, 1);
	glUniform1i(SunProgramParam::transmitTex, 0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, aboLen_);
	glBindVertexArray(0);
}