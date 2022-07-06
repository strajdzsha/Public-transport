#include "Network.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int main() {
	try {
		Network* net = new Network();
		net->userInterface();
	}
	catch (InvalidPath err) {
		return -3;
	}
	catch (StationDoesntExist err) {
		return -3;
	}
	catch (LineDoesntExist err) {
		return -3;
	}

	return 1;
}

