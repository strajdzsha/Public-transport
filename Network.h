#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "Line.h"
#include "Station.h"

/// <summary>
/// This is main class. It coordinates between classes Line and Station. 
/// </summary>

enum Method {any, minTime, minTransfer};

struct Node {
	int parent, lineID;
};

class Network {
public:
	Network();
	void loadNetwork(const std::string& stationsPath, const std::string& linesPath);
	void stationInformation(int code);
	void lineInformation(std::string lineName);
	void lineStats(std::string lineName);
	void findPath(int code1, int code2, Method m);
	void dijkstra(int sourceCode, int endCode, Time* currTime = NULL);
	void userInterface();
private:
	//Data
	std::vector<Station*> stations;
	std::vector<Line*> lines;
	std::vector<std::vector<bool>> lineStationMatrix;
	std::vector<std::vector<int>> adjMatrix; //adjacency matrix
	std::vector<Node*> parents;
	//Loading functions
	std::string readWord(const std::string& s, int& i, char thermChar);
	std::string readWord2(const std::string& s, int& i, char thermChar);
	Line* readLineData(const std::string& s, int& i);
	void loadStations(const std::string& stationsPath);
	void loadLines(const std::string& linesPath);
	void addStationsToLine(const std::string& s, int& i, Line* line);
	//User interaction
	void userInterfaceLoading();
	void userInterfaceStation();
	void userInterfaceLine();
	void userInterfaceStats();
	void userInterfacePath();
	//Other
	int getStationId(int stationCode);
	int getLineId(std::string lineName);
	int minDistance(std::vector<int> dist, std::vector<bool> sptSet);
	int distanceTwoStations(int id1, int id2, std::vector<Line*>& lastLine, int currTime = -1);
	void createEmptyAdjMatrix();
	void createAdjMatrix();
	void printPath(int j);
	void printSolution(std::vector<int> dist, int src, int end);
	std::string decodePath(const std::string& path);
	Line* findStartingLine(int src);
};


#endif
