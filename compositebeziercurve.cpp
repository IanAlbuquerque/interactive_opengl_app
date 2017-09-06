#include "compositebeziercurve.h"

CompositeBezierCurve::CompositeBezierCurve()
{
  _numPassingPoints = 0;
}

int CompositeBezierCurve::numPassingPoints()
{
  return _numPassingPoints;
}


void CompositeBezierCurve::pushPassingPoint(QVector2D passingPoint, float weight)
{
  if(_numPassingPoints == 0)
  {
    bezierCurves.push_back(BezierCurve());
    bezierCurves[0].pushControlPoint(passingPoint, weight);
  }
  else if(_numPassingPoints == 1)
  {
    QVector2D segmentDif = passingPoint - bezierCurves[0].getControlPoint(0);
    bezierCurves[0].pushControlPoint(bezierCurves[0].getControlPoint(0) + (segmentDif / 3.0f), 1.0f);
    bezierCurves[0].pushControlPoint(bezierCurves[0].getControlPoint(0) + (2.0f * segmentDif / 3.0f), 1.0f);
    bezierCurves[0].pushControlPoint(passingPoint, weight);
  }
  else
  {
     int lastBezierCurveIndex = bezierCurves.size() - 1;
     QVector2D segmentDif = passingPoint - bezierCurves[lastBezierCurveIndex].getControlPoint(3);

     bezierCurves.push_back(BezierCurve());

     bezierCurves[lastBezierCurveIndex+1].pushControlPoint( bezierCurves[lastBezierCurveIndex].getControlPoint(3),
                                                            bezierCurves[lastBezierCurveIndex].getControlPointWeight(3));
     bezierCurves[lastBezierCurveIndex+1].pushControlPoint( bezierCurves[lastBezierCurveIndex].getControlPoint(3) + (1.0f * segmentDif / 3.0f),
                                                            1.0f);
     bezierCurves[lastBezierCurveIndex+1].pushControlPoint( bezierCurves[lastBezierCurveIndex].getControlPoint(3) + (2.0f * segmentDif / 3.0f),
                                                            1.0f);
     bezierCurves[lastBezierCurveIndex+1].pushControlPoint(passingPoint, weight);
  }

  _numPassingPoints++;
}

QVector2D CompositeBezierCurve::getPoint(float h)
{
  float wholeCurveH = h * (bezierCurves.size() * 1.0f);
  int bezierIndex = (int) wholeCurveH;
  float bezierH = wholeCurveH - (float) bezierIndex;
  return bezierCurves[bezierIndex].getPoint(bezierH);
}


QVector2D CompositeBezierCurve::getPassingPoint(int i)
{
  if(i == 0)
  {
    return bezierCurves[0].getControlPoint(0);
  }
  else
  {
    return bezierCurves[i-1].getControlPoint(3);
  }
}

float CompositeBezierCurve::getPassingPointWeight(int i)
{
  if(i == 0)
  {
    return bezierCurves[0].getControlPointWeight(0);
  }
  else
  {
    return bezierCurves[i-1].getControlPointWeight(3);
  }
}

void CompositeBezierCurve::movePassingPoint(int i, float x, float y, float w, bool alter)
{
  if(i < 0 || i >= _numPassingPoints)
  {
    return;
  }

  if(i == 0)
  {
    QVector2D difNextPoint = bezierCurves[0].getControlPoint(1) - bezierCurves[0].getControlPoint(0);
    bezierCurves[0].moveControlPoint(0, x, y, w);
    QVector2D point0 = bezierCurves[0].getControlPoint(0);
    QVector2D point1 = point0 + difNextPoint;
    bezierCurves[0].moveControlPoint(1, point1[0], point1[1]);
  }
  else if( i == _numPassingPoints - 1)
  {
    QVector2D usualDif = bezierCurves[i-1].getControlPoint(2) - bezierCurves[i-1].getControlPoint(3);
    bezierCurves[i-1].moveControlPoint(3, x, y, w);
    if(alter)
    {
      QVector2D difLastPoint = bezierCurves[i-1].getControlPoint(3) - bezierCurves[i-1].getControlPoint(0);
      QVector2D posControlPoint2 = bezierCurves[i-1].getControlPoint(3) - difLastPoint / 3.0f;
      QVector2D posControlPoint1 = bezierCurves[i-1].getControlPoint(3) - 2.0f * difLastPoint / 3.0f;
      bezierCurves[i-1].moveControlPoint(2, posControlPoint2[0], posControlPoint2[1]);
      if(i-2 >= 0)
      {
        QVector2D difMinus2 = bezierCurves[i-2].getControlPoint(0) - bezierCurves[i-2].getControlPoint(3);
        QVector2D directionPos1AndMinus2 = (bezierCurves[i-1].getControlPoint(0) + difMinus2/3.0f)  - posControlPoint1;
        directionPos1AndMinus2.normalize();
        QVector2D posControlPointMinus2 = bezierCurves[i-1].getControlPoint(0) + (difMinus2.length() / 3.0f) * directionPos1AndMinus2;
        posControlPoint1 = bezierCurves[i-1].getControlPoint(0) - (difMinus2.length() / 3.0f) * directionPos1AndMinus2;
        bezierCurves[i-2].moveControlPoint(2, posControlPointMinus2[0], posControlPointMinus2[1]);
      }
      bezierCurves[i-1].moveControlPoint(1, posControlPoint1[0], posControlPoint1[1]);
    }
    else
    {
      QVector2D newPoint3 = bezierCurves[i-1].getControlPoint(3);
      QVector2D newPoint2 = newPoint3 + usualDif;
      bezierCurves[i-1].moveControlPoint(2, newPoint2[0], newPoint2[1]);
    }
  }
  else
  {
    QVector2D difNextPoint = bezierCurves[i].getControlPoint(1) - bezierCurves[i-1].getControlPoint(3);
    QVector2D difLastPoint = bezierCurves[i-1].getControlPoint(2) - bezierCurves[i-1].getControlPoint(3);
    bezierCurves[i-1].moveControlPoint(3, x, y, w);
    bezierCurves[i].moveControlPoint(0, x, y, w);
    QVector2D nextPoint = bezierCurves[i-1].getControlPoint(3) + difNextPoint;
    QVector2D lastPoint = bezierCurves[i-1].getControlPoint(3) + difLastPoint;
    bezierCurves[i].moveControlPoint(1, nextPoint[0], nextPoint[1]);
    bezierCurves[i-1].moveControlPoint(2, lastPoint[0], lastPoint[1]);
  }
}

void CompositeBezierCurve::movePassingPoint(int i, float x, float y, bool alter)
{
  if(i < 0 || i >= _numPassingPoints)
  {
    return;
  }

  movePassingPoint(i, x, y, getPassingPointWeight(i), alter);
}

void CompositeBezierCurve::deletePassingPoint(int i)
{
  if(i < 0 || i >= _numPassingPoints)
  {
    return;
  }

  if(_numPassingPoints == 0)
  {
    return;
  }
  else if(_numPassingPoints == 1)
  {
    this->bezierCurves.erase(bezierCurves.begin());
  }
  else if(_numPassingPoints == 2)
  {
    QVector2D ptToKeep;
    int weightToKeep;
    if(i==0)
    {
      ptToKeep = bezierCurves[0].getControlPoint(3);
      weightToKeep = bezierCurves[0].getControlPointWeight(3);
    }
    else
    {
      ptToKeep = bezierCurves[0].getControlPoint(0);
      weightToKeep = bezierCurves[0].getControlPointWeight(0);
    }
    bezierCurves.erase(bezierCurves.begin());
    bezierCurves.push_back(BezierCurve());
    bezierCurves[0].pushControlPoint(ptToKeep, weightToKeep);
  }
  else
  {
    if(i==0)
    {
      bezierCurves.erase(bezierCurves.begin());
    }
    else if(i==_numPassingPoints-1)
    {
      bezierCurves.erase(bezierCurves.end());
    }
    else
    {
      QVector2D point3 = bezierCurves[i].getControlPoint(3);
      QVector2D point2 = bezierCurves[i].getControlPoint(2);
      bezierCurves.erase(bezierCurves.begin()+i);
      bezierCurves[i-1].moveControlPoint(2, point2[0], point2[1]);
      bezierCurves[i-1].moveControlPoint(3, point3[0], point3[1]);
    }
  }
  _numPassingPoints--;
}


int CompositeBezierCurve::numDerivativePoints()
{
  if(_numPassingPoints == 0 || _numPassingPoints == 1)
  {
    return 0;
  }
  else
  {
    return bezierCurves.size() * 2;
  }
}

QVector2D CompositeBezierCurve::getDerivativePoint(int i)
{
  if(i % 2 == 0)
  {
    return bezierCurves[i/2].getControlPoint(1);
  }
  else
  {
    return bezierCurves[(i-1)/2].getControlPoint(2);
  }
}

float CompositeBezierCurve::getDerivativePointWeight(int i)
{
  if(i % 2 == 0)
  {
    return bezierCurves[i/2].getControlPointWeight(1);
  }
  else
  {
    return bezierCurves[(i-1)/2].getControlPointWeight(2);
  }
}

int CompositeBezierCurve::numCurves()
{
  return bezierCurves.size();
}


void CompositeBezierCurve::moveDerivativePoint(int i, float x, float y, float w)
{
  if(i < 0 || i >= this->numDerivativePoints())
  {
    return;
  }

  if(i % 2 == 0)
  {
    bezierCurves[i/2].moveControlPoint(1,x,y,w);
    if(i != 0)
    {
      QVector2D diff = bezierCurves[i/2].getControlPoint(1) - bezierCurves[i/2].getControlPoint(0);
      QVector2D reflection = bezierCurves[i/2].getControlPoint(0) - diff;
      bezierCurves[(i/2)-1].moveControlPoint(2,reflection[0],reflection[1]);
    }
  }
  else
  {
    bezierCurves[(i-1)/2].moveControlPoint(2,x,y,w);
    if(i != this->numDerivativePoints() - 1)
    {
      QVector2D diff = bezierCurves[(i-1)/2].getControlPoint(2) - bezierCurves[(i-1)/2].getControlPoint(3);
      QVector2D reflection = bezierCurves[(i-1)/2].getControlPoint(3) - diff;
      bezierCurves[(i-1)/2 + 1].moveControlPoint(1,reflection[0],reflection[1]);
    }
  }

}

void CompositeBezierCurve::moveDerivativePoint(int i, float x, float y)
{

  if(i < 0 || i >= this->numDerivativePoints())
  {
    return;
  }

  moveDerivativePoint(i, x, y, getDerivativePointWeight(i));
}
