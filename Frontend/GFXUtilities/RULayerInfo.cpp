#include "RULayerInfo.h"
#include <iostream>

RULayerInfo::RULayerInfo(int newLayerType, int newNeuronQty, int edges)
{
	layerType = newLayerType;
	neuronQty = newNeuronQty;

	for(int n = 0; n < newNeuronQty; ++n)
	{
	    neuronList.push_back(shmea::GPointer<Neuron>(new Neuron(edges)));
	}
	
}


RULayerInfo::~RULayerInfo()
{

}

void RULayerInfo::setNeurons(int newNeuronSize, int edges)
{
	neuronQty = newNeuronSize;
	
	neuronList.clear();

	for(int n = 0; n < neuronQty; ++n)
	{
	   neuronList.push_back(shmea::GPointer<Neuron>(new Neuron(edges)));
	}
}

void RULayerInfo::setActivation(int neuron, float newActivation)
{
	neuronList[neuron]->setActivation(newActivation);
}

void RULayerInfo::setWeights(int neuron, std::vector<shmea::GPointer<float> > newWeights)
{
	neuronList[neuron]->setWeights(newWeights);
}

int RULayerInfo::getNeurons() const
{
	return neuronQty;
}

std::vector<shmea::GPointer<Neuron> > RULayerInfo::getNeuronList() const{
	return neuronList;
}

void RULayerInfo::displayLayer()
{
	printf("Neurons: %d\n", neuronQty);

	for(int n = 0; n < neuronQty; ++n)
	{
	    std::cout << "Neuron " << n + 1 << ": \n";
	    neuronList[n]->displayNeuron();
	}
	printf("\n");
}
