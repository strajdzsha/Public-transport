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

int Network::minDistance(std::vector<int>& dist, std::vector<bool> visited)
{
	int min = INT_MAX, minInd;
	
	for (int i = 0; i < this->stations.size(); i++)
	{
		if (visited[i] == false && dist[i] <= min) { min = dist[i]; minInd = i; }
	}

	return minInd;
}

void Network::createEmptyAdjMatrix()
{
	for (int i = 0; i < this->stations.size(); i++)
	{
		vector<bool> row;
		for (int j = 0; j < this->stations.size(); j++)
		{
			row.push_back(0);
		}
		this->adjMatrix.push_back(row);
	}
}

void Network::createEmptyTimeMatrix()
{
	for (int i = 0; i < this->stations.size(); i++)
	{
		vector<int> row;
		for (int j = 0; j < this->stations.size(); j++)
		{
			row.push_back(INT_MAX);
		}
		this->timeMatrix.push_back(row);
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
			this->adjMatrix[id2][id1] = 1;
		}
	}
}

void Network::createTimeMatrix(Time* t1)
{
	this->createEmptyTimeMatrix();
	for (int i = 0; i < this->stations.size(); i++)
	{
		for (int j = i; j < this->stations.size(); j++)
		{
			int minTime = INT_MAX;
			for (int k = 0; k < this->lines.size(); k++)
			{
				Line* l = this->lines[k];
				int time = INT_MAX;
				if (l->areConnected(i, j)) {
					int t2 = l->closestArrival(t1, i);
					time = t2 - t1->convertToMinutes() + TIME_BETWEEN_STATIONS;
				}
				if (time < minTime) { minTime = time; }
			}
			this->timeMatrix[i][j] = minTime;
			this->timeMatrix[j][i] = minTime;
		}
	}
}

void Network::findAnyPath(int id1, int id2, string& path, std::vector<bool>& visited, bool& found)
{
	if (found) { return; }
	visited[id1] = 1;
	string pathtmp = path;
	if (this->adjMatrix[id1][id2] == 1) {
		path += to_string(id1) + "->" + to_string(id2);
		cout << path;
		found = 1;
		//cout << this->decodePath(path);
	}
	else {
		for (int i = 0; i < this->stations.size(); i++)
		{
			if (!visited[i] && this->adjMatrix[id1][i] == 1) {
				path += to_string(id1) + "->" + to_string(i) + " ";
				findAnyPath(i, id2, path, visited, found);
				path = pathtmp;
			}
		}
	}
}

void Network::findMinTimePath(int id1, int id2)
{
	vector<int> distance;
	vector<bool> visited;
	vector<int> path;

	for (int i = 0; i < this->stations.size(); i++)
	{
		path.push_back(-1);
		distance.push_back(INT_MAX);
		visited.push_back(0);
	}
	distance[id1] = 0;

	for (int i = 0; i < this->stations.size() - 1; i++)
	{
		int u = minDistance(distance, visited);
		visited[u] = true;

		for (int j = 0; j < this->stations.size(); j++)
		{
			if (visited[j] == false && this->timeMatrix[u][j] != INT_MAX && distance[u] + this->timeMatrix[u][j] < distance[j]) { 
				path[j] = u; 
				distance[j] = distance[u] + this->timeMatrix[u][j];
				//if (j == id2) { break; }
			}
		}
	}

	//this->printPath(path);
	this->printPath(distance);
}

void Network::findMinTransferPath(int id1, int id2, std::vector<bool>& visited)
{
}

void Network::printPath(std::vector<int> path)
{
	for (int i = 0; i < path.size(); i++)
	{
		cout << path[i] << " ";
	}
}

std::string Network::decodePath(const string& path)
{
	// aj iskomentarisi i sredi ovo majke ti
	string fullPath = "";
	Line* curr = this->lines[0];
	int i = 0;
	while (i < path.size()) {
		int id1 = stoi(this->readWord(path, i, '-'));
		i++;
		int id2 = stoi(this->readWord(path, i, ' '));
		if (curr->areConnected(id1, id2)) {
			fullPath += to_string(this->stations[id1]->getCode()) + ' ';
			if (i >= path.size() - 1) { fullPath += to_string(this->stations[id2]->getCode()); }
		}
		else {
			for (int j = 0; j < this->lines.size(); j++)
			{
				if (this->lines[j]->areConnected(id1, id2)) {
					fullPath += to_string(this->stations[id1]->getCode()) + ' ';
					fullPath += '\n' + curr->getName() + "->" + this->lines[j]->getName() + '\n';
					curr = this->lines[j];
					fullPath += to_string(this->stations[id1]->getCode()) + ' ';
					break;
				}
			}
		}
	}

	return fullPath;
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

void Network::printTimeMatrix()
{
	for (int i = 0; i < this->stations.size(); i++)
	{
		for (int j = 0; j < this->stations.size(); j++)
		{
			if (this->timeMatrix[i][j] == INT_MAX)
				cout << "inf" << ' ';
			else
				cout << this->timeMatrix[i][j] << ' ';
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
		this->findAnyPath(id1, id2, path, visited, found);
		break;
	case minTime:
		this->createTimeMatrix(t);
		this->findMinTimePath(id1, id2);
		break;
	case minTransfer:
		this->findMinTransferPath(id1, id2, visited);
		break;
	default:
		break;
	}
}
