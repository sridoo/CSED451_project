#include <fstream>
#include <iostream>

#include "transmit.hpp"

using namespace std;

int main() {
	auto table = bakeTransmitTable();
	cout << "calc done" << endl;
	ofstream fileTransmit(".\\resource\\transmitTable.cache", std::ios::binary);
	writeTransmitTable(fileTransmit, table);
	cout << "writing done" << endl;
	ifstream fileRead(".\\resource\\transmitTable.cache", std::ios::binary);
	TransmitTable transmitTable(fileRead);
	cout << "readDone" << endl;
	writeTransmitTableReadable(cout, 1000, table);
	cout << endl;
	writeTransmitTableReadable(cout, 1000, transmitTable);
}