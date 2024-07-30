#include "Graphable.h"
#include "RUGraph.h"
#include "../GFXUtilities/DrawNeuralNet.h"
#include <limits>
#include "../GFXUtilities/point2.h"

template <>
void Graphable<DrawNeuralNet>::computeAxisRanges(bool additionOptimization)
{

	

	if (!parent)
		return;

	// Set the parents
	parent->setXMin(getXMin());
	parent->setXMax(getXMax());
	parent->setYMin(getYMin());
	parent->setYMax(getYMax());

	parent->requireDrawUpdate();

}

template <>
void Graphable<DrawNeuralNet>::draw(gfxpp* cGfx)
{
    if (points.size() == 0)
		return;

    int nnLayers = points[0]->getLayersCount();
    if (nnLayers < 2)
	return;

    //Component width for testing
    float widthMax = (float)parent->getWidth();
    float heightMax = (float)parent->getHeight();

    std::vector<Point2> prevNodes;
    
    int radiusW = widthMax / (4 * nnLayers - 2);
    for (int layer = 0; layer < nnLayers; ++layer)
	{
	std::vector<Point2> curNodes;
	std::vector<shmea::GPointer<DrawNeuron> > neurons = points[0]->getLayerNeurons(layer);
	int lNeuronAmount = points[0]->getLayerNeuronsCount(layer);

	float cWeightMax = std::numeric_limits<float>::min();
	float cWeightMin = std::numeric_limits<float>::max();
	for (int n = 0; n < lNeuronAmount; ++n)
	{
	    DrawNeuron neuron = *neurons[n];
	    std::vector<shmea::GPointer<float> > tWeights = neuron.getWeights();
	    if (tWeights.size() > 0)
	    {
		for(unsigned int i = 0; i < tWeights.size(); i++)
		{
		    if(*tWeights[i] > cWeightMax)
			cWeightMax = *tWeights[i];
		    if(*tWeights[i] < cWeightMin)
			cWeightMin = *tWeights[i];
		}
	    }
	}


	for (int n = 0; n < lNeuronAmount; ++n)
	{
	    DrawNeuron neuron = *neurons[n];
	    Point2 curNeuron;
	    
	    float cActivation = neuron.getActivation();

	    std::vector<shmea::GPointer<float> > cWeights = neuron.getWeights();

	   int radiusH = heightMax / (4 * lNeuronAmount - 2);

	    int radius = radiusH;
	    if (radiusW < radiusH)
		radius = radiusW;

	    //Draws Circle for Neuron
	    for(int i = -radius; i < radius; i++){
		int pointX = (radiusW * 2) + ((widthMax / nnLayers) * layer) + i;
		for(int j = -radius; j < radius; j++)
		{
		    int pointY = radiusH + ((heightMax / lNeuronAmount) * n) + j;

		    double dist = sqrt(pow(((double)i), 2.0f) + pow(((double)j), 2.0f));

		    if(dist > radius)
			continue;
		//Gets the center point of each drawn neuron and adds it to the vector
		//Also gets the current nodes center, to attach the previous nodes if it exists
		   if (i == 0 && j == 0)
		   {
		       curNeuron = Point2(pointX, pointY);
		       curNodes.push_back(curNeuron);
		   }
		    if(dist > radius / (abs(2.0f * cActivation)))
		    {
			SDL_SetRenderDrawColor(cGfx->getRenderer(), 0, 0, 0, 255);
		    }
		    else
		    {
			SDL_SetRenderDrawColor(cGfx->getRenderer(), 255, 0, 0, 255);
		    }
		    SDL_RenderDrawPoint(cGfx->getRenderer(), pointX, pointY);

		}
	   }

	    if (cWeights.size() > 0)
	    {
	   //Draws connections from current left layer neurons to the currently drawn neuron
		if(prevNodes.size() > 0)
		{


		    //Weight is distributed from each left neuron to the right before proceeding to the next neuron
		    for(unsigned int i = 0; i < prevNodes.size(); i++)
	    	    {
			float Weight = *cWeights[i];

			int R = 0;
			int G = 0;

			if (Weight == cWeightMax)
			{
			    R = 255;
			    G = 0;
			}
			else if(Weight == cWeightMin)
			{
			    R = 0;
			    G = 255;
			}
			else
			{
			    R = (255 * cWeightMin) / Weight;
			    G = (255 * Weight) / cWeightMax;
			   
			}

		    SDL_SetRenderDrawColor(cGfx->getRenderer(), R, G, 0, 255);
		    SDL_RenderDrawLine(cGfx->getRenderer(), curNeuron.getX(), curNeuron.getY(), prevNodes[i].getX(), prevNodes[i].getY());
		    }
		}
	    }
        }
	prevNodes = curNodes;
	}

        

    return;

}
