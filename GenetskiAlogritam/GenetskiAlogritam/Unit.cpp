#include "Unit.h"

Unit::Unit()
{
	data = vector<int>(dimension);
	for (int i = 0; i < dimension; i++) 
	{
		data[i] = rand() % 2;
	}

}

Unit::Unit(vector<int> newData)
{
	data = vector<int>(dimension);
	for (int i = 0; i < dimension; i++)
	{
		data[i] = newData[i];
	}
}

int Unit::getScore()
{
	int score = 0;
	for (int i = 0; i < dimension; i++)
	{
		score += values[i] * data[i];
	}
	return score;
}

int Unit::getWeight()
{
	int weight = 0;
	for (int i = 0; i < dimension; i++)
	{
		weight += weights[i] * data[i];
	}
	return weight;
}

int Unit::getQuality()
{
	return getWeight() > maxWeight ? 0 : getScore();
}
