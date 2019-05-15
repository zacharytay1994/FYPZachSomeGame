#pragma once

#include <ctime>

class Clock {
public:
	Clock()
	{}
	void Update() {

	}
public:
	static std::clock_t begin;
private:
	int minutes;
	int second;
	int milliseconds;
};