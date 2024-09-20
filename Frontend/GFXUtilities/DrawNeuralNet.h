#ifndef _DRAWNEURALNET
#define _DRAWNEURALNET

#include "GraphablePoint.h"
#include "RULayerInfo.h"
#include "Backend/Database/GPointer.h"
#include "Backend/Database/GList.h"
#include "Backend/Database/GType.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>

class gfxpp;

class DrawNeuralNet : public GraphablePoint
{

protected:
    int layers;
    std::vector<shmea::GPointer<RULayerInfo> > layerInfoList;
    
    int layerSpacing;
    int neuronSpacing;
    int neuronRadius;

public:
    DrawNeuralNet();    
    DrawNeuralNet(int);
    virtual ~DrawNeuralNet();

    void setInputLayer(int);
    void setHiddenLayer(int, int);
    void setOutputLayer(int);

    void setActivation(shmea::GList);
    void setWeights(shmea::GList);

    int getLayersCount();
    std::vector<shmea::GPointer<DrawNeuron> > getLayerNeurons(int);
    int getLayerNeuronsCount(int);



    void displayNeuralNet();

    virtual double getX() const
    {
	return 0;
    }

    virtual double getY() const
    {
	return 0;
    }

    virtual void setX(double newX)
    {
	//
    }

    virtual void setY(double newY)
    {
	//
    }

    virtual double getMinY() const
    {
	return 0;
    }

    virtual double getMaxY() const
    {
	return 0;
    }



};




#endif
