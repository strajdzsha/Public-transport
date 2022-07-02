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

class Network {
public:
	Network();
	void loadNetwork(const std::string& stationsPath, const std::string& linesPath);
	void printStations();
	void printLines();
	void printAdjMatrix();
	void stationInformation(int code);
	void lineInformation(std::string lineName);
	void lineStats(std::string lineName);
	void findPath(int code1, int code2, Method m);
	void dijkstra(int sourceCode, Time* currTime = NULL);
private:
	//Data
	std::vector<Station*> stations;
	std::vector<Line*> lines;
	std::vector<std::vector<bool>> lineStationMatrix;
	std::vector<std::vector<int>> adjMatrix; //adjacency matrix
	std::vector<std::vector<int>> timeMatrix;
	std::vector<int> timeOfArrivalArray;
	//Loading functions
	std::string readWord(const std::string& s, int& i, char thermChar);
	std::string readWord2(const std::string& s, int& i, char thermChar);
	Line* readLineData(const std::string& s, int& i);
	void loadStations(const std::string& stationsPath);
	void loadLines(const std::string& linesPath);
	void addStationsToLine(const std::string& s, int& i, Line* line);
	//Other
	int getStationId(int stationCode);
	int getLineId(std::string lineName);
	void createEmptyAdjMatrix();
	void createAdjMatrix();
	void printPath(std::vector<int> path);
	std::string decodePath(const std::string& path);
	int minDistance(std::vector<int> dist, std::vector<bool> sptSet);
	int distanceTwoStations(int id1, int id2, int currTime = -1, Line* currLine = NULL);
	void printSolution(std::vector<int> dist);
};

#endif
