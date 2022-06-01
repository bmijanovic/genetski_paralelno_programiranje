#include "Unit.h"
#include <iostream>
#include "GA.h"
#include <vector>
#include <algorithm>
using namespace std;

bool sortByFirstAsc(const pair<int, Unit*>& a,
	const pair<int, Unit*>& b)
{
	return (a.first < b.first);
}

bool sortByFirstDesc(const pair<int, Unit*>& a,
	const pair<int, Unit*>& b)
{
	return (a.first > b.first);
}

GA::GA(int populationSize = 1000, int iterations = 100, double mutationFactor = 0.1, double elitism = 0.01)
{
	this->iterations = iterations;
	this->mutationFactor = mutationFactor;
	this->elitism = elitism;
	this->populationSize = populationSize;
	this->units = vector<Unit*>(populationSize);
	for (int i = 0; i < populationSize; i++)
	{
		this->units[i] = new Unit();
	}
	this->rng = mt19937(chrono::system_clock::now().time_since_epoch().count());
}

void GA::simulate()	
{
	Unit* best = new Unit();
	for (int i = 0; i < iterations; i++)
	{
		cout << "Iteration start: " << i << endl;
		best = _selectPopulation();
		cout << "Iteration end" << endl;
	}
	cout << best->getQuality() << endl << best->getWeight() << endl;
}

double GetTime(){
	return (double)clock() / CLOCKS_PER_SEC;
}

double totalRand = 0;
double totalSort = 0;

pair<vector<int>, vector<int>> GA::_parentSelection(vector<pair<int, Unit*>> &parents)
{
	vector<pair<int, vector<int>>> rangedParents(populationSize);
	double start = GetTime();
	for (int i = 0; i < populationSize; i++)
	{
		pair<int, vector<int>> par;
		par.first = i * rand01();
		par.second = parents[i].second->data;
		rangedParents[i] = par;
	}
	totalRand += GetTime() - start;
	//cout << GetTime() - start << endl;
	start = GetTime();
	//sort(rangedParents.begin(), rangedParents.end(), sortByFirstAsc);
	int maxInd = -1, secMaxInd = -1;
	for (int i = 0; i < populationSize; i++)
	{
		if (maxInd == -1 || rangedParents[i].first > rangedParents[maxInd].first)
		{
			secMaxInd = maxInd;
			maxInd = i;
		}
		else if (secMaxInd == -1 || rangedParents[i].first > rangedParents[secMaxInd].first)
		{
			secMaxInd = maxInd;
		}
	}
	totalSort += GetTime() - start;
	//cout << GetTime() - start << endl;
	pair<vector<int>, vector<int>> returnValue;
	//returnValue.first = rangedParents[populationSize - 1].second;
	//returnValue.second = rangedParents[populationSize - 2].second;
	returnValue.first = rangedParents[maxInd].second;
	returnValue.second = rangedParents[secMaxInd].second;
	return returnValue;
}

vector<Unit*> GA::_breed(vector<pair<int, Unit*>> parents)
{
	vector<Unit*> children;
	//pair<vector<int>, vector<int>> parentsData = _parentSelection(parents);
	for (int i = 0; i < populationSize / 2; i++)
	{
		pair<vector<int>, vector<int>> parentsData = _parentSelection(parents);
		int splitPosition = (rng() % Unit::dimension);
		auto beginFirst = parentsData.first.begin();
		auto endFirst = parentsData.first.end();
		auto beginSecond = parentsData.second.begin();
		auto endSecond = parentsData.second.end();

		vector<int> childData1(beginFirst, beginFirst + splitPosition);
		vector<int> childData2(beginSecond, beginSecond + splitPosition);

		childData1.insert(childData1.end(), beginSecond + splitPosition, endSecond);
		childData2.insert(childData2.end(), beginFirst + splitPosition, endFirst);

		Unit* child1 = new Unit(childData1);
		Unit* child2 = new Unit(childData2);
	
		children.push_back(_mutate(child1));
		children.push_back(_mutate(child2));
	}
	//cout << "Rand " << totalRand << endl;
	//cout << "Sort " << totalSort << endl;
	return children;
}

double GA::rand01()
{
	return (double)(rng() % 1000000) / 1000000;
}

Unit* GA::_mutate(Unit* child)
{
	for (int i = 0; i < Unit::dimension; i++)
	{
		if (rand01() < mutationFactor)
			child->data[i] = 1 - child->data[i];
	}
	return child;
}



Unit* GA::_selectPopulation()
{
	vector<pair<int, Unit*>> parents(populationSize);
	for (int i = 0; i < populationSize; i++)
	{
		pair<int, Unit*> par;
		par.first = this->units[i]->getQuality();
		par.second = this->units[i];
		parents[i] = par;
	}
	sort(parents.begin(), parents.end(), sortByFirstAsc);
	vector<Unit*> children = _breed(parents);
	vector<pair<int, Unit*>> topChildren;
	for (int i = 0; i < populationSize; i++)
	{
		pair<int, Unit*> par;
		par.first =  children[i]->getQuality();
		par.second = children[i];
		topChildren.push_back(par);
	}
	sort(topChildren.begin(), topChildren.end(), sortByFirstDesc);
	vector<pair<int, Unit*>> topParents;
	int numOfTopParents = populationSize * elitism;
	for (int i = parents.size() - numOfTopParents; i < parents.size(); i++)
		topParents.push_back(parents[i]);
	units.clear();
	for (int i = 0; i < numOfTopParents; i++)
	{
		if (topParents[i].first > topChildren[0].first)
			units.push_back(topParents[i].second);
	}
	int numOdTopChildren = populationSize - units.size();
	for (int i = 0; i < numOdTopChildren; i++)
	{
		units.push_back(topChildren[i].second);
	}
	return units[0];
}

