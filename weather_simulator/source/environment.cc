#include<iostream>
#include<cstddef>
#include<array>
#include<map>
#include<algorithm>

#include "GL/glew.h"
#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "stb_image.h"
#include "shaderParams.hpp"
#include "readObjFile.hpp"
#include "globalParams.hpp"

using namespace std;
using namespace glm;

static mat2x3 getMBB(const vector<BAOelem>& bao, const vector<GLuint>& ebo) {
	if (ebo.empty())
		return {};

	mat2x3 val = { 10000, 10000, 10000, -10000, -10000, -10000 };
	vec3& mins = val[0], & maxes = val[1];

	for (auto i : ebo) {
		auto& coord = bao[i].vPos;
		for (size_t j = 0; j < vec3::length(); j++) {
			mins[j] = std::min(mins[j], coord[j]);
			maxes[j] = std::max(maxes[j], coord[j]);
		}
	}

	return val;
}

static mat2x3 getMBB(const mat2x3& o1, const mat2x3& o2) {
	mat2x3 val;
	vec3& mins = val[0], & maxes = val[1];
	for (size_t j = 0; j < vec3::length(); j++) {
		mins[j] = std::min(o1[0][j], o2[0][j]);
		maxes[j] = std::max(o1[1][j], o2[1][j]);
	}
	return val;
}

static GLuint makeTexID(const char* filePath) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 텍스처 로드 및 생성
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLint format = (nrChannels == 3 ? GL_RGB : GL_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}


static class Plain final {
	GLuint vao, textureID, baoLen;
	Plain();

	static constexpr char texturePath[] = ".\\resource\\grass.jpg";

	void initVAO();

public:
	static Plain& instance();
	void drawIter(const mat4& intrinsicMat, const mat4& exTrinsicMat);
};

Plain& Plain::instance() {
	static Plain self;
	return self;
}

Plain::Plain() {
	initVAO();
	textureID = makeTexID(texturePath);
}

void Plain::initVAO() {
	const array<BAOelem, 4> bao = {
		//farLeft
		BAOelem{{-mapWidth / 2.0f, 0.f, -mapWidth / 2.0f}, {0, 1, 0}, {-mapWidth / 2.0f, -mapWidth / 2.0f}},
		//FarRight
		{{mapWidth / 2.0f, 0.f, -mapWidth / 2.0f}, {0, 1, 0}, {mapWidth / 2.0f, -mapWidth / 2.0f}},
		//closeRight
		{{mapWidth / 2.0f, 0.f, mapWidth / 2.0f}, {0, 1, 0}, {mapWidth / 2.0f, mapWidth / 2.0f}},
		//closeLeft
		{{-mapWidth / 2.0f, 0.f, mapWidth / 2.0f}, {0, 1, 0}, {-mapWidth / 2.0f, mapWidth / 2.0f}}
	};

	baoLen = bao.size();

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint baoID;
	glCreateBuffers(1, &baoID);
	glBindBuffer(GL_ARRAY_BUFFER, baoID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bao), bao.data(), GL_STATIC_DRAW);
	vertexAttribPointerForStaticObj(vao);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &baoID);
}

void Plain::drawIter(const mat4& intrinsicMat, const mat4& exTrinsicMat) {
	glBindVertexArray(vao);
	glUniformMatrix4fv(StaticObjProgramParam::tr, 1, GL_FALSE, value_ptr(intrinsicMat * exTrinsicMat));
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLE_FAN, 0, baoLen);
	glBindVertexArray(0);
}

static class Tree final {
	//vaoID, textureID, eboLen;
	vector<array<GLuint, 3>> vaoIDs;
	mat4 modelTr;

	Tree();

	static constexpr float treeHeight = 2;
	static constexpr char texturePath[] = ".\\resource\\Tree\\Texture.png";

public:
	static Tree& instance();
	void drawIter(const mat4& intrinsicMat, const mat4& exTrinsicMat);
};

Tree& Tree::instance() {
	static Tree self;
	return self;
}

Tree::Tree() {
	auto [bao, groups, mtlFileName] = readObjFile(".\\resource\\tree\\tree_in_OBJ.obj");
	vector<GLuint> boIDs(groups.size() + 1);  //eboID + baoIDs
	vaoIDs.resize(groups.size());

	glCreateBuffers(boIDs.size(), boIDs.data());
	glNamedBufferData(boIDs.back(), bao.size() * sizeof(BAOelem), bao.data(), GL_STATIC_DRAW);

	mat2x3 mbr;
	bool mbrInitFlag = false;
	vector<GLuint> texIdxMap(mtlFileName.size(), -1);

	for (size_t i = 0; i < groups.size(); i++) {
		auto& [ebo, mtlIdx] = groups[i];
		auto& vaoID = vaoIDs[i];
		
		if (texIdxMap[mtlIdx] == -1) 
			texIdxMap[mtlIdx] = makeTexID(mtlFileName[mtlIdx].c_str());

		vaoID[1] = texIdxMap[mtlIdx];
		vaoID[2] = ebo.size();

		glCreateVertexArrays(1, &vaoID[0]);
		glBindVertexArray(vaoID[0]);

		glBindBuffer(GL_ARRAY_BUFFER, boIDs.back());
		vertexAttribPointerForStaticObj(vaoID[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boIDs[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo.size() * sizeof(GLuint), ebo.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		if (mbrInitFlag)
			mbr = getMBB(mbr, getMBB(bao, ebo));
		else {
			mbrInitFlag = true;
			mbr = getMBB(bao, ebo);
		}
	}

	modelTr = scale(mat4(1), treeHeight / (mbr[1].y - mbr[0].y) * vec3(1));
	modelTr = translate(modelTr,
		{
			-0.5f * (mbr[0].x + mbr[1].x),
			-mbr[0].y,
			-0.5f * (mbr[0].z + mbr[1].z),
		}
	);
}

void Tree::drawIter(const mat4& intrinsicMat, const mat4& exTrinsicMat) {
	glUniformMatrix4fv(StaticObjProgramParam::tr, 1, GL_FALSE, value_ptr(intrinsicMat * exTrinsicMat * modelTr));
	for (auto& [vao, textureID, eboLen] : vaoIDs) {
		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawArrays(GL_TRIANGLES, 0, eboLen);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}

static class EnvProto final{
	vector<vec3> treeCoords;
	EnvProto();

public:
	static EnvProto& instance();
	void drawIter(const mat4& intrinsicMat, const mat4& exTrinsicMat);
};

EnvProto& EnvProto::instance() {
	static EnvProto self;
	return self;
}

EnvProto::EnvProto() {
	treeCoords = { {0, 0, 0} };
}


void EnvProto::drawIter(const mat4& intrinsicMat, const mat4& exTrinsicMat) {
	glUseProgram(staticObjProgram);
	Plain::instance().drawIter(intrinsicMat, exTrinsicMat);
	for (auto& treeCoord : treeCoords)
		Tree::instance().drawIter(intrinsicMat, translate(exTrinsicMat, treeCoord));
}

void drawEnvIter(const mat4& intrinsicMat, const mat4& exTrinsicMat) {
	EnvProto::instance().drawIter(intrinsicMat, exTrinsicMat);
}