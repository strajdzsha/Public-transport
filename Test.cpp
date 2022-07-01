#include "Network.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int main() {
	Network* net = new Network();
	net->loadNetwork("stajalista.txt", "linije.txt");
	//net->printStations();
	//net->printLines();
	//net->findPath(154, 578, minTime);
	net->stationInformation(154);
	//net->lineInformation("12");
	//net->lineStats("12");
	//net->findPath(154, 578, minTime);
	cout << '\n';
	//net->printAdjMatrix();

	return 1;
}

