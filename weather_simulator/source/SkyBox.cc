#include<array>
#include <cmath>
#include <iostream>

#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "globalParams.hpp"
#include "shaderParams.hpp"
#include "atmTextures.hpp"
#include "SkyBox.hpp"
#include "camera.hpp"

using namespace std;
using namespace glm;

SkyBox::SkyBox() {
	array<vec3, 30> verts = {
		//top
		vec3{-mapWidthHalf, mapWidth, mapWidthHalf},
		{-mapWidthHalf, mapWidth, -mapWidthHalf},
		{mapWidthHalf, mapWidth, -mapWidthHalf},
		{mapWidthHalf, mapWidth, -mapWidthHalf},
		{-mapWidthHalf, mapWidth, mapWidthHalf},
		{mapWidthHalf, mapWidth, mapWidthHalf},
		//front
		{-mapWidthHalf, mapWidth, mapWidthHalf},
		{-mapWidthHalf, 0.f, mapWidthHalf},
		{mapWidthHalf, 0.f, mapWidthHalf},
		{mapWidthHalf, 0.f, mapWidthHalf},
		{-mapWidthHalf, mapWidth, mapWidthHalf},
		{mapWidthHalf, mapWidth, mapWidthHalf},
		//back
		{-mapWidthHalf, mapWidth, -mapWidthHalf},
		{-mapWidthHalf, 0.f, -mapWidthHalf},
		{mapWidthHalf, 0.f, -mapWidthHalf},
		{mapWidthHalf, 0.f, -mapWidthHalf},
		{-mapWidthHalf, mapWidth, -mapWidthHalf},
		{mapWidthHalf, mapWidth, -mapWidthHalf},
		//left
		{-mapWidthHalf, mapWidth, -mapWidthHalf},
		{-mapWidthHalf, 0.f, -mapWidthHalf},
		{-mapWidthHalf, 0.f, mapWidthHalf},
		{-mapWidthHalf, 0.f, mapWidthHalf},
		{-mapWidthHalf, mapWidth, -mapWidthHalf},
		{-mapWidthHalf, mapWidth, mapWidthHalf},
		//right
		{mapWidthHalf, mapWidth, -mapWidthHalf},
		{mapWidthHalf, 0.f, -mapWidthHalf},
		{mapWidthHalf, 0.f, mapWidthHalf},
		{mapWidthHalf, 0.f, mapWidthHalf},
		{mapWidthHalf, mapWidth, -mapWidthHalf},
		{mapWidthHalf, mapWidth, mapWidthHalf},
	};
	aboLen_ = verts.size();


	glCreateVertexArrays(1, &vaoID_);
	glBindVertexArray(vaoID_);

	GLuint baoID;
	glCreateBuffers(1, &baoID);
	glBindBuffer(GL_ARRAY_BUFFER, baoID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(SkyBoxProgramParam::vPos, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	for (size_t i = 0; i < SkyBoxProgramParam::vEnd; ++i)
		glEnableVertexArrayAttrib(vaoID_, i);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &baoID);
}

void SkyBox::drawSkyBoxIter(const mat4& intrinsicMat) {
	glUseProgram(skyBoxProgram);
	for (size_t i = 0; i < 2; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_3D, intensityTableTexID[i]);
	}
	glBindVertexArray(vaoID_);
	
	glUniformMatrix4fv(SkyBoxProgramParam::inTr, 1, GL_FALSE, value_ptr(intrinsicMat));
	
	glUniform1f(SkyBoxProgramParam::viewHeightN, camPos.y / 8000.f);
	glUniform1f(SkyBoxProgramParam::sunCos, sunCos);
	glUniform1f(SkyBoxProgramParam::viewHeight, camPos.y);

	glUniform1i(SkyBoxProgramParam::intensityAMtex, 0);
	glUniform1i(SkyBoxProgramParam::intensityFMtex, 1);
	glUniform1i(SkyBoxProgramParam::isAM, sunLightDir.x > 0);

	glDrawArrays(GL_TRIANGLES, 0, aboLen_);

	glBindVertexArray(0);
}

SkyBox& SkyBox::instance() {
	static SkyBox self;
	return self;
}