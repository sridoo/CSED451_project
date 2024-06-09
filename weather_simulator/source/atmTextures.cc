#include<fstream>
#include <iostream>
#include <memory>

#include "atmTextures.hpp"

using namespace std;

GLuint transmitTableTexID;
GLuint intensityTableTexID[2];


static constexpr size_t transmitT_hDim = 512, transmitT_cosDim = 512;
static constexpr size_t intensity_hDim = 32, intensity_viewDim = 256, intensity_sunDim = 64;

static void loadTransmitTableTex() {
	ifstream transmitCache(".\\resource\\atmData\\transmitTableTex.tex", std::ios::binary);
	unique_ptr<GLubyte[]> texture(new GLubyte[transmitT_hDim * transmitT_cosDim * 3]);
	transmitCache.read((char*)texture.get(), transmitT_hDim * transmitT_cosDim * 3 * sizeof(GLubyte));

	glGenTextures(1, &transmitTableTexID);
	glBindTexture(GL_TEXTURE_2D, transmitTableTexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, transmitT_hDim, transmitT_cosDim, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.get());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static void loadIntensityTableTex() {
	const char* intensityFiles[2] = { ".\\resource\\atmData\\singleScatterTexAM.tex", ".\\resource\\atmData\\singleScatterTexFM.tex"};
	unique_ptr<GLubyte[]> texture(new GLubyte[intensity_hDim * intensity_viewDim * intensity_sunDim * 3]);
	glGenTextures(2, intensityTableTexID);

	for (size_t i = 0; i < 2; i++) {
		ifstream texFile(intensityFiles[i], std::ios::binary);
		texFile.read((char*)texture.get(), intensity_hDim * intensity_viewDim * intensity_sunDim * 3 * sizeof(GLubyte));
		glBindTexture(GL_TEXTURE_3D, intensityTableTexID[i]);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, intensity_hDim, intensity_sunDim, intensity_viewDim, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.get());
		glGenerateMipmap(GL_TEXTURE_3D);
	}
	glBindTexture(GL_TEXTURE_3D, 0);
}

void loadAtmTextures() {
	loadTransmitTableTex();
	loadIntensityTableTex();
}