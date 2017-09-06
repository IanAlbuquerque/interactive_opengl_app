#include "beziercurve.h"

BezierCurve::BezierCurve()
{

}

float BezierCurve::getControlPointWeight(int i)
{
  return weights[i];
}

QVector2D BezierCurve::getControlPoint(int i)
{
  return controlPoints[i];
}

int BezierCurve::numControlPoints()
{
  return (this->controlPoints).size();
}

void BezierCurve::deleteControlPoint(int i)
{
  (this->controlPoints).erase((this->controlPoints).begin()+i);
}

void BezierCurve::moveControlPoint(int i, float x, float y)
{
  if(i < 0 || i >= this->numControlPoints())
  {
    return;
  }
  this->moveControlPoint(i, x, y, (this->weights)[i]);
}

void BezierCurve::moveControlPoint(int i, float x, float y, float w)
{
  if(i < 0 || i >= this->numControlPoints())
  {
    return;
  }
  (this->controlPoints)[i].setX(x);
  (this->controlPoints)[i].setY(y);
  (this->weights)[i] = w;
}

void BezierCurve::pushControlPoint(QVector2D controlPoint, float weight)
{
  this->controlPoints.push_back(controlPoint);
  this->weights.push_back(weight);
  return;
}

QVector2D BezierCurve::getPoint(float h)
{
  int n = this->controlPoints.size();
  float currentMultiplier = 1 * pow(1 - h, n);
  float denominator = currentMultiplier * this->weights[0];
  QVector2D currentPoint = (currentMultiplier * this->weights[0]) * this->controlPoints[0];
  for(int i=1; i < n; i++)
  {
    currentMultiplier *= h;
    currentMultiplier /= (1.0f - h);
    currentMultiplier /= (float) i;
    currentMultiplier *= (float) (n - (i - 1.0f));
    denominator += currentMultiplier * this->weights[i];
    currentPoint += (currentMultiplier * this->weights[i]) * this->controlPoints[i];
  }
  return currentPoint / denominator;
}

std::vector<QVector2D>* BezierCurve::getControlPoints()
{
  int n = this->controlPoints.size();
  std::vector<QVector2D>* controlPointsToReturn = new std::vector<QVector2D>();
  for(int i=0; i<n; i++)
  {
    controlPointsToReturn->push_back((this->controlPoints)[i]);
  }
  return controlPointsToReturn;
}
