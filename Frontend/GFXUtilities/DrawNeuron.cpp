#include "DrawNeuron.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
// Path: neuron.cpp

//generateRandomNumber is for testing only, when initializing it is likely a good idea to use 0.0f`
double generateRandomNumber()
{
    double r = static_cast <double> (rand());
    return r / RAND_MAX;
}

DrawNeuron::DrawNeuron(int newEdges)
{
	edges = newEdges;

	for(int i = 0; i < edges; i++)
	{
		weights.push_back(shmea::GPointer<float>(new float(0.0)));
	}
	activation = 0.0f;
}

DrawNeuron::~DrawNeuron()
{

}

void DrawNeuron::setActivation(float act)
{
	activation = act;
}



void DrawNeuron::setWeights(std::vector<shmea::GPointer<float> > w)
{
	weights = w;
}

float DrawNeuron::getActivation() const
{
	return activation;
}

std::vector<shmea::GPointer<float> > DrawNeuron::getWeights() const
{
	return weights;
}

void DrawNeuron::displayNeuron()
{
	std::cout << "Activation: " << activation << std::endl;
	std::cout << "Weights: ";
	for(unsigned int i = 0; i < weights.size(); i++)
	{
		std::cout << " " << *weights[i];
		if(i != weights.size() - 1)
		{
			std::cout << ",";
		}
	}
	std::cout << std::endl;
}
