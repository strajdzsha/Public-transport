#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "Line.h"
#include "Station.h"
#include "Exceptions.h"

/// <summary>
/// This is main class. It coordinates between classes Line and Station. 
/// Responsible for user interface and all functionalities it requires.
/// </summary>

enum Method {any, minTime, minTransfer};

struct Node {
	Node() {
		this->parent = -1;
		this->lineID = -1;
	}

	Node(int parent, int lineID) {
		this->parent = parent;
		this->lineID = lineID;
	}
	int parent, lineID;
};

struct StationLine {
	StationLine(int id, Line* line) : id(id), line(line) {}
	int id;
	Line* line;
};

struct NodeTrf {
	NodeTrf(int src, int dst, Line* line) : src(src), dst(dst), line(line) {}
	int src, dst;
	Line* line;
};

class Network {
public:
	Network();
	void userInterface();
private:
	//Data
	std::vector<Station*> stations;
	std::vector<Line*> lines;
	std::vector<std::vector<bool>> lineStationMatrix;
	std::vector<std::vector<int>> adjMatrix; //adjacency matrix
	std::vector<std::vector<StationLine*>> connectionMatrix;
	std::vector<Node*> parents;
	std::vector<NodeTrf*> parentsTransfer;
	//Main functionalities
	void loadNetwork(const std::string& stationsPath, const std::string& linesPath);
	void stationInformation(int code);
	void lineInformation(std::string lineName);
	void lineStats(std::string lineName);
	void dijkstraMinTime(int sourceCode, int endCode, Time* currTime = NULL);
	void dijkstraMinTransfer(int sourceCode, int endCode);
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
	std::vector<int> findStartingLines(int id);
	Line* getPrevLine(int currID);
	int getStationId(int stationCode);
	int getLineId(std::string lineName);
	int minDistance(std::vector<int> dist, std::vector<bool> sptSet);
	int distanceTwoStations(int id1, int id2, int currTime = -1, Line* currLine = nullptr);
	int distanceMinTime(int id1, int id2, int currTime);
	int distanceMinTransfer(int id1, int id2, Line* currLine);
	void createEmptyAdjMatrix();
	void createAdjMatrix();
	void createConnectionMatrix();
	void writePath(int j, int source, std::ofstream& of, std::vector<Node*> parents);
	void addParent(int src, int dst, Line* l);
};


#endif
