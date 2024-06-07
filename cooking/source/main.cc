#include <fstream>
#include <iostream>

#include "transmit.hpp"
#include "scattering.hpp"


using namespace std;

int main() {
	ifstream transmitCache(".\\resource\\transmitTable.cache", ios::binary);
	TransmitTable transmitTable(transmitCache);
	auto scatterT = bakeScatterTable(transmitTable);
	ofstream singleScatterTex(".\\resource\\singleScatterTex.tex");
	writeScatterTableTex(singleScatterTex, scatterT);
}