#include "Line.h"
#include <iostream>
constexpr auto MAX_HOURS = 24;
constexpr auto MAX_MINUTES = 60;
#define TIME_BETWEEN_STATIONS 3;

using namespace std;

Time*& Time::operator+(const Time& t) {
	Time* res = new Time(0,0);
	if (this->hour + t.hour >= MAX_HOURS) {
		res->hour = this->hour + t.hour - MAX_HOURS;
	}
	else {
		res->hour = this->hour + t.hour;
	}
	if (this->minute + t.minute >= MAX_MINUTES) {
		res->minute = this->minute + t.minute - MAX_MINUTES;
	}
	else {
		res->minute = this->minute + t.minute;
	}
	return res;
}

void Time::print() {
	std::cout << this->hour << ":" << this->minute;
}

void Line::print() {
	cout << this->lineName << endl;
}

Station* Line::getStation(int id) {
	if (id == -1) {
		return this->stations.back();
	}
	if (id >= this->stations.size()) {
		return nullptr;
	}
	return this->stations[id];
}

int Line::getArrivalTime(int stationId)
{
	for (int i = 0; i < this->stations.size(); i++)
	{
		if (this->stations[i]->getId() == stationId)
			return this->firstDeparture->convertToMinutes() + i * TIME_BETWEEN_STATIONS;
	}
}

int Line::closestArrival(Time* t, int id) {
	int time = t->convertToMinutes();
	int res = this->getArrivalTime(id);
	//int res = this->firstDeparture->convertToMinutes();
	while (res < time) {
		res += this->interval;
	}
	return res;
}

void Line::createEmptyAdjMatrix(int size)
{
	for (int i = 0; i < size; i++)
	{
		vector<bool> row;
		for (int j = 0; j < size; j++)
		{
			row.push_back(0);
		}
		this->adjMatrix.push_back(row);
	}
}
