#include "Network.h"
#include <iostream>

using namespace std;
#define TIME_BETWEEN_STATIONS 3;

Network::Network()
{
	this->stations = std::vector<Station*>();
	this->lines = std::vector<Line*>();
}

void Network::loadNetwork(const std::string& stationsPath, const std::string& linesPath)
{
	this->loadStations(stationsPath);
	this->loadLines(linesPath);
}

std::string Network::readWord(const std::string& s, int& i, char thermChar)
{
	string word = "";
	while (i < s.length() && s[i] != thermChar && s[i] != '\n') {
		word.append(1, s[i]);
		i++;
	}
	i++;
	return word;
}

std::string Network::readWord2(const std::string& s, int& i, char thermChar)
{
	string word = "";
	while (i < s.length() && s[i] != thermChar && s[i] != '\n') {
		word.append(1, s[i]);
		i++;
	}
	if (s[i] == '\n') { return word; }
	i++;
	return word;
}

Line* Network::readLineData(const std::string& s, int& i)
{
	// Parses out information about city lines. Each row of .txt file is assumed to be in format "name [hh:mm-hh:mm#interval] <list of station codes>"
	string name = readWord(s, i, ' ');
	i++;//skiping '['
	int hour1 = stoi(readWord(s, i, ':'));
	int minute1 = stoi(readWord(s, i, '-'));
	Time* t1 = new Time(hour1, minute1);
	int hour2 = stoi(readWord(s, i, ':'));
	int minute2 = stoi(readWord(s, i, '#'));
	Time* t2 = new Time(hour2, minute2);
	int interval = stoi(readWord(s, i, ']'));
	i++;//skipping ' '

	Line* line = new Line(name, t1, t2, interval);

	return line;
}

void Network::loadStations(const std::string& stationsPath)
{
	ifstream fstations(stationsPath);
	stringstream ss_stations;
	ss_stations << fstations.rdbuf(); fstations.close();
	string sstations = ss_stations.str();

	int i = 0;
	int id = 0;
	while (i < sstations.length()) {
		int code = stoi(readWord(sstations, i, ' '));
		string name = readWord(sstations, i, '\n');
		Station* st = new Station(code, name);
		st->setId(id);
		id++;
		this->stations.push_back(st);
	}
}

void Network::loadLines(const std::string& linesPath)
{
	ifstream flines(linesPath);
	stringstream ss_lines;
	ss_lines << flines.rdbuf(); flines.close();
	string slines = ss_lines.str();

	int i = 0;
	int id = 0;
	while (i < slines.length()) {
		Line* line;
		line = this->readLineData(slines, i);
		line->setId(id);

		this->addStationsToLine(slines, i, line);
		this->lines.push_back(line);
		id++;
	}
}

void Network::addStationsToLine(const std::string& s, int& i, Line* line)
{
	vector<bool> row;
	for (int i = 0; i < this->stations.size(); i++) { row.push_back(0); }

	while (i < s.size() && s[i] != '\n') {
		int code = stoi(this->readWord2(s, i, ' '));
		int id = this->getStationId(code);
		line->addStation(this->stations[id]);
		row[id] = 1;
	}
	this->lineStationMatrix.push_back(row);
	i++;
}

int Network::getStationId(int stationCode)
{
	for (int i = 0; i < this->stations.size(); i++)
	{
		if (this->stations[i]->getCode() == stationCode) { return this->stations[i]->getId(); }
	}
}

int Network::getLineId(std::string lineName)
{
	for (int i = 0; i < this->lines.size(); i++)
	{
		if (this->lines[i]->getName() == lineName) { return this->lines[i]->getId(); }
	}
}

void Network::createEmptyAdjMatrix()
{
	for (int i = 0; i < this->stations.size(); i++)
	{
		vector<int> row;
		for (int j = 0; j < this->stations.size(); j++)
		{
			row.push_back(0);
		}
		this->adjMatrix.push_back(row);
	}
}

void Network::createAdjMatrix()
{
	this->createEmptyAdjMatrix();
	for (int i = 0; i < this->lines.size(); i++)
	{
		Line* l = this->lines[i];
		l->createEmptyAdjMatrix(this->stations.size());
		for (int j = 0; j < l->getNOfStations() - 1; j++)
		{
			int id1 = l->getStation(j)->getId();
			int id2 = l->getStation(j + 1)->getId();
			l->addConnection(id1, id2);
			l->addConnection(id2, id1);
			this->adjMatrix[id1][id2] = 1;
			this->adjMatrix[id2][id1] = 2;
		}
	}
}

void Network::printPath(std::vector<int> path)
{
	for (int i = 0; i < path.size(); i++)
	{
		cout << path[i] << " ";
	}
}

void Network::printStations()
{
	for (int i = 0; i < this->stations.size(); i++) { this->stations[i]->print(); }
}

void Network::printLines()
{
	for (int i = 0; i < this->lines.size(); i++) { this->lines[i]->print(); }
}

void Network::printAdjMatrix()
{
	for (int i = 0; i < this->stations.size(); i++)
	{
		for (int j = 0; j < this->stations.size(); j++)
		{
			cout << this->adjMatrix[i][j] << ' ';
		}
		cout << '\n';
	}
}

void Network::stationInformation(int code)
{
	string filename = "";
	filename += "stajaliste";
	filename += '_';
	filename += to_string(code);
	filename += ".txt";
	ofstream out(filename);

	string output = "";
	int id = this->getStationId(code);
	string name = this->stations[id]->getName();
	output += to_string(code);
	output += ' ';
	output += name;
	output += " [";

	for (int i = 0; i < this->lines.size(); i++)
	{
		if (this->lineStationMatrix[i][id] == 1) {
			Line* l = this->lines[i];
			output += l->getName();
			output += i < this->lines.size() - 1 ? " " : "";
		}
	}
	output += ']';
	output += '\n';

	out << output;
	out.close();
}

void Network::lineInformation(std::string lineName)
{
	string filename = "";
	filename += "linija";
	filename += '_';
	filename += lineName;
	filename += ".txt";
	ofstream out(filename);

	int lineId = this->getLineId(lineName);
	string output = "";
	output += lineName;
	output += ' ';
	Station* firstSt = this->lines[lineId]->getStation(0);
	output += firstSt->getName();
	output += "->";
	Station* lastSt = this->lines[lineId]->getStation(-1);
	output += lastSt->getName();
	output += '\n';

	for (int i = 0; i < this->stations.size(); i++)//aj zameni posle ovde this->lines[lineId]->getN...
	{
		Station* st = this->lines[lineId]->getStation(i);
		if (st == nullptr) { break; }
		output += st->getName();
		output += '\n';
	}

	out << output;
	out.close();
}

void Network::lineStats(std::string lineName)
{
	string filename = "";
	filename += "statistika";
	filename += '_';
	filename += lineName;
	filename += ".txt";
	ofstream out(filename);

	int lineId = this->getLineId(lineName);
	string output = "";
	output += lineName;
	output += '\n';
	string intersectingLines = "";
	for (int i = 0; i < this->lines.size(); i++)
	{
		if (i != lineId) {
			for (int j = 0; j < this->stations.size(); j++)
			{
				if (this->lineStationMatrix[i][j] == 1 && this->lineStationMatrix[lineId][j] == 1) {
					intersectingLines += this->lines[i]->getName();
					intersectingLines += ' ';
					break;
				}
			}
		}
	}
	output += intersectingLines;
	output += '\n';

	output += to_string(this->lines[lineId]->nOfDepartures());
	output += '\n';

	out << output;
	out.close();
}

void Network::findPath(int code1, int code2, Method m)
{
	Time* t = new Time(7, 0);
	this->createAdjMatrix();
	int id1 = this->getStationId(code1);
	int id2 = this->getStationId(code2);///**** moras obezbediti id1 < id2
	vector<bool> visited(this->stations.size(), 0);
	string path = "";
	bool found = 0;
	switch (m)
	{
	case any:
		//this->findAnyPath(id1, id2, path, visited, found);
		break;
	case minTime:
		//this->createTimeMatrix(t, id1);
		//this->findMinTimePath(id1, id2);
		break;
	case minTransfer:
		//this->findMinTransferPath(id1, id2, visited);
		break;
	default:
		break;
	}
}

int Network::minDistance(vector<int> dist, vector<bool> sptSet)
{
	int V = this->stations.size();
	// Initialize min value
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++)
		if (sptSet[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}

int Network::distanceTwoStations(int id1, int id2, int currTime, Line* currLine)
{
	if (currTime != -1){
		// distance coresponds to min time of travel
		if (this->adjMatrix[id1][id2] == 0) {
			return -1;
		}
		else {
			int minIdx = -1;
			int minTime = INT_MAX;
			for (int i = 0; i < this->lines.size(); i++)
			{
				Line* l = this->lines[i];
				if (l->areConnected(id1, id2)) {
					int res = l->closestArrival(currTime, id1, this->adjMatrix[id1][id2]);
					if (res < minTime) { minTime = res; minIdx = i; }
				}
				else {
					continue;
				}
			}
			return minTime - currTime + TIME_BETWEEN_STATIONS;
		}
	}
	else {
		// distance coresponds to min ammount of transfer needed between two stations
		if (this->adjMatrix[id1][id2] == 0) {
			return -1;
		}
		else {
			int minIdx = -1;
			int transfer = INT_MAX;
			if (currLine->areConnected(id1, id2)) {
				return 0;
			}
		}

	}
}

void Network::dijkstra(int srcCode, Time* currTime)
{
	this->createAdjMatrix();

	int src = this->getStationId(srcCode);
	int V = this->stations.size();

	vector<int> dist(V);
	vector<bool> visited(V);

	for (int i = 0; i < V; i++)
		dist[i] = INT_MAX, visited[i] = false;

	dist[src] = 0;

	for (int count = 0; count < V - 1; count++) {
		int u = minDistance(dist, visited);

		visited[u] = true;

		for (int v = 0; v < V; v++)
			if (!visited[v] && distanceTwoStations(u, v, currTime->addMinutes(dist[u])) != -1 && dist[u] != INT_MAX
				&& dist[u] + distanceTwoStations(u, v, currTime->addMinutes(dist[u])) < dist[v])
			{	
				dist[v] = dist[u] + distanceTwoStations(u, v, currTime->addMinutes(dist[u]));

			}
	}
	printSolution(dist);
}

void Network::printSolution(vector<int> dist)
{
	int V = this->stations.size();
	cout << "Vertex \t Distance from Source" << endl;
	for (int i = 0; i < V; i++)
		cout << this->stations[i]->getCode() << " \t\t" << dist[i] << endl;
}