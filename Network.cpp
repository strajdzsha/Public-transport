﻿#include "Network.h"
#include <iostream>
#include <stack>

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

std::vector<int> Network::sharedLines(int id1, int id2)
{
	vector<int> sharedLines;
	for (int i = 0; i < this->lines.size(); i++)
	{
		if (this->lineStationMatrix[i][id1] == 1 && this->lineStationMatrix[i][id2]) {
			sharedLines.push_back(i);
		}
	}
	return sharedLines;
}

std::vector<int> Network::findStartingLines(int id)
{
	vector<int> startingLines;
	for (int i = 0; i < this->lines.size(); i++)
	{
		if (this->lineStationMatrix[i][id] == 1) {
			startingLines.push_back(i);
		}
	}
	return startingLines;
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

void Network::createConnectionMatrix()
{
	for (int i = 0; i < this->stations.size(); i++)
	{
		vector<StationLine*> row;
		for (int j = 0; j < this->stations.size(); j++)
		{
			if (i != j) {
				for (int k = 0; k < this->lines.size(); k++)
				{
					if (this->lineStationMatrix[k][i] == 1 && this->lineStationMatrix[k][j] == 1) {
						StationLine* tmp = new StationLine(j, this->lines[k]);
						row.push_back(tmp);
					}
				}
			}
		}
		this->connectionMatrix.push_back(row);
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
	if (adjMatrix[id1][id2] == 0) return 0;
	if (currTime != -1){
		// distance coresponds to min time of travel
		this->distanceMinTime(id1, id2, currTime);
	}
	else {
		// distance coresponds to min ammount of transfer needed between two stations
		this->distanceMinTransfer(id1, id2, currLine);
	}
}

int Network::distanceMinTime(int id1, int id2, int currTime)
{
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
	this->parents[id2]->lineID = minIdx;
	return minTime - currTime + TIME_BETWEEN_STATIONS;
}

int Network::distanceMinTransfer(int id1, int id2, Line* currLine)
{
	int currLineID = currLine->getId();
	if (this->parents[id1]->lineID == currLineID) {
		this->parents[id2]->lineID = currLineID;
		return 0;
	}
	else {
		this->parents[id2]->lineID = currLineID;
		return 1;
	}
}

vector<int> Network::shareElement(std::vector<int> a, std::vector<int> b)
{
	vector<int> res;
	for (int i = 0; i < a.size(); i++)
	{
		for (int j = 0; j < b.size(); j++)
		{
			if (a[i] == b[j]) {
				res.push_back(a[i]);
			}
		}
	}
	return res;
}

Line* Network::getPrevLine(int currID)
{
	for (int i = 0; i < this->parentsTransfer.size(); i++)
	{
		NodeTrf* tmp = this->parentsTransfer[i];
		if (tmp->dst == currID) return tmp->line;
	}
	return nullptr;
}

void Network::dijkstraMinTime(int srcCode, int endCode, Time* currTime)
{
	this->createAdjMatrix();

	int src = this->getStationId(srcCode), end = this->getStationId(endCode), V = this->stations.size();
	vector<int> dist(V);
	vector<bool> visited(V);

	for (int i = 0; i < V; i++) {
		dist[i] = INT_MAX;
		visited[i] = false;
		Node* tmp = new Node();
		this->parents.push_back(tmp);
	}

	dist[src] = 0;
	this->parents[src]->parent = -1;
	this->parents[src]->lineID = -1;

	for (int count = 0; count < V - 1; count++) {
		int u = minDistance(dist, visited);

		visited[u] = true;

		for (int v = 0; v < V; v++)
			if (!visited[v] && distanceTwoStations(u, v, currTime->addMinutes(dist[u])) && dist[u] != INT_MAX
				&& dist[u] + distanceTwoStations(u, v, currTime->addMinutes(dist[u])) < dist[v])
			{
				dist[v] = dist[u] + distanceTwoStations(u, v, currTime->addMinutes(dist[u]));
				this->parents[v]->parent = u;
			}
	}

	printPath(end);
}

void Network::dijkstraMinTransfer(int sourceCode, int endCode)
{
	this->createAdjMatrix();
	this->createConnectionMatrix();

	int src = this->getStationId(sourceCode), end = this->getStationId(endCode), V = this->stations.size();
	vector<int> dist(V);
	vector<bool> visited(V);

	for (int i = 0; i < V; i++) {
		dist[i] = INT_MAX;
		visited[i] = false;
	}

	this->addParent(-1, src, nullptr);
	dist[src] = 0;

	for (int i = 0; i < V - 1; i++) {

		int u = minDistance(dist, visited);
		visited[u] = true;

		for (int j = 0; j < this->connectionMatrix[u].size(); j++)
		{
			StationLine* curr = this->connectionMatrix[u][j];
			int v = curr->id;
			Line* currLine = curr->line;
			
			Line* prevLine = getPrevLine(u);
			int distance;
			if (currLine == prevLine) {
				distance = dist[u];
			}
			else {
				distance = dist[u] + 1;
			}
			if (distance < dist[v]) {
				dist[v] = distance;
				addParent(u, v, currLine);
			}
		}
	}

	printPath2(src, end);
}

void Network::userInterface()
{
	cout << "Dobrodosli u simulaciju gradske saobracajne mreze. Molimo Vas odaberite jednu od opcija:" << endl;
	cout << "1. Ucitavanje podataka o linijama i stajalistima" << endl;
	cout << "0. Kraj rada" << endl;
	int tmp;
	cin >> tmp;
	if (tmp == 0) return;
	else if (tmp == 1) {
		this->userInterfaceLoading();
	}
	else if (tmp == 100) {
		this->loadNetwork("stajalista.txt", "linije.txt");
		this->dijkstraMinTransfer(123, 1);
	}
	else {
		cout << "Greska: unesite validnu vrednost (1 ili 0)" << endl;
		return;
	}
}

void Network::printSolution(vector<int> dist, int src, int end)
{
	int V = this->stations.size();
	int curr = src;

	printPath(end);
}

void Network::addParent(int src, int dst, Line* l)
{
	for (int i = 0; i < this->parentsTransfer.size(); i++)
	{
		NodeTrf* tmp = this->parentsTransfer[i];
		if (tmp->dst == dst) {
			tmp->src = src;
			tmp->line = l;
			return;
		}
	}
	NodeTrf* tmp = new NodeTrf(src, dst, l);
	this->parentsTransfer.push_back(tmp);
}

void Network::printPath(int j)
{
	if (this->parents[j]->parent == -1)
		return;

	printPath(this->parents[j]->parent);

	if (this->parents[j]->lineID != this->parents[this->parents[j]->parent]->lineID) {
		cout << "\n" << "->" << this->lines[this->parents[j]->lineID]->getName() << endl;
		cout << this->stations[this->parents[j]->parent]->getCode() << " " << this->stations[j]->getCode() << " ";
	}
	else {
		cout << this->stations[j]->getCode() << " ";
	}
}

void Network::printPath2(int source, int end)
{
	int curr = end;
	stack<NodeTrf*> path;
	while (curr != source) {
		for (int i = 0; i < this->parentsTransfer.size(); i++)
		{
			NodeTrf* tmp = this->parentsTransfer[i];
			if (tmp->dst == curr) {
				path.push(tmp);
				curr = tmp->src;
			}
		}
	}
	for (int i = 0; i < path.size(); i++)
	{
		cout << path.top()->dst << " " << path.top()->line->getId() << endl;
		path.pop();
	}

}

void Network::userInterfaceLoading()
{
	cout << "Molimo Vas, unesti putanju do fajla sa stajalistima:" << endl;
	string stationPath;
	cin >> stationPath;
	string linePath;
	cout << "Molimo Vas, unesti putanju do fajla sa linijama:" << endl;
	cin >> linePath;
	this->loadNetwork(stationPath, linePath);
	cout << "Gradska mreza je uspesno ucitana." << endl;
	cout << "Molimo Vas, odaberite jednu od sledecih opcija:" << endl;
	cout << "1. Prikaz informacija o stajalistu" << endl;
	cout << "2. Prikaz osnovnih informacija o liniji gradskog prevoza" << endl;
	cout << "3. Prikaz statistickih informacija o liniji gradskog prevoza" << endl;
	cout << "4. Pronalazak putanje izmedju dva stajalista" << endl;
	cout << "0. Kraj rada" << endl;
	int tmp;
	cin >> tmp;
	switch (tmp)
	{
	case 0:
		return;
	case 1:
		this->userInterfaceStation();
		break;
	case 2:
		this->userInterfaceLine();
		break;
	case 3:
		this->userInterfaceStats();
		break;
	case 4:
		this->userInterfacePath();
		break;
	default:
		cout << "Greska: unesite neke od validnih vrednosti (0-4)." << endl;
		break;
	}
}

void Network::userInterfaceStation()
{
	cout << "Unesite sifru stajalista koje Vas interesuje:" << endl;
	int tmp;
	cin >> tmp;
	this->stationInformation(tmp);
	cout << "Informacije o stajalistu su Vam uspesno upisane u fajl stajaliste_" << tmp << endl;
}

void Network::userInterfaceLine()
{
	cout << "Unesite sifru linije koja Vas interesuje:" << endl;
	string tmp;
	cin >> tmp;
	this->lineInformation(tmp);
	cout << "Informacije o liniji su Vam uspesno upisane u fajl linija_" << tmp << endl;
}

void Network::userInterfaceStats()
{
	cout << "Unesite sifru linije koja Vas interesuje:" << endl;
	string tmp;
	cin >> tmp;
	this->lineStats(tmp);
	cout << "Informacije o liniji su Vam uspesno upisane u fajl statistika_" << tmp << endl;
}

void Network::userInterfacePath()
{
	cout << "Odaberite tip pretrage putanje:" << endl;
	cout << "1. Bilo koja putanja" << endl;
	cout << "2. Putanja sa najbrzim vremenom" << endl;
	cout << "3. Putanja sa najmanje presedanja" << endl;
	int tmp;
	cin >> tmp;

	cout << "Unesite sifru polazne stanice:" << endl;
	int src;
	cin >> src;

	cout << "Unesite sifru krajnje stanice:" << endl;
	int end;
	cin >> end;

	string time, shh, smm;
	int hh, mm;
	switch (tmp)
	{
	case 1: case 2:
		cout << "Unesite vreme dolaska na pocetnu stanicu u formatu HH:MM :" << endl;
		cin >> time;
		shh = time.substr(0, 2);
		hh = stoi(shh);
		smm = time.substr(3, 5);
		mm = stoi(smm);
		this->dijkstraMinTime(src, end, new Time(hh, mm));
		return;
	case 3:
		this->dijkstraMinTransfer(src, end);
		return;
	default:
		cout << "Greska: unesite jednu od validnih vrednosti (1-3).";
		break;
	}
}
