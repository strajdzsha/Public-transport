#include "Network.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int main() {
	Network* net = new Network();
	net->loadNetwork("stajalista-test.txt", "linije-test.txt");
	//net->printStations();
	//net->printLines();
	cout << '\n';
	//net->stationInformation(0);
	net->dijkstra(0, new Time(7, 0));
	//net->lineInformation("12");
	//net->lineStats("12");
	//net->findPath(154, 578, minTime);
	//net->printAdjMatrix();

	return 1;
}

