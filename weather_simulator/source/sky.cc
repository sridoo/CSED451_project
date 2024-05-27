#include <memory>

#include "glm/vec3.hpp"
#include "GL/glew.h"

#include"sky.hpp"
#include"cmath"


using namespace std;
using namespace glm;

template<typename T>
static const T PI = acos(-1.0);

static constexpr double atmThick = 8000;
static constexpr double earthRadius = 6371000;
static constexpr double earthRadius2 = earthRadius * earthRadius;
static constexpr double atmTopRadius = earthRadius + atmThick;
static constexpr double atmTopRaidus2 = atmTopRadius * atmTopRadius;

const static dvec3 rayScatCoef = { 5.6 * exp(-6.0), 1.35 * exp(-5.0), 3.31 * exp(-5.0) };
const static vec3 sunLight = { 1.f, 1.f, 1.f };
static inline dvec3 rayDisribut(double height) {
	return rayScatCoef * exp(-height / 8000);
}

static constexpr size_t transmitT_hDim = 512, transmitT_cosDim = 512;
static constexpr double dtransmitT_hDim = 512.0, dtransmitT_cosDim = 512.0;
static constexpr size_t intensity_hDim = 32, intnesity_viewDim = 256, intensity_sunDim = 64;

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
static inline double displacedRaidus(double initRadius, double initRadius2, double dirCos, double x) {
	return sqrt(initRadius2 + pow(x, 2) - 2 * x * initRadius * dirCos);
}
static inline double displacedCos(double initRadius, double initRadius2, double dirCos, double x) {
	return (initRadius - dirCos * x) / displacedRaidus(initRadius, initRadius2, dirCos, x);
}
static inline double sunDisplacedCos(double viewCos, double initRadius, double initRadius2, double sunCos, double x) {
	return cosSum(displacedCos(initRadius, initRadius2, viewCos, x), sunCos);
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

class Density {
	const double dirCos_, initRadius_, initRadius2_;
public:
	Density(double dirCos, double initRadius)
		: dirCos_(dirCos), initRadius_(initRadius), initRadius2_(pow(initRadius, 2)){}

	double operator()(double x) {
		return exp(-(displacedRaidus(initRadius_, initRadius2_, dirCos_, x) - earthRadius) / atmThick);
	}
};

double getHerizonAtmDepth(double curRadius, double curRaidus2, double dirCos) {
	double b = curRadius * dirCos, tmp;
	double curHeightSine2 = curRaidus2 - pow(b, 2);
	if (dirCos < -0.001)
		return b - sqrt(atmTopRaidus2 - curHeightSine2);
	else if (double tmp = earthRadius2 - curHeightSine2; tmp > 0.001)
		return b - sqrt(tmp);
	return b + sqrt(atmTopRaidus2 - curHeightSine2);
}

double phaseFunRay(double cosine) {
	return 3.0 / 4.0 * (1.0 + pow(cosine, 2));
}

static unique_ptr<vec3[]> cookTransmitT() {
	unique_ptr<vec3[]> transmitT(new vec3[transmitT_hDim * transmitT_cosDim]);	//[hight(atmThick/512 ~ atmThick), cos(angle(0, 180)), rgb]
	constexpr float heightStep = atmThick / transmitT_hDim, cosStep = 2.0 / transmitT_cosDim;

	for (size_t i = 0; i < transmitT_hDim; ++i) {
		double curRadius = earthRadius + static_cast<double>(i) * heightStep;
		double curRadius2 = pow(curRadius, 2);
		vec3* const arr = transmitT.get() + i * transmitT_cosDim;

		for (size_t j = 0; j < transmitT_cosDim; ++j) {
			double curCos = 1.0 - cosStep * static_cast<double>(j);
			double xEnd = getHerizonAtmDepth(curRadius, curRadius2, curCos);
			vec3 val = -rayScatCoef * integrate(Density(curCos, curRadius), 0.0, xEnd, 7.0);
			arr[j] = { exp(val.x), exp(val.y), exp(val.z) };
		}
	}

	return transmitT;
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
				arrj[k] = sunLight * (float)phaseFunRay(curSunCos) * vec3(rayScatCoef) / 4.f / PI<float> * integrate3(SingleScatAtten(curSunCos, curViewCos, curRadius, transmitT), 0, getHerizonAtmDepth(curRadius, curRadius2, curViewCos), 5.0);
			}
		}
	}

	return singleScatT;
}


void cookIterAtmTables() {
	auto transmitT = cookTransmitT();
	auto scatT = cookSingleScat(transmitT.get());
}