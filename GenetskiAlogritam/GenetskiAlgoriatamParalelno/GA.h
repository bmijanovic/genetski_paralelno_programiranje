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
	int CUTOFF = 16;
	vector<Unit*> units;
	mt19937 rng;
	pair<vector<int>, vector<int>> _parentSelection(vector<pair<int, Unit*>>& parents);
	pair<vector<int>, vector<int>> _parallel_parentSelection(vector<pair<int, Unit*>>& parents);

	vector<Unit*> _breed(vector<pair<int, Unit*>> parents, int parentsSize);
	vector<Unit*> _parallel_breed(vector<pair<int, Unit*>> parents, int parentsSize);

	Unit* _mutate(Unit* child);
	Unit* _parallel_mutate(Unit* child);

	Unit* _selectPopulation();
	double rand01();

public:
	GA(int populationSize, int iterations, double mutationFactor, double elitism);
	void simulate();
};

