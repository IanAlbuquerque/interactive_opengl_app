#ifndef COMPOSITEBEZIERCURVE_H
#define COMPOSITEBEZIERCURVE_H

#include <vector>
#include <QVector2D>

#include "beziercurve.h"

class CompositeBezierCurve
{
public:
  CompositeBezierCurve();
  QVector2D getPoint(float h);
  void pushPassingPoint(QVector2D passingPoint, float weight);
  int numPassingPoints();
  QVector2D getPassingPoint(int i);
  int getPassingPointWeight(int i);
  int numDerivativePoints();
  QVector2D getDerivativePoint(int i);
  int getDerivativePointWeight(int i);
  void movePassingPoint(int i, float x, float y, float w);
  void movePassingPoint(int i, float x, float y);
  void deletePassingPoint(int i);
  int numCurves();
private:
  int _numPassingPoints;
  std::vector<BezierCurve> bezierCurves;
};

#endif // COMPOSITEBEZIERCURVE_H
