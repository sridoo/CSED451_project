#include <thread>
#include <vector>
#include <iostream>

#include "transmit.hpp"
#include "atmParams.hpp"

using namespace glm;
using namespace std;

static constexpr double dtransmitT_hDim = transmitT_hDim, dtransmitT_cosDim = transmitT_cosDim;

template<typename T>
static double integrate(T fun, double from, double to, double step) {
	double val = 0, last = fun(from);
	for (double i = from + step; i < to; i += step) {
		double cur = fun(i);
		val += step * (last + cur) / 2.f;
		last = cur;
	}
	return val;
}

class Density {
	const double dirCos_, initRadius_, initRadius2_;
public:
	Density(double dirCos, double initRadius, double initRadius2)
		: dirCos_(dirCos), initRadius_(initRadius), initRadius2_(initRadius2) {}

	double operator()(double x) {
		return displacedDensity(dirCos_, initRadius_, initRadius2_, x);
	}
};

constexpr float heightStep = atmThick / dtransmitT_hDim, cosStep = 2.0 / dtransmitT_cosDim;

void bakeTransmitTPart(dvec3* transmitT, size_t from, size_t end) {

	for (size_t i = from; i < end; ++i) {
		double curRadius = earthRadius + static_cast<double>(i) * heightStep;
		double curRadius2 = pow(curRadius, 2);
		dvec3* const arr = transmitT + i * transmitT_cosDim;

		for (size_t j = 0; j < transmitT_cosDim; ++j) {
			double curCos = 1.0 - cosStep * static_cast<double>(j);
			double xEnd = getHerizonAtmDepth(curRadius, curRadius2, curCos);
			dvec3 val = -rayScatCoef * integrate(Density(curCos, curRadius, curRadius2), 0.0, xEnd, 7.0);
			arr[j] = { exp(val.x), exp(val.y), exp(val.z) };
		}
	}
}

TransmitTable bakeTransmitTable() {
	unique_ptr<dvec3[]> transmitT(new dvec3[transmitT_hDim * transmitT_cosDim]);

	size_t nThreads = thread::hardware_concurrency();
	vector<thread> threads;
	threads.reserve(nThreads);
	size_t blockSize = transmitT_hDim / nThreads;

	for (size_t i = 0; i < nThreads; ++i) {
		threads.emplace_back(bakeTransmitTPart, transmitT.get(), i * blockSize, (i + 1) * blockSize);
	}
	bakeTransmitTPart(transmitT.get(), nThreads * blockSize, transmitT_hDim);

	for (auto& i : threads)
		i.join();

	return std::move(transmitT);
}

void writeTransmitTableReadable(ostream& out, size_t lineLimit, const TransmitTable& table) {
	
	size_t n_lines = 0;
	for (size_t i = 0; i < transmitT_hDim; ++i) {
		out << "Height: " << static_cast<double>(i) * heightStep << endl;
		const dvec3* const arr = table.get() + i * transmitT_cosDim;

		for (size_t j = 0; j < transmitT_cosDim; ++j) {
			out << "cos(Angle) = " << 1.0 - cosStep * static_cast<double>(j) << ": ";
			out << arr[j].x << ", " << arr[j].y << ", " << arr[j].z << endl;
			if (n_lines++ >= lineLimit)
				return;
		}
	}
}

void writeTransmitTable(ostream& out, const TransmitTable& table) {
	out.write((const char*)table.get(), sizeof(dvec3) * transmitT_hDim * transmitT_cosDim);
}


TransmitTable::TransmitTable(istream& in)
 : val(new dvec3[transmitT_hDim * transmitT_cosDim]){
	in.read((char*)val.get(), sizeof(dvec3) * transmitT_hDim * transmitT_cosDim);
}

TransmitTable::TransmitTable(unique_ptr<dvec3[]> arr) : val(std::move(arr)) {}

dvec3* TransmitTable::get() {
	return val.get();
}
const dvec3* TransmitTable::get() const{
	return val.get();
}

const dvec3& TransmitTable::operator()(double radius, double cosine) const {
	return val[size_t((radius - earthRadius) / heightStep) * transmitT_cosDim + size_t((1.0 - cosine) / cosStep)];
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

	size_t nThreads = thread::hardware_concurrency();
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

void writeTransmitTableTex(std::ostream& out, const TransmitTable& table) {
	out.write((const char*)table.toRGB().get(), sizeof(GLubyte) * transmitT_hDim * transmitT_cosDim * 3);
}