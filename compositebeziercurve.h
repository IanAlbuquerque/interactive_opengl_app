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
  float getPassingPointWeight(int i);
  int numDerivativePoints();
  QVector2D getDerivativePoint(int i);
  float getDerivativePointWeight(int i);
  void movePassingPoint(int i, float x, float y, float w, bool alter = false);
  void movePassingPoint(int i, float x, float y, bool alter = false);
  void deletePassingPoint(int i);
  void moveDerivativePoint(int i, float x, float y, float w);
  void moveDerivativePoint(int i, float x, float y);
  int numCurves();
private:
  int _numPassingPoints;
  std::vector<BezierCurve> bezierCurves;
};

#endif // COMPOSITEBEZIERCURVE_H
