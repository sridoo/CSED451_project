#include<fstream>
#include <iostream>
#include <memory>

#include "atmTextures.hpp"

using namespace std;

GLuint transmitTableTexID;
GLuint intensityTableTexID;


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
	ifstream texFile(".\\resource\\atmData\\singleScatterTex.tex", std::ios::binary);
	unique_ptr<GLubyte[]> texture(new GLubyte[intensity_hDim * intensity_viewDim * intensity_sunDim * 3]);
	texFile.read((char*)texture.get(), intensity_hDim * intensity_viewDim * intensity_sunDim * 3 * sizeof(GLubyte));

	for (size_t i = 0; i < 1000; i++)
		cout << (int)texture[i * 3] << ", " << (int)texture[i * 3 + 1] << ", " << (int)texture[i * 3 + 2] << endl;

	glGenTextures(1, &intensityTableTexID);
	glBindTexture(GL_TEXTURE_3D, intensityTableTexID);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, intensity_viewDim, intensity_hDim, intensity_sunDim, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.get());
	glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void loadAtmTextures() {
	loadTransmitTableTex();
	loadIntensityTableTex();
}