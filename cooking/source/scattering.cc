#include "scattering.hpp"

#include <thread>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>

#include "atmParams.hpp"
#include "transmit.hpp"

using namespace glm;
using namespace std;

template<typename T>
static const T PI = acos(-1.0);

static constexpr double dtransmitT_hDim = transmitT_hDim, dtransmitT_cosDim = transmitT_cosDim;
static constexpr size_t intensity_hDim = 32, intnesity_viewDim = 256, intensity_sunDim = 64;
static const vec3 sunLight{ 1.f, 1.f, 1.f };

class Density {
	const double dirCos_, initRadius_, initRadius2_;
public:
	Density(double dirCos, double initRadius, double initRadius2)
		: dirCos_(dirCos), initRadius_(initRadius), initRadius2_(initRadius2) {}

	double operator()(double x) {
		return displacedDensity(dirCos_, initRadius_, initRadius2_, x);
	}
};

static inline double displacedCos(double initRadius, double initRadius2, double dirCos, double x) {
	return (initRadius - dirCos * x) / displacedRaidus(initRadius, initRadius2, dirCos, x);
}

static inline double radiusToHeight(double radius) {
	return radius - earthRadius;
}
static inline size_t radiusToHeightIdx(double radius) {
	return radiusToHeight(radius) / atmThick * dtransmitT_hDim;
}
static inline size_t cosToCosIdx(double cosine) {
	return (1.0 + cosine) / 2.0 * dtransmitT_cosDim;
}

static inline double density(double dirCos, double initRadius, double initRadius2, double x) {
	return exp(-(displacedRaidus(initRadius, initRadius2, dirCos, x) - earthRadius) / atmThick);
}

static void bakeSingleScatPart(dvec3* singleScatT, size_t from, size_t end, const TransmitTable& transmitT) {

}

static unique_ptr<vec3[]> cookSingleScat(vec3* transmitT) {
	unique_ptr<vec3[]> singleScatT(new vec3[intensity_hDim * intnesity_viewDim * intensity_sunDim]);
	constexpr double heightStep = atmThick / intensity_hDim,
		viewCosStep = 2.0 / intnesity_viewDim,
		sunCosStep = 2.0 / intensity_sunDim;

	class SingleScatAtten {
		double initRadius_, initRadius2_, viewCos_, sunCos_;
		vec3& transmitInit_;
		vec3* const transmitTable_;
	public:
		SingleScatAtten(double sunCos, double viewCos, double initRadius, vec3* transmitTable)
			: initRadius_(initRadius),
			initRadius2_(pow(initRadius, 2)), viewCos_(viewCos), sunCos_(sunCos),
			transmitInit_{ transmitTable[radiusToHeightIdx(initRadius) * transmitT_cosDim + cosToCosIdx(viewCos)] },
			transmitTable_(transmitTable)
		{}

		vec3 operator()(double x) {
			return (float)density(viewCos_, initRadius_, initRadius2_, x)
				* transmitInit_
				/ transmitTable_[radiusToHeightIdx(displacedRaidus(initRadius_, initRadius2_, viewCos_, x)) + cosToCosIdx(cosSum(viewCos_, displacedCos(initRadius_, initRadius2_, viewCos_, x)))]
				* transmitTable_[radiusToHeightIdx(displacedRaidus(initRadius_, initRadius2_, viewCos_, x)) + cosToCosIdx(cosSum(sunDisplacedCos(viewCos_, initRadius_, initRadius2_, sunCos_, x), viewCos_))];
		}
	};

	for (size_t i = 0; i < intensity_hDim; ++i) {
		double curRadius = earthRadius + static_cast<double>(i) * heightStep;
		double curRadius2 = pow(curRadius, 2);

		vec3* arri = singleScatT.get() + i * intnesity_viewDim * intensity_sunDim;
		for (size_t j = 0; j < intnesity_viewDim; ++j) {
			double curViewCos = 1 - static_cast<double>(j) * viewCosStep;
			vec3* arrj = arri + j * intensity_sunDim;
			for (size_t k = 0; k < intensity_sunDim; ++k) {
				double curSunCos = 1 - static_cast<double>(k) * sunCosStep;
				arrj[k] = sunLight * (float)phaseFunRay(curSunCos) * vec3(rayScatCoef) / 4.f / PI<float> *integrate3(SingleScatAtten(curSunCos, curViewCos, curRadius, transmitT), 0, static_cast<float>(getHerizonAtmDepth(curRadius, curRadius2, curViewCos)), 5.0);
			}
		}
	}

	return singleScatT;
}

static inline dvec3 rayDisribut(double height) {
	return rayScatCoef * exp(-height / 8000);
}

template<typename Fun>
static vec3 integrate3(Fun fun, float from, float to, float step) {
	vec3 val = {}, last = fun(from);
	for (float i = from + step; i < to; i += step) {
		vec3 cur = fun(i);
		val += step * (last + cur) / 2.f;
		last = cur;
	}
	return val;
}

static inline double cosineToSine(double cosine) {
	return sqrt(1 - pow(cosine, 2));
}

static inline double cosSum(double cos1, double cos2) {
	return cos1 * cos2 - cosineToSine(cos1) * cosineToSine(cos2);
}
static inline double sunDisplacedCos(double viewCos, double initRadius, double initRadius2, double sunCos, double x) {
	return cosSum(displacedCos(initRadius, initRadius2, viewCos, x), sunCos);
}

static inline double phaseFunRay(double cosine) {
	return 3.0 / 4.0 * (1.0 + pow(cosine, 2));
}