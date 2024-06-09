#include <fstream>
#include <iostream>

#include "transmit.hpp"
#include "scattering.hpp"


using namespace std;

int main() {
	//ifstream transmitCache(".\\resource\\transmitTable.cache", ios::binary);
	//TransmitTable transmitTable(transmitCache);;
	//ofstream singleScatterTexAM(".\\resource\\singleScatterTexAM.tex", ios::binary);
	//ofstream singleScatterTexPM(".\\resource\\singleScatterTexFM.tex", ios::binary);
	//auto scatterT = bakeScatterTable(0, transmitTable);
	//writeScatterTableTex(singleScatterTexAM, singleScatterTexPM, scatterT);
	auto transmitTable = bakeTransmitTable();
	ofstream transmitCache(".\\resource\\transmitTableTex.tex", ios::binary);
	writeTransmitTableTex(transmitCache, transmitTable);
}