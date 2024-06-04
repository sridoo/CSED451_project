#include <memory>
#include <thread>
#include <vector>

#include "glm/vec3.hpp"

#include "atmParams.hpp"
#include "transmit.hpp"

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
		val += step * (last + cur) / 2.f;
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

constexpr size_t intensity_hDim = 32, intensity_viewDim = 256, intensity_sunDim = 64;
static constexpr double dintensity_hDim = intensity_hDim, dintensity_viewDim = intensity_viewDim, dintensity_sunDim = intensity_sunDim;

static constexpr double heightStep = atmThick / dintensity_hDim,
viewCosStep = 2.0 / dintensity_viewDim,
sunCosStep = 2.0 / dintensity_sunDim;

void bakeSingleScatterPart(dvec3* out, size_t from, size_t to, const TransmitTable& transmitTable) {
	for (size_t i = from; i < to; i++) {
		const double curRadius = earthRadius + static_cast<double>(i) * heightStep;
		const double curRadius2 = pow(curRadius, 2);
		dvec3* const arrh = out + i * intensity_viewDim * intensity_sunDim;

		for (size_t j = 0; j < intensity_viewDim; ++j) {
			const double curViewCos = 1 - static_cast<double>(j) * viewCosStep;
			dvec3* const arrv = arrh + j * intensity_sunDim;

			for (size_t k = 0; k < intensity_sunDim; ++k) {
				double curSunCos = 1 - static_cast<double>(k) * sunCosStep;
				arrv[k] = phaseFunRay(curSunCos) * rayScatCoef / 4.0 / PI
					* integrate3(SingleScatAtten(curSunCos, curViewCos, curRadius, transmitTable), 0, getHerizonAtmDepth(curRadius, curRadius2, curViewCos), 10.0);
			}
		}
	}
}

void bakeSingleScatterTable(const TransmitTable& transmitTable) {
	unique_ptr<dvec3[]> singleScatT(new dvec3[intensity_hDim * intensity_viewDim * intensity_sunDim]);

	size_t nThreads = thread::hardware_concurrency();
	vector<thread> threads;
	threads.reserve(nThreads);
	
	size_t baseWorkLoad = intensity_hDim / nThreads;
	size_t additionalLoadStart = intensity_hDim - baseWorkLoad * nThreads;

	for (size_t i = 0; i < nThreads; ++i) {
		threads.emplace_back(bakeSingleScatterPart,
			singleScatT.get(),
			i * blockSize,
			(i + 1) * blockSize);
	}
}