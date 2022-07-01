#ifndef STATION_H
#define STATION_H

#include <string>
#include <vector>

class Station {
public:
	Station(int stationCode, const std::string& name) : stationCode(stationCode), name(name) { this->stationId = -1; };
	int getId() { return this->stationId; }
	int getCode() { return this->stationCode; }
	std::string getName() { return this->name; }
	void setId(int id) { this->stationId = id; }
	void print();
private:
	int stationId, stationCode;
	std::string name;
};

#endif
