#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <vector>
#include <QVector2D>

class BezierCurve
{
public:
  BezierCurve();
  void pushControlPoint(QVector2D controlPoint, float weight);
  QVector2D getPoint(float h);
  std::vector<QVector2D>* getControlPoints();
  int numControlPoints();
  void moveControlPoint(int i, float x, float y, float w);
  void moveControlPoint(int i, float x, float y);
  void deleteControlPoint(int i);
  QVector2D getControlPoint(int i);
  float getControlPointWeight(int i);
private:
  std::vector<QVector2D> controlPoints;
  std::vector<float> weights;
};

#endif // BEZIERCURVE_H
