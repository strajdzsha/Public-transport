#include "Station.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void Station::print()
{
	cout << this->stationCode << ":" << this->name << endl;
}
