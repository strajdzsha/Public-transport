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
	void printTimeMatrix();
	void stationInformation(int code);
	void lineInformation(std::string lineName);
	void lineStats(std::string lineName);
	void findPath(int code1, int code2, Method m);
private:
	//Data
	std::vector<Station*> stations;
	std::vector<Line*> lines;
	std::vector<std::vector<bool>> lineStationMatrix;
	std::vector<std::vector<bool>> adjMatrix; //adjacency matrix
	std::vector<std::vector<int>> timeMatrix;
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
	int minDistance(std::vector<int>& dist, std::vector<bool> visited);
	void createEmptyAdjMatrix();
	void createEmptyTimeMatrix();
	void createAdjMatrix();
	void createTimeMatrix(Time* t1);
	void findAnyPath(int id1, int id2, std::string& path, std::vector<bool>& visited, bool& found);
	void findMinTimePath(int id1, int id2);
	void findMinTransferPath(int id1, int id2, std::vector<bool>& visited);
	void printPath(std::vector<int> path);
	std::string decodePath(const std::string& path);
};

#endif
