#include "Graphable.h"
#include "RUGraph.h"
#include "../GFXUtilities/DrawNeuralNet.h"
#include "../Graphics/GfxRenderer.h"
#include <limits>
#include <algorithm>

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
					if(!tWeights[i]) continue;
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
						if (cGfx->getDraw()) cGfx->getDraw()->setDrawColor(0, 0, 0, 255);
					}
					else
					{
						if (cGfx->getDraw()) cGfx->getDraw()->setDrawColor(255, 0, 0, 255);
					}
					if (cGfx->getDraw()) cGfx->getDraw()->drawPoint(pointX, pointY);

				}
			}

			if (cWeights.size() > 0)
			{
			//Draws connections from current left layer neurons to the currently drawn neuron
				if(prevNodes.size() > 0)
				{
					// Clamp to the smallest of the two to avoid out-of-range access
					unsigned int numConnections = (unsigned int)std::min(prevNodes.size(), cWeights.size());
					for(unsigned int i = 0; i < numConnections; i++)
					    {
						if(!cWeights[i]) continue;
						float Weight = *cWeights[i];

						int R = 0;
						int G = 0;
						float range = cWeightMax - cWeightMin;
						if (range <= 1e-6f)
						{
							R = 200;
							G = 200;
						}
						else
						{
							float t = (Weight - cWeightMin) / range;
							if (t < 0.0f) t = 0.0f;
							if (t > 1.0f) t = 1.0f;
							R = (int)((1.0f - t) * 255.0f);
							G = (int)(t * 255.0f);
						}

					if (cGfx->getDraw()) cGfx->getDraw()->setDrawColor(R, G, 0, 255);
					if (cGfx->getDraw()) cGfx->getDraw()->drawLine(curNeuron.getX(), curNeuron.getY(), prevNodes[i].getX(), prevNodes[i].getY());
					}
				}
			}
		}
		// After finishing this layer, set prevNodes to current layer's nodes
		prevNodes = curNodes;
	}
}
