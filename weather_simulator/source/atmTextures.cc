#include<fstream>
#include <memory>

#include "atmParams.hpp"
#include "atmTextures.hpp"

using namespace std;

GLuint transmitTableTexID;


static constexpr size_t transmitT_hDim = 512, transmitT_cosDim = 512;

void loadTransmitTableTexID() {
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

void loadAtmTextures() {
	loadTransmitTableTexID();
}