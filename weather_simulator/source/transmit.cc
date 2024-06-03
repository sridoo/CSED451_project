#include <thread>
#include <vector>
#include <exception>
#include <fstream>

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

	file.read((char*)val.get(), transmitT_hDim * transmitT_cosDim);
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

void toFloatPart(vec3* ftransmitT, const dvec3* dtransmitT, size_t from, size_t end) {
	for (size_t i = from; i < end; ++i) {
		size_t off = i * transmitT_cosDim;
		const dvec3* const darr = dtransmitT + off;
		vec3* const farr = ftransmitT + off;

		for (size_t j = 0; j < transmitT_cosDim; ++j)
			farr[j] = darr[j];
	}
}

FtransmitTable TransmitTable::toFloat() const {
	unique_ptr<vec3[]> fval(new vec3[transmitT_hDim * transmitT_cosDim]);

	size_t nThreads = jthread::hardware_concurrency();
	vector<thread> threads;
	threads.reserve(nThreads);
	size_t blockSize = transmitT_hDim / nThreads;

	for (size_t i = 0; i < nThreads; ++i) {
		threads.emplace_back(toFloatPart, fval.get(), val.get(), i * blockSize, (i + 1) * blockSize);
	}
	toFloatPart(fval.get(), val.get(), nThreads * blockSize, transmitT_hDim);

	for (auto& i : threads)
		i.join();

	return std::move(fval);
}

FtransmitTable::FtransmitTable(std::unique_ptr<glm::vec3[]> val)
	: val_(std::move(val)){}

vec3* FtransmitTable::get() {
	return val_.get();
}

const vec3* FtransmitTable::get() const {
	return val_.get();
}

TransmitTable transmitTable(".\\resource\\atmData\\transmitTable.cache");
FtransmitTable fTransmitTable;