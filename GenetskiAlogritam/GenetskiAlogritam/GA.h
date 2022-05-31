#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "Unit.h"

using namespace std;

class GA
{
	int populationSize;
	int iterations;
	double mutationFactor;
	double elitism;
	vector<Unit> units;
	mt19937 rng;
	pair<vector<int>, vector<int>> _parentSelection(vector<pair<int, Unit>> &parents);
	vector<Unit> _breed(vector<pair<int, Unit>> parents);
	Unit _mutate(Unit& child);
	Unit _selectPopulation();
	double rand01();

public:
	GA(int populationSize, int iterations, double mutationFactor, double elitism);
	void simulate();
};

