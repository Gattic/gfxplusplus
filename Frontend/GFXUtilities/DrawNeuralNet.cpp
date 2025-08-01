#include "DrawNeuralNet.h"
#include "RULayerInfo.h"
#include "SDL2/SDL.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

DrawNeuralNet::DrawNeuralNet()
{
	layers = 0;
	
}

DrawNeuralNet::DrawNeuralNet(int newLayers)
{
	layers = newLayers;
	int layerType = RULayerInfo::INPUT;
	for(int i = 0; i < newLayers; ++i)
	{
	    if(i > 0 && i != newLayers - 1)
	    {
		layerType = RULayerInfo::HIDDEN;
	    }
	    else if(i == newLayers - 1)
	    {
		layerType = RULayerInfo::OUTPUT;
	    }

	    //Generate each layer for the neurons, and later edit each layer based on user input
	    //layerInfoList.push_back(shmea::GPointer<RULayerInfo>(new RULayerInfo(layerType, 2)));

	    if (i == 0)
		layerInfoList.push_back(shmea::GPointer<RULayerInfo>(new RULayerInfo(layerType, 0)));
	    else
		layerInfoList.push_back(shmea::GPointer<RULayerInfo>(new RULayerInfo(layerType, 0, layerInfoList[i - 1]->getNeurons())));

	}
}

DrawNeuralNet::~DrawNeuralNet()
{
//	delete layerInfoList;


	layers = 0;

}

void DrawNeuralNet::setInputLayer(int newNeurons)
{
	layerInfoList[0]->setNeurons(newNeurons);

}


//Does nothing if you give it index of 0 or the last index in the list
//When a hidden layer gets updated, all the layers after it need to be updated as well
void DrawNeuralNet::setHiddenLayer(int index, int newNeurons)
{
    if(index == 0 || index == layers - 1)
	return;
    
    layerInfoList[index]->setNeurons(newNeurons, layerInfoList[index - 1]->getNeurons());


    for(int i = index+1; i < layers; ++i)
    {
	layerInfoList[i]->setNeurons(layerInfoList[i]->getNeurons(), layerInfoList[i - 1]->getNeurons());
    }
}

void DrawNeuralNet::setOutputLayer(int newNeurons)
{
	layerInfoList[layers - 1]->setNeurons(newNeurons, layerInfoList[layers - 2]->getNeurons());
}

void DrawNeuralNet::setActivation(shmea::GList activations)
{
    int layer = 1;
    int neuron = 0;
    for(unsigned int i = 0; i < activations.size(); ++i)
    {
	if(activations[i].getType() == shmea::GType::FLOAT_TYPE)
	{
	    layerInfoList[layer]->setActivation(neuron, activations.getFloat(i));
	    ++neuron;
	} else if (activations[i].getType() == shmea::GType::STRING_TYPE)
	{
	    ++layer;
	    neuron = 0;
	}
    }
}

void DrawNeuralNet::setWeights(shmea::GList weights)
{
    int layer = 1; //Start with Hidden Layer
    int neuron = 0;

    std::vector<shmea::GPointer<float> > newWeights;
    for(unsigned int i = 0; i < weights.size(); ++i)
    {
	if(weights[i].getType() == shmea::GType::FLOAT_TYPE)
	{
	    newWeights.push_back(shmea::GPointer<float>(new float(weights.getFloat(i))));
	} else if (weights[i].getType() == shmea::GType::STRING_TYPE)
	{
	    if(weights.getString(i) == ",")
	    {
		layerInfoList[layer]->setWeights(neuron, newWeights);
		++neuron;
		newWeights.clear();
	    } else if (weights.getString(i) == ";")
	    {
		++layer;
		neuron = 0;
	    }
	}
    }
	
}

int DrawNeuralNet::getLayersCount()
{
    return layers;
}

std::vector<shmea::GPointer<DrawNeuron> > DrawNeuralNet::getLayerNeurons(int layer)
{
        if (layer < 0 || layer >= layers)
	{
	    return std::vector<shmea::GPointer<DrawNeuron> >();
	}
	return layerInfoList[layer]->getNeuronList();
}

int DrawNeuralNet::getLayerNeuronsCount(int layer)
{
    if (layer < 0 || layer >= layers)
	{
	    return 0;
	}
	return layerInfoList[layer]->getNeurons();
}

void DrawNeuralNet::displayNeuralNet()
{
	for(int i = 0; i < layers; ++i)
	{
	    if(i == 0)
		printf("Input Layer:\n");
	    else if(i == layers - 1)
		printf("Output Layer:\n");
	    else
		printf("Hidden Layer %d:\n", i);


	    layerInfoList[i]->displayLayer();

	}
}
