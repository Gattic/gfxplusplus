#include <vector>
#include <limits>
#include <cmath>

// Forward declaration of traits
template<typename T>
struct GraphableTraits;

// Define traits for Point2
template<>
struct GraphableTraits<Point2>
{
    static float getY(const Point2* point) { return point->getY(); }
    static float getYMin(const Point2* point) { return point->getY(); }
    static float getYMax(const Point2* point) { return point->getY(); }
};

// Define traits for Candle
template<>
struct GraphableTraits<Candle>
{
    static float getYMin(const Candle* candle) { return candle->getLow(); }
    static float getYMax(const Candle* candle) { return candle->getHigh(); }
    static float getYOpen(const Candle* candle) { return candle->getOpen(); }
    static float getYClose(const Candle* candle) { return candle->getClose(); }
};

template<typename T>
class Graphable
{
public:
    void computeAxisRanges(bool additionOptimization);

private:
    std::vector<T*> points;
    std::vector<T*> normalizedPoints;
    bool redoRange;
    Parent* parent;

    float getXMin() const;
    float getXMax() const;
    float getYMin() const;
    float getYMax() const;
    void setXMin(float value);
    void setXMax(float value);
    void setYMin(float value);
    void setYMax(float value);
};


template<typename T>
void Graphable<T>::computeAxisRanges(bool additionOptimization)
{
    if (!parent)
        return;

    if (points.empty())
        return;

    redoRange = !additionOptimization;
    if (additionOptimization)
    {
        // Check if the latest y is not within the current range
        float yMax = GraphableTraits<T>::getYMax(points.back());
        float yMin = GraphableTraits<T>::getYMin(points.back());
        if ((yMin < getYMin()) || (yMax > getYMax()))
            redoRange = true;
    }

    unsigned int i = 0;
    if (redoRange)
    {
        for (; i < points.size(); ++i)
        {
            T* pt = points[i];
            float x_pt = pt->getX();
            float y_max = GraphableTraits<T>::getYMax(pt);
            float y_min = GraphableTraits<T>::getYMin(pt);

            setXMax(x_pt);
            setXMin(x_pt);
            setYMax(y_max);
            setYMin(y_min);
        }
    }

    // Set the parent's range
    parent->setXMin(getXMin());
    parent->setXMax(getXMax());
    parent->setYMin(getYMin());
    parent->setYMax(getYMax());

	//==============================================Normalize the points==============================================

    unsigned int agg = parent->getAggregate();
    float xRange = static_cast<float>(points.size()) / static_cast<float>(agg);
    float yRange = getYMax() - getYMin();
    if (points.size() % agg)
        ++xRange;

    float pointXGap = static_cast<float>(parent->getWidth()) / xRange;
    float pointYGap = static_cast<float>(parent->getHeight()) / yRange;
    if (isinf(pointXGap))
    {
        normalizedPoints.clear();
        return;
    }

    // Size up the normalized points vector
    while (normalizedPoints.size() < xRange)
    {
        normalizedPoints.push_back(new T());
    }

    while (normalizedPoints.size() > xRange)
    {
        normalizedPoints.pop_back();
	//normalizedPoints.erase(normalizedPoints.begin()+normalizedPoints.size()-1);
    }

    i = 0;
    unsigned int aggCounter = 0;
    float aggValue = 0.0f;
    unsigned int normalCounter = 0;

    for (; i < points.size(); ++i)
    {
        float newYValue = (GraphableTraits<T>::getYMax(points[i]) - getYMin()) * pointYGap;

        ++aggCounter;
        aggValue = newYValue;

        if ((aggCounter < agg) && (i < points.size() - 1))
        {
            continue;
        }

        float newXValue = normalCounter * pointXGap;
        normalizedPoints[normalCounter]->setX(parent->getAxisOriginX() + newXValue);
        normalizedPoints[normalCounter]->setY(parent->getAxisOriginY() + static_cast<float>(parent->getHeight()) - aggValue);

        ++normalCounter;
        aggCounter = 0;
        aggValue = 0.0f;
    }

    parent->requireDrawUpdate();
}

