#pragma once
#include <cstdlib>
#include <vector>

using namespace std;

class Unit
{
public:
	vector<int> data;
	static vector<int> values;
	static vector<int> weights;
	static int dimension;
	static int maxWeight;

	Unit();
	Unit(vector<int> newData);
	int getScore();
	int getWeight();
	int* getSelectedItems();
	int getQuality();
};
