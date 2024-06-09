#include <thread>
#include <vector>
#include <iostream>
#include<algorithm>

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
static inline double cosSub(double cos1, double cos2) {
	return cos1 * cos2 + cosineToSine(cos1) * cosineToSine(cos2);
}
static inline double sinSub(double cos1, double cos2) {
	return cosineToSine(cos1) * cos2 - cosineToSine(cos2) * cos1;
}
static inline double sinSum(double cos1, double cos2) {
	return cosineToSine(cos1) * cos2 + cosineToSine(cos2) * cos1;
}
static inline double phaseFunRaySingle(double cosine) {
	return 3.0 / 4.0 * (1.0 + pow(cosine, 2));
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


class SingleScatAttenPM {
	double initRadius_, initRadius2_, viewCos_, sunCos_, sinViewSubSun_, sinSun_;
	const TransmitTable& transmitTable_;

	inline bool earthShadowCond(double x) {
		return false;
		return sunCos_ < 0.0001 && initRadius_ * sinSun_ + x * sinViewSubSun_ < earthRadius + 1.0;
	}

public:
	SingleScatAttenPM(double sunCos, double viewCos, double initRadius, double initRaidus2,
		double sinViewSubSun, double sinSun,
		const TransmitTable& transmitTable)
		: initRadius_(initRadius),
		initRadius2_(initRaidus2), viewCos_(viewCos),
		sunCos_(sunCos), sinViewSubSun_(sinViewSubSun), sinSun_(sinSun),
		transmitTable_{ transmitTable }
	{}

	dvec3 operator()(double x) {
		if (earthShadowCond(x))
			return {};

		double newRaidus = displacedRaidus(initRadius_, initRadius2_, viewCos_, x),
			displacedCosine = displacedCos(initRadius_, initRadius2_, viewCos_, x);
		
		return displacedDensity(viewCos_, initRadius_, initRadius2_, x)
			/ transmitTable_(newRaidus, cosSum(viewCos_, displacedCosine))
			* transmitTable_(newRaidus, -cosSum(displacedCosine, sunCos_));
	}
};

class SingleScatAttenAM {
	double initRadius_, initRadius2_, viewCos_, sunCos_, sinViewSumSun_, sinSun_;
	const TransmitTable& transmitTable_;

	inline bool earthShadowCond(double x) {
		return false;
		return sunCos_ < 0.0001 && initRadius_ * sinSun_ + x * sinViewSumSun_ < earthRadius + 1.0;
	}

public:
	SingleScatAttenAM(double sunCos, double viewCos, double initRadius, double initRaidus2,
		double sinViewSumSun, double sinSun,
		const TransmitTable& transmitTable)
		: initRadius_(initRadius),
		initRadius2_(initRaidus2), viewCos_(viewCos),
		sunCos_(sunCos), sinViewSumSun_(sinViewSumSun), sinSun_(sinSun),
		transmitTable_{ transmitTable }
	{}

	dvec3 operator()(double x) {
		if (earthShadowCond(x))
			return {};

		double newRaidus = displacedRaidus(initRadius_, initRadius2_, viewCos_, x),
			displacedCosine = displacedCos(initRadius_, initRadius2_, viewCos_, x);

		return displacedDensity(viewCos_, initRadius_, initRadius2_, x)
			/ transmitTable_(newRaidus, cosSum(viewCos_, displacedCosine))
			* transmitTable_(newRaidus, -cosSub(displacedCosine, sunCos_));
	}
};

static constexpr double dintensity_hDim = intensity_hDim, dintensity_viewDim = intensity_viewDim, dintensity_sunDim = intensity_sunDim;

static constexpr double heightStep = atmThick / dintensity_hDim,
viewCosStep = 2.0 / dintensity_viewDim,
sunCosStep = 2.0 / dintensity_sunDim;

static void bakeSingleScatterPart(dvec3* am, dvec3* pm, double* maxVal_, size_t from, size_t to, const TransmitTable* const transmitTable_) {
	const TransmitTable& transmitTable = *transmitTable_;
	double& maxVal = *maxVal_;
	maxVal = 0;

	while (from < to) {
		double const curViewCos = 1.0 - static_cast<double>(from) * viewCosStep;
		dvec3* const arrAmV = am + from * intensity_hDim * intensity_sunDim;
		dvec3* const arrPmV = pm + from * intensity_hDim * intensity_sunDim;

		for (size_t j = 0; j < intensity_sunDim; ++j) {
			double const curSunCos = 1.0 - static_cast<double>(j) * sunCosStep;
			double const sunSin = cosineToSine(curSunCos);

			dvec3 const head = 0.25 / PI * rayScatCoef;

			dvec3 const phasePmV = head * phaseFunRaySingle(-cosSub(curViewCos, curSunCos));
			dvec3* const arrPmSun = arrPmV + j * intensity_hDim;
			double const sinViewSubSun = sinSub(curViewCos, curSunCos);

			dvec3 const phaseAmV = head * phaseFunRaySingle(-cosSum(curViewCos, curSunCos));
			double const sinViewSumSun = sinSum(curViewCos, curSunCos);
			dvec3* const arrAmSun = arrAmV + j * intensity_hDim;

			for (size_t k = 0; k < intensity_hDim; ++k) {
				double curRadius = earthRadius + static_cast<double>(k) * heightStep;
				double curRadius2 = pow(curRadius, 2);
				const dvec3& horizenTransmit = transmitTable(curRadius, curViewCos);
				double integrateRange = getHerizonAtmDepth(curRadius, curRadius2, curViewCos);
				arrPmSun[k] = phasePmV * horizenTransmit
					* integrate3(SingleScatAttenPM(curSunCos, curViewCos, curRadius, curRadius2, sinViewSubSun, sunSin, transmitTable), 0, integrateRange, 15.0);
				arrAmSun[k] = phaseAmV * horizenTransmit
					* integrate3(SingleScatAttenAM(curSunCos, curViewCos, curRadius, curRadius2, sinViewSumSun, sunSin, transmitTable), 0, integrateRange, 15.0);
				maxVal = std::max({ maxVal, arrAmSun[k].x, arrAmSun[k].y, arrAmSun[k].z, arrPmSun[k].x, arrPmSun[k].y, arrPmSun[k].z });
			}

		}
		from++;
	}
}

static ScatterTable bakeSingleScatterTable(const TransmitTable& transmitTable) {
	ScatterTable singleScatT;

	size_t nThreads = thread::hardware_concurrency();
	vector<thread> threads;
	threads.reserve(nThreads);
	vector<double> maxes(nThreads);
	cout << "Calculating singe scattering..." << endl;
	size_t baseWorkLoad = intensity_viewDim / nThreads;
	size_t leftLoad = intensity_viewDim - baseWorkLoad * nThreads;
	for (size_t i = 0, j = 0; i < nThreads; i++) {
		size_t next = j + baseWorkLoad + (i < leftLoad);
		threads.emplace_back(bakeSingleScatterPart, singleScatT.first.get(), singleScatT.second.get(), &maxes[i], j, next, &transmitTable);
		j = next;
	}

	for (size_t i = 0; i < nThreads; ++i) {
		threads[i].join();
		singleScatT.maxVal = std::max(singleScatT.maxVal, maxes[i]);
	}
	cout << "Calculation done." << endl;
	return singleScatT;
}

ScatterTable bakeScatterTable(size_t scatterLevel, const TransmitTable& transmitTable) {
	ScatterTable accum = bakeSingleScatterTable(transmitTable);
	if (!scatterLevel)
		return accum;

	return accum;
}

static void toRGBPart(GLubyte* amOut, dvec3* amIn, GLubyte* pmOut, dvec3* pmIn, double maxVal, size_t from, size_t to) {
	while (from < to) {
		GLubyte* rgbElem = amOut + 3 * from;
		rgbElem[0] = static_cast<GLubyte>(std::min(amIn[from].x * 254.0 / maxVal, 255.0));
		rgbElem[1] = static_cast<GLubyte>(std::min(amIn[from].y * 254.0 / maxVal, 255.0));
		rgbElem[2] = static_cast<GLubyte>(std::min(amIn[from].z * 254.0 / maxVal, 255.0));

		rgbElem = pmOut + 3 * from;
		rgbElem[0] = static_cast<GLubyte>(std::min(pmIn[from].x * 254.0 / maxVal, 255.0));
		rgbElem[1] = static_cast<GLubyte>(std::min(pmIn[from].y * 254.0 / maxVal, 255.0));
		rgbElem[2] = static_cast<GLubyte>(std::min(pmIn[from].z * 254.0 / maxVal, 255.0));
		from++;
	}
}

void writeScatterTableTex(ostream& amOut, ostream& pmOut, const ScatterTable& scatterTable) {
	unique_ptr<GLubyte[]> rgbValAM(new GLubyte[nIntensityElems * 3]);
	unique_ptr<GLubyte[]> rgbValPM(new GLubyte[nIntensityElems * 3]);


	size_t nThreads = thread::hardware_concurrency();
	vector<thread> threads;
	threads.reserve(nThreads);

	size_t baseWorkLoad = nIntensityElems / nThreads;
	size_t leftLoad = nIntensityElems - baseWorkLoad * nThreads;

	for (size_t i = 0, j = 0; i < nThreads; i++) {
		size_t next = j + baseWorkLoad + (i < leftLoad);
		threads.emplace_back(toRGBPart, rgbValAM.get(), scatterTable.first.get(), rgbValPM.get(), scatterTable.second.get(), 1/30.0, j, next);
		j = next;
	}

	for (auto& i : threads)
		i.join();

	amOut.write((const char*)rgbValAM.get(), nIntensityElems * 3);
	pmOut.write((const char*)rgbValPM.get(), nIntensityElems * 3);
}

static void writeScatterTReadable_h(ostream& out, size_t maxLines, const unique_ptr<glm::dvec3[]>& tpart) {
	size_t count = 0;
	for (size_t i = 0; i < intensity_sunDim; ++i) {
		const double curSunCos = 1 - static_cast<double>(i) * sunCosStep;
		dvec3* const arrh = tpart.get() + i * intensity_viewDim * intensity_hDim;
		out << "SunCosine=" << curSunCos << endl;

		for (size_t j = 0; j < intensity_hDim; j++) {
			const double curRadius = earthRadius + static_cast<double>(i) * heightStep;
			const double curRadius2 = pow(curRadius, 2);

			out << "Height=" << curRadius - earthRadius << endl;

			dvec3* const arrv = arrh + j * intensity_viewDim;

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

void writeScatterTReadable(std::ostream& out, size_t maxLines, const ScatterTable& scatterTable) {
	cout << "AM: " << endl;
	writeScatterTReadable_h(out, maxLines, scatterTable.first);
	cout << endl << "PM: " << endl;
	writeScatterTReadable_h(out, maxLines, scatterTable.second);
}

void writeScatterTable(ostream& amOut, ostream& pmOut, const ScatterTable& scatterTable) {
	amOut.write((char*)scatterTable.first.get(), nIntensityElems * sizeof(dvec3));
	pmOut.write((char*)scatterTable.second.get(), nIntensityElems * sizeof(dvec3));
}

ScatterTable readScatterTable(std::istream& amIn, istream& pmIn) {
	ScatterTable singleScatT;
	amIn.read((char*)singleScatT.first.get(), nIntensityElems * sizeof(dvec3));
	amIn.read((char*)singleScatT.second.get(), nIntensityElems * sizeof(dvec3));
	return singleScatT;
}

ScatterTable::ScatterTable()
	: first(new dvec3[nIntensityElems]), second(new dvec3[nIntensityElems]), maxVal(0) {}