#include <thread>
#include <vector>
#include <exception>
#include <fstream>
#include <iostream>

#include "atmParams.hpp"
#include "transmit.hpp"

using namespace glm;
using namespace std;

static constexpr double dtransmitT_hDim = transmitT_hDim, dtransmitT_cosDim = transmitT_cosDim;

TransmitTable::TransmitTable(const char* cachePath)
	: val(new dvec3[transmitT_hDim * transmitT_cosDim]) {

	ifstream file(cachePath, std::ios::binary);
	if (!file)
		throw invalid_argument("Cannot fine transmitTable cache file: "s + cachePath);

	file.read((char*)val.get(), transmitT_hDim * transmitT_cosDim * sizeof(dvec3));
}

dvec3* TransmitTable::get() {
	return val.get();
}
const dvec3* TransmitTable::get() const{
	return val.get();
}

const dvec3& TransmitTable::operator()(double radius, double cosine) const {
	return val[size_t((radius - earthRadius) / atmThick * dtransmitT_hDim * dtransmitT_cosDim) + size_t((1.0 + cosine) / 2.0 * dtransmitT_cosDim)];
}

void toRGBPart(GLubyte* rgbTransmitT, const dvec3* dtransmitT, size_t from, size_t end) {
	for (size_t i = 0; i < end; ++i) {
		size_t off = i * transmitT_cosDim;
		const dvec3* const darr = dtransmitT + off;
		GLubyte* const rgbArr = rgbTransmitT + off * 3;

		for (size_t j = 0; j < transmitT_cosDim; ++j) {
			GLubyte* const rgbElem = rgbArr + j * 3;
			
			rgbElem[0] = static_cast<unsigned char>(darr[j][0] * 255.0);
			rgbElem[1] = static_cast<unsigned char>(darr[j][1] * 255.0);
			rgbElem[2] = static_cast<unsigned char>(darr[j][2] * 255.0);
		}
	}
}

unique_ptr<GLubyte[]> TransmitTable::toRGB() const {
	unique_ptr<GLubyte[]> rgbVal(new GLubyte[transmitT_hDim * transmitT_cosDim * 3]);

	size_t nThreads = jthread::hardware_concurrency();
	vector<thread> threads;
	threads.reserve(nThreads);
	size_t blockSize = transmitT_hDim / nThreads;

	for (size_t i = 0; i < nThreads; ++i) {
		threads.emplace_back(toRGBPart, rgbVal.get(), val.get(), i * blockSize, (i + 1) * blockSize);
	}
	toRGBPart(rgbVal.get(), val.get(), nThreads * blockSize, transmitT_hDim);

	for (auto& i : threads)
		i.join();
	return rgbVal;
}

TransmitTable transmitTable(".\\resource\\atmData\\transmitTable.cache");
GLuint transmitTableTexID;

void initTransmitTableTexID() {
	auto fTransmitTable = transmitTable.toRGB();
	glGenTextures(1, &transmitTableTexID);
	glBindTexture(GL_TEXTURE_2D, transmitTableTexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, transmitT_hDim, transmitT_cosDim, 0, GL_RGB, GL_UNSIGNED_BYTE, fTransmitTable.get());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}