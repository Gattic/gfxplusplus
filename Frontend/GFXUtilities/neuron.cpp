#include "neuron.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
// Path: neuron.cpp

double generateRandomNumber()
{
    double r = static_cast <double> (rand());
    return r / RAND_MAX;
}

//generateRandomNumber is for testing only, when initializing it is likely a good idea to use 0.0f`
Neuron::Neuron(int newEdges)
{
	edges = newEdges;
	activation = generateRandomNumber();

	for(int i = 0; i < edges; i++)
	{
		weights.push_back(shmea::GPointer<float>(new float(generateRandomNumber())));
	}
}

Neuron::~Neuron()
{

}

void Neuron::setActivation(float act)
{
	activation = act;
}

void Neuron::setWeights(std::vector<shmea::GPointer<float> > w)
{
	weights = w;
}

float Neuron::getActivation() const
{
	return activation;
}

std::vector<shmea::GPointer<float> > Neuron::getWeights() const
{
	return weights;
}

void Neuron::displayNeuron()
{
	std::cout << "Activation: " << activation << std::endl;
	std::cout << "Weights: ";
	for(int i = 0; i < weights.size(); i++)
	{
		std::cout << " " << *weights[i];
		if(i != weights.size() - 1)
		{
			std::cout << ",";
		}
	}
	std::cout << std::endl;
}
