#include "Unit.h"
#include <iostream>
#include "GA.h"
#include <vector>
#include <algorithm>
#include <tbb\blocked_range.h>
#include <tbb\parallel_reduce.h>

using namespace std;
using namespace tbb;

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
	cout << "Maximum value is: " << best->getQuality() << endl << "Maximum weight is: " << best->getWeight() << endl;
}

double GetTime() {
	return (double)clock() / CLOCKS_PER_SEC;
}

pair<vector<int>, vector<int>> GA::_parentSelection(vector<pair<int, Unit*>>& parents)
{
	vector<pair<int, vector<int>>> rangedParents(parents.size());
	for (int i = 0; i < parents.size(); i++)
	{
		pair<int, vector<int>> par;
		par.first = i * rand01();
		par.second = parents[i].second->data;
		rangedParents[i] = par;
	}

	int maxInd = -1, secMaxInd = -1;
	for (int i = 0; i < parents.size(); i++)
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
	pair<vector<int>, vector<int>> returnValue;
	returnValue.first = rangedParents[maxInd].second;
	returnValue.second = rangedParents[secMaxInd].second;
	return returnValue;
}

vector<Unit*> GA::_parallel_breed(vector<pair<int, Unit*>> parents, int parentsSize)
{
	task_group g;
	vector<Unit*> children;
	vector<Unit*> children1;
	vector<Unit*> children2;
	vector<Unit*> children3;
	vector<Unit*> children4;

	if (parentsSize <= GA::CUTOFF)
	{
		children = _breed(parents, parentsSize);
	}
	else
	{
		int size = parentsSize / 4;
		vector<pair<int, Unit*>>::const_iterator first = parents.begin();

		/*vector<pair<int, Unit*>> v1(first, first + size);
		vector<pair<int, Unit*>> v2(first + size, first + size * 2);
		vector<pair<int, Unit*>> v3(first + size * 2, first + size * 3);
		vector<pair<int, Unit*>> v4(first + size * 3, first + size * 4);

		g.run([&] {children1 = _parallel_breed(v1, size); });
		g.run([&] {children2 = _parallel_breed(v2, size); });
		g.run([&] {children3 = _parallel_breed(v3, size); });
		g.run([&] {children4 = _parallel_breed(v4, size); });*/

		g.run([&] {children1 = _parallel_breed(parents, size); });
		g.run([&] {children2 = _parallel_breed(parents, size); });
		g.run([&] {children3 = _parallel_breed(parents, size); });
		g.run([&] {children4 = _parallel_breed(parents, size); });
		g.wait();
	}
	children.insert(children.end(), children1.begin(), children1.end());
	children.insert(children.end(), children2.begin(), children2.end());
	children.insert(children.end(), children3.begin(), children3.end());
	children.insert(children.end(), children4.begin(), children4.end());

	return children;
}


vector<Unit*> GA::_breed(vector<pair<int, Unit*>> parents, int parentsSize)
{
	vector<Unit*> children;
	for (int i = 0; i < parentsSize / 2; i++)
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
	vector<Unit*> children = _parallel_breed(parents, parents.size());
	vector<pair<int, Unit*>> topChildren;
	for (int i = 0; i < populationSize; i++)
	{
		pair<int, Unit*> par;
		par.first = children[i]->getQuality();
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

