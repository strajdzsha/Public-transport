#ifndef LINE_H
#define LINE_H

#include <vector>
#include <string>
#include "Station.h"

class Time {
public:
	Time() { this->hour = 0; this->minute = 0; }
	Time(int hour, int minute) : hour(hour), minute(minute) { };
	Time*& operator + (const Time&  obj);
	int addMinutes(int minutes);
	int convertToMinutes() { return this->hour * 60 + this->minute; }
	void print();
private:
	int hour, minute;
};


class Line {
public:
	Line(const std::string& lineName, Time* firstDeparture, Time* lastDeparture, int interval) : lineName(lineName), firstDeparture(firstDeparture), lastDeparture(lastDeparture), interval(interval) { };
	std::string getName() { return this->lineName; }
	Station* getStation(int id);
	int closestArrival(int t, int id, int direction);
	int getArrivalTime(int stationId, int direction);
	int getId() { return this->id; }
	int getNOfStations() { return this->stations.size(); }
	int nOfDepartures() { return (this->lastDeparture->convertToMinutes() - this->firstDeparture->convertToMinutes()) / this->interval + 1; }
	bool areConnected(int id1, int id2) { return this->adjMatrix[id1][id2]; }
	bool containsStation(int id);
	void setId(int id) { this->id = id; }
	void addStation(Station* s) { this->stations.push_back(s); }
	void addConnection(int id1, int id2) { this->adjMatrix[id1][id2] = 1; }
	void createEmptyAdjMatrix(int size);
	void print();
	std::vector<int> getStationIds();
private:
	std::string lineName;
	std::vector<Station*> stations;
	std::vector<std::vector<bool>> adjMatrix;
	Time* firstDeparture,* lastDeparture;
	int interval, id;
};

#endif
