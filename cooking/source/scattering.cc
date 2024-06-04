#include <thread>
#include <vector>
#include <iostream>

#include "GL/glew.h"
#include <glm/glm.hpp>

#include "atmParams.hpp"
#include "transmit.hpp"
#include "scattering.hpp"

using namespace glm;
using namespace std;

static inline double cosineToSine(double cosine) {
	return sqrt(1 - pow(cosine, 2));
}
static inline double displacedCos(double initRadius, double initRadius2, double viewCos, double x) {
	return (initRadius - viewCos * x) / displacedRaidus(initRadius, initRadius2, viewCos, x);
}
static inline double cosSum(double cos1, double cos2) {
	return cos1 * cos2 - cosineToSine(cos1) * cosineToSine(cos2);
}
static inline double phaseFunRay(double cosine) {
	return 8.0 / 10.0 * (7.0 / 5.0 + cosine / 2.0);
}

const double PI = acos(-1.0);

template<typename Fun>
static dvec3 integrate3(Fun fun, double from, double to, double step) {
	dvec3 val = {}, last = fun(from);
	for (double i = from + step; i < to; i += step) {
		dvec3 cur = fun(i);
		val += step * (last + cur) / 2.0;
		last = cur;
	}
	return val;
}

class SingleScatAtten {
	double initRadius_, initRadius2_, viewCos_, sunCos_;
	const dvec3& transmitInit_;
	const TransmitTable& transmitTable_;
public:
	SingleScatAtten(double sunCos, double viewCos, double initRadius, const TransmitTable& transmitTable)
		: initRadius_(initRadius),
		initRadius2_(pow(initRadius, 2)), viewCos_(viewCos), sunCos_(sunCos),
		transmitInit_{ transmitTable(initRadius, viewCos) },
		transmitTable_{ transmitTable }
	{}

	dvec3 operator()(double x) {
		double newHeight = displacedRaidus(initRadius_, initRadius2_, viewCos_, x),
			displacedCosine = displacedCos(initRadius_, initRadius2_, viewCos_, x);
		return displacedDensity(viewCos_, initRadius_, initRadius2_, x)
			* transmitInit_
			/ transmitTable_(newHeight, cosSum(viewCos_, displacedCosine))
			* transmitTable_(newHeight, cosSum(sunCos_, displacedCosine));
	}
};

static constexpr double dintensity_hDim = intensity_hDim, dintensity_viewDim = intensity_viewDim, dintensity_sunDim = intensity_sunDim;

static constexpr double heightStep = atmThick / dintensity_hDim,
viewCosStep = 2.0 / dintensity_viewDim,
sunCosStep = 2.0 / dintensity_sunDim;

static void bakeSingleScatterPart(dvec3* out, size_t from, size_t to, const TransmitTable* const transmitTable) {
	for (size_t i = from; i < to; i++) {
		const double curRadius = earthRadius + static_cast<double>(i) * heightStep;
		const double curRadius2 = pow(curRadius, 2);
		dvec3* const arrh = out + i * intensity_viewDim * intensity_sunDim;

		for (size_t j = 0; j < intensity_sunDim; ++j) {
			const double curSunCos = 1 - static_cast<double>(j) * sunCosStep;
			dvec3* const arrv = arrh + j * intensity_viewDim;

			for (size_t k = 0; k < intensity_viewDim; ++k) {
				double curViewCos = 1 - static_cast<double>(k) * viewCosStep;
				arrv[k] = phaseFunRay(curSunCos) * rayScatCoef / 4.0 / PI
					* integrate3(SingleScatAtten(curSunCos, curViewCos, curRadius, *transmitTable), 0, getHerizonAtmDepth(curRadius, curRadius2, curViewCos), 15.0);
			}
		}
	}
}

static ScatterTable bakeSingleScatterTable(const TransmitTable& transmitTable) {
	unique_ptr<dvec3[]> singleScatT(new dvec3[intensity_hDim * intensity_viewDim * intensity_sunDim]);

	size_t nThreads = thread::hardware_concurrency();
	vector<thread> threads;
	threads.reserve(nThreads);

	size_t baseWorkLoad = intensity_hDim / nThreads;
	size_t additionalLoadStart = intensity_hDim - baseWorkLoad * nThreads;

	for (size_t i = 0, next, j = 0; i < intensity_hDim; i = next, j++) {
		next = i + baseWorkLoad + (j < additionalLoadStart);
		threads.emplace_back(bakeSingleScatterPart, singleScatT.get(), i, next, &transmitTable);
	}

	for (auto& i : threads)
		i.join();

	return singleScatT;
}

ScatterTable bakeScatterTable(const TransmitTable& transmitTable) {
	return bakeSingleScatterTable(transmitTable);
}

static void toRGBPart(GLubyte* out, dvec3* table, size_t from, size_t to) {
	for (size_t i = from; i < to; i++) {
		const double curRadius = earthRadius + static_cast<double>(i) * heightStep;
		const double curRadius2 = pow(curRadius, 2);
		size_t off = i * intensity_viewDim * intensity_sunDim;
		const dvec3* const arrh = table + off;
		GLubyte* const arrRgbi = out + off * 3;

		for (size_t j = 0; j < intensity_sunDim; ++j) {
			const double curSunCos = 1 - static_cast<double>(j) * sunCosStep;
			size_t offj = j * intensity_viewDim;
			const dvec3* const arrv = arrh + offj;
			GLubyte* const arrRgbj = arrRgbi + offj * 3;

			for (size_t k = 0; k < intensity_viewDim; ++k) {
				GLubyte* const rgbElem = arrRgbj + 3 * k;
				rgbElem[0] = static_cast<GLubyte>(arrv[k].x * 255.0);
				rgbElem[1] = static_cast<GLubyte>(arrv[k].y * 255.0);
				rgbElem[2] = static_cast<GLubyte>(arrv[k].z * 255.0);
			}
		}
	}
}

void writeScatterTableTex(ostream& out, const ScatterTable& scatterTable) {
	unique_ptr<GLubyte[]> rgbVal(new GLubyte[intensity_hDim* intensity_viewDim * intensity_sunDim * 3]);

	size_t nThreads = thread::hardware_concurrency();
	vector<thread> threads;
	threads.reserve(nThreads);

	size_t baseWorkLoad = intensity_hDim / nThreads;
	size_t additionalLoadStart = intensity_hDim - baseWorkLoad * nThreads;

	for (size_t i = 0, next, j = 0; i < intensity_hDim; i = next, j++) {
		next = i + baseWorkLoad + (j < additionalLoadStart);
		threads.emplace_back(toRGBPart, rgbVal.get(), scatterTable.get(), i, next);
	}

	for (auto& i : threads)
		i.join();

	out.write((const char*)rgbVal.get(), intensity_hDim * intensity_viewDim * intensity_sunDim * 3);
}


void writeScatterTReadable(std::ostream& out, size_t maxLines, const ScatterTable& scatterTable) {
	size_t count = 0;
	for (size_t i = 0; i < intensity_hDim; i++) {
		const double curRadius = earthRadius + static_cast<double>(i) * heightStep;
		const double curRadius2 = pow(curRadius, 2);
		dvec3* const arrh = scatterTable.get() + i * intensity_viewDim * intensity_sunDim;
		
		out << "Height=" << curRadius - earthRadius << endl;

		for (size_t j = 0; j < intensity_sunDim; ++j) {
			const double curSunCos = 1 - static_cast<double>(j) * sunCosStep;
			dvec3* const arrv = arrh + j * intensity_viewDim;

			out << "SunCosine=" << curSunCos << endl;

			for (size_t k = 0; k < intensity_viewDim; ++k) {
				double curViewCos = 1 - static_cast<double>(k) * viewCosStep;
				out << "VewCosine=" << curViewCos << ": ";
				out << arrv[k].x << ", " << arrv[k].y << ", " << arrv[k].z << std::endl;
				if (++count >= maxLines)
					return;
			}
		}
	}
}

void writeScatterTable(std::ostream& out, const ScatterTable& scatterTable) {
	out.write((char*)scatterTable.get(), intensity_hDim * intensity_viewDim * intensity_sunDim * sizeof(dvec3));
}

ScatterTable readScatterTable(std::istream& in, const ScatterTable& scatterTable) {
	unique_ptr<dvec3[]> singleScatT(new dvec3[intensity_hDim * intensity_viewDim * intensity_sunDim]);
	in.read((char*)singleScatT.get(), intensity_hDim * intensity_viewDim * intensity_sunDim * sizeof(dvec3));
	return singleScatT;
}