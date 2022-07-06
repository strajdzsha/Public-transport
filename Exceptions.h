#pragma once
#include <exception>
using namespace std;

class InvalidPath :public exception {
public:
	InvalidPath():exception("Greska. Putanja do fajlova ne postoji."){}
};

class StationDoesntExist : public exception {
public:
	StationDoesntExist() :exception("Greska. Kod stanice koji ste ukucali ne postoji.") {}
};

class LineDoesntExist : public exception {
public:
	LineDoesntExist() :exception("Greska. Kod linije koji ste ukucali ne postoji.") {}
};