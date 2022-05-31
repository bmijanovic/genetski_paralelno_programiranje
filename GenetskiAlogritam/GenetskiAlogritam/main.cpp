#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include "GA.h"
#include "Unit.h"

using namespace std;

void loadData(int *maxWeight, list<pair<int, int>> *objects)
{
	ifstream infile;
	string firstLine;
	infile.open("data_knapsack01.txt");
	getline(infile, firstLine, ' ');
	getline(infile, firstLine);
	*maxWeight = stoi(firstLine);
	string line;
	while(getline(infile, line))
	{
		size_t pos = 0;
		string tokeni[2];
		unsigned int i = 0;
		while ((pos = line.find(",")) != std::string::npos) {
			tokeni[i++] = line.substr(0, pos);
			line.erase(0, pos + 1);
		}
		tokeni[i] = line;
		pair<int, int> object;
		object.first = stoi(tokeni[0]);
		object.second = stoi(tokeni[1]);
		objects->push_back(object);
	}

}

vector<int> Unit::values;
vector<int> Unit::weights;
int Unit::maxWeight;
int Unit::dimension;

int main()
{
	int maxWeight;
	list<pair<int, int>> objects;
	cout << "Load start" << endl;
	loadData(&maxWeight, &objects);
	cout << "Load end" << endl;
	int size = objects.size();
	vector<int> values(size);
	vector<int> weights(size);
	list<pair<int, int>>::iterator it;
	int i = 0;

	vector<int> dp(maxWeight + 1, 0);
	for(it = objects.begin(); it != objects.end(); it++)
	{
		weights[i] = it->first;
		values[i] = it->second;
		for (int j = maxWeight; j >= weights[i]; j--) 
		{
			dp[j] = max(dp[j], dp[j - weights[i]] + values[i]);
		}
		cout << weights[i] << " " << values[i] << " " << dp[maxWeight] << endl;
		i++;
	}

	int bestWeight = maxWeight;
	while (bestWeight > 0 && dp[bestWeight - 1] == dp[maxWeight])
		bestWeight--;
	cout << "Dinamicko: " << dp[maxWeight] << " " << bestWeight << endl;

	Unit::values = values;
	Unit::weights = weights;
	Unit::maxWeight = maxWeight;
	Unit::dimension = size;

	GA ga(1000, 200, 0.1, 0.01);
	ga.simulate();

	return 0;
}