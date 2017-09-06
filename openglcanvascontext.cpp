#define POINT_MOUSE_TOLERANCE_IN_PIXELS 10.0f

#define PASSING_POINT 0
#define DERIVATIVE_POINT 1

#include "openglcanvascontext.h"
#include <cstdio>

#include <QMouseEvent>

OpenGLCanvasContext::OpenGLCanvasContext(QWidget* parent)
  : QOpenGLWidget(parent), shaderProgram(nullptr)
{
  isDrawingBezierCurve = false;
  isMouseInCanvas = false;
  isControlPointsVisible = false;
  hasPointSelected = false;
  isMovingPoint = false;
  isMovingCanvas = false;
  setMouseTracking(true);

  canvasTopX = -10.0f;
  canvasTopY = 10.0f;
  pixelsPerUnit = 50.0f;
}


OpenGLCanvasContext::~OpenGLCanvasContext()
{
  makeCurrent();
  controlPointsBuffer.destroy();
  bezierCurvesBuffer.destroy();
  doneCurrent();
  delete shaderProgram;

  for(unsigned int i=0; i<bezierCurves.size(); i++)
  {
    //delete bezierCurves[i];
  }
  bezierCurves.clear();
}


void OpenGLCanvasContext::initializeGL()
{
  initializeOpenGLFunctions();

  makeCurrent();

  glViewport(0,0,width(),height());

  //Layout de ponto e linha:
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.0f);
  glPointSize(POINT_MOUSE_TOLERANCE_IN_PIXELS);

  shaderProgram = new QOpenGLShaderProgram();
  shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexshader.vsh" );
  shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentshader.fsh");
  shaderProgram->link();

  if (!shaderProgram->isLinked())
  {
      //TODO: Exibir erro de link e fechar o programa
  }

  shaderProgram->bind();

  controlPointsBuffer.create();
  bezierCurvesBuffer.create();

  resizeGL(width(), height());
  emit unselectedAllPoints();
}


void OpenGLCanvasContext::paintGL()
{
  shaderProgram->bind();

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  //---------------------------------------------------------

  for(unsigned int i=0; i < bezierCurves.size(); i++)
  {
    drawBezierCurve(bezierCurves[i]);
  }
}

void OpenGLCanvasContext::drawBezierCurve(CompositeBezierCurve bezierCurve)
{
  std::vector<QVector3D> bezierPoints;
  std::vector<QVector3D> passingPoints;
  std::vector<QVector3D> derivativePoints;
  std::vector<QVector3D> linePoints;
  std::vector<QVector3D> selectedPoints;

  if(bezierCurve.numDerivativePoints() == 0 || bezierCurve.numPassingPoints() == 0)
  {
    return;
  }

  int numPoints = bezierCurve.numCurves() * 100;
  for(int i=0; i < numPoints; i++)
  {
     bezierPoints.push_back(QVector3D(bezierCurve.getPoint((float)i/numPoints), 0.0f));
  }

  bezierCurvesBuffer.bind();
  bezierCurvesBuffer.allocate( &bezierPoints[0], (int) bezierPoints.size() * sizeof(QVector3D) );
  shaderProgram->enableAttributeArray(0);
  shaderProgram->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));
  shaderProgram->setUniformValue("color", QVector3D(0,1,0));
  glDrawArrays(GL_LINE_STRIP, 0, (int)bezierPoints.size());

  if(isControlPointsVisible)
  {
    for(int i = 0; i < bezierCurve.numPassingPoints(); i++)
    {
      passingPoints.push_back(QVector3D(bezierCurve.getPassingPoint(i), 0.0f));
    }

    if(passingPoints.size() > 0)
    {
      controlPointsBuffer.bind();
      controlPointsBuffer.allocate( &passingPoints[0], (int) passingPoints.size() * sizeof(QVector3D) );
      shaderProgram->enableAttributeArray(0);
      shaderProgram->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));
      shaderProgram->setUniformValue("color", QVector3D(0,1.0,0));
      glDrawArrays(GL_POINTS, 0, (int) passingPoints.size());
    }

    for(int i = 0; i < bezierCurve.numDerivativePoints(); i++)
    {
      derivativePoints.push_back(QVector3D(bezierCurve.getDerivativePoint(i), 0.0f));
    }

    if(derivativePoints.size() > 0)
    {
      controlPointsBuffer.allocate( &derivativePoints[0], (int) derivativePoints.size() * sizeof(QVector3D) );
      shaderProgram->enableAttributeArray(0);
      shaderProgram->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));
      shaderProgram->setUniformValue("color", QVector3D(1.0,1.0,0));
      glDrawArrays(GL_POINTS, 0, (int) derivativePoints.size());
    }

    for(int i = 0; i < (int) derivativePoints.size(); i++)
    {
      linePoints.push_back(derivativePoints[i]);
      if(i % 2 == 0)
      {
        linePoints.push_back(passingPoints[i/2]);
      }
      else
      {
        linePoints.push_back(passingPoints[(i+1)/2]);
      }
    }

    if(linePoints.size() > 0)
    {
      controlPointsBuffer.allocate( &linePoints[0], (int) linePoints.size() * sizeof(QVector3D) );
      shaderProgram->enableAttributeArray(0);
      shaderProgram->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));
      shaderProgram->setUniformValue("color", QVector3D(1.0,1.0,0));
      glDrawArrays(GL_LINES, 0, (int) linePoints.size());
    }

    if(hasPointSelected)
    {
      if(movingPointType == PASSING_POINT)
      {
        selectedPoints.push_back(QVector3D(bezierCurves[movingPointBezierIndex].getPassingPoint(movingPointIndex), 0.0f));
      }
      else
      {
        selectedPoints.push_back(QVector3D(bezierCurves[movingPointBezierIndex].getDerivativePoint(movingPointIndex), 0.0f));
      }

      if(selectedPoints.size() > 0)
      {
        controlPointsBuffer.allocate( &selectedPoints[0], (int) selectedPoints.size() * sizeof(QVector3D) );
        shaderProgram->enableAttributeArray(0);
        shaderProgram->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));
        shaderProgram->setUniformValue("color", QVector3D(1.0,0,0));
        glDrawArrays(GL_POINTS, 0, (int) selectedPoints.size());
      }
    }
  }
}


void OpenGLCanvasContext::resizeGL(int width, int height)
{
  glViewport(0, 0, width, height);
  projectionMatrix4x4.setToIdentity();
  projectionMatrix4x4.ortho(canvasTopX, canvasTopX + width/pixelsPerUnit, canvasTopY - height/pixelsPerUnit, canvasTopY, -1.f, 1.0f);
  shaderProgram->bind();
  shaderProgram->setUniformValue("transformMatrix", projectionMatrix4x4*viewMatrix4x4);
}

void OpenGLCanvasContext::mouseDoubleClickEvent(QMouseEvent * event)
{
  if(isDrawingBezierCurve)
  {
    isDrawingBezierCurve = false;
    bezierCurves[bezierCurves.size()-1].deletePassingPoint(bezierCurves[bezierCurves.size()-1].numPassingPoints()-1);
    emit finishedDrawingCurve();
    update();
  }
}

void OpenGLCanvasContext::enterEvent(QEvent *event)
{
  isMouseInCanvas = true;

  if(isDrawingBezierCurve)
  {
    bezierCurves[bezierCurves.size()-1].pushPassingPoint(QVector2D(0.0f, 0.0f), 1.0f);

    update();
  }
  isMovingPoint = false;
}


void OpenGLCanvasContext::leaveEvent(QEvent *event)
{
  isMouseInCanvas = false;

  if(isDrawingBezierCurve)
  {
    bezierCurves[bezierCurves.size()-1].deletePassingPoint(bezierCurves[bezierCurves.size()-1].numPassingPoints()-1);

    update();
  }
  isMovingPoint = false;
}


void OpenGLCanvasContext::mousePressEvent(QMouseEvent *event)
{
  QVector3D point( event->x(), height()-event->y(), 0 );
  point = point.unproject( viewMatrix4x4, projectionMatrix4x4, QRect(0,0,width(),height()));
  QVector2D mousePos(point[0], point[1]);

  if(!isDrawingBezierCurve && isControlPointsVisible)
  {
      isMovingPoint = false;
      hasPointSelected = false;
      for(int k = 0; k < (int) bezierCurves.size(); k++)
      {
        for(int i = 0; i < bezierCurves[k].numPassingPoints(); i++)
        {
          if(mousePos.distanceToPoint(bezierCurves[k].getPassingPoint(i)) < POINT_MOUSE_TOLERANCE_IN_PIXELS/pixelsPerUnit)
          {
            movingPointType = PASSING_POINT;
            movingPointIndex = i;
            movingPointBezierIndex = k;
            isMovingPoint = true;
            hasPointSelected = true;
          }
        }
        for(int i = 0; i < bezierCurves[k].numDerivativePoints(); i++)
        {
          if(mousePos.distanceToPoint(bezierCurves[k].getDerivativePoint(i)) < POINT_MOUSE_TOLERANCE_IN_PIXELS/pixelsPerUnit)
          {
            movingPointType = DERIVATIVE_POINT;
            movingPointIndex = i;
            movingPointBezierIndex = k;
            isMovingPoint = true;
            hasPointSelected = true;
          }
        }
      }

      if(hasPointSelected)
      {
        QVector2D pos;
        float weight;
        if(movingPointType == PASSING_POINT)
        {
          pos = bezierCurves[movingPointBezierIndex].getPassingPoint(movingPointIndex);
          weight = bezierCurves[movingPointBezierIndex].getPassingPointWeight(movingPointIndex);
        }
        else if(movingPointType == DERIVATIVE_POINT)
        {
          pos = bezierCurves[movingPointBezierIndex].getDerivativePoint(movingPointIndex);
          weight = bezierCurves[movingPointBezierIndex].getDerivativePointWeight(movingPointIndex);
        }
        emit selectedPoint(pos[0],pos[1],weight);
      } else {
        emit unselectedAllPoints();
      }

      if(!isMovingPoint)
      {
        isMovingCanvas = true;
        lastMousePos = mousePos;
      }

    update();
  }

  if(!isDrawingBezierCurve && !isControlPointsVisible)
  {
    isMovingCanvas = true;
    lastMousePos = mousePos;
  }
}


void OpenGLCanvasContext::mouseMoveEvent(QMouseEvent *event)
{
  QVector3D point( event->x(), height()-event->y(), 0 );
  point = point.unproject( viewMatrix4x4, projectionMatrix4x4, QRect(0,0,width(),height()));
  point.setZ(0.f);
  QVector2D mousePos(point[0], point[1]);

  if(isDrawingBezierCurve)
  {
    int lastPassingPointIndex = bezierCurves[bezierCurves.size()-1].numPassingPoints() - 1;
    bezierCurves[bezierCurves.size()-1].movePassingPoint(lastPassingPointIndex, point[0], point[1], true);

    update();
  }
  if(isMovingPoint && hasPointSelected)
  {
    float weight;
    if(movingPointType == PASSING_POINT)
    {
      weight = bezierCurves[movingPointBezierIndex].getPassingPointWeight(movingPointIndex);
      bezierCurves[movingPointBezierIndex].movePassingPoint(movingPointIndex, point[0], point[1]);
    }
    else if(movingPointType == DERIVATIVE_POINT)
    {
      weight = bezierCurves[movingPointBezierIndex].getDerivativePointWeight(movingPointIndex);
      bezierCurves[movingPointBezierIndex].moveDerivativePoint(movingPointIndex, point[0], point[1]);
    }
    emit selectedPoint(point[0],point[1],weight);
    update();
  }
  if(isMovingCanvas && mousePos.distanceToPoint(lastMousePos) > 1.0f/pixelsPerUnit)
  {
    canvasTopX -= mousePos[0] - lastMousePos[0];
    canvasTopY -= mousePos[1] - lastMousePos[1];
    resizeGL(width(), height());

    QVector3D pointNewProj( event->x(), height()-event->y(), 0 );
    pointNewProj = pointNewProj.unproject( viewMatrix4x4, projectionMatrix4x4, QRect(0,0,width(),height()));
    QVector2D mousePosNewProj(pointNewProj[0], pointNewProj[1]);
    lastMousePos = mousePosNewProj;

    update();
  }
}


void OpenGLCanvasContext::mouseReleaseEvent(QMouseEvent *event)
{
  if(isDrawingBezierCurve)
  {
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( viewMatrix4x4, projectionMatrix4x4, QRect(0,0,width(),height()));
    point.setZ(0.f);

    bezierCurves[bezierCurves.size()-1].pushPassingPoint(QVector2D(point[0], point[1]), 1.0f);

    update();
  }
  isMovingPoint = false;
  isMovingCanvas = false;
  resizeGL(width(), height());
}


void OpenGLCanvasContext::wheelEvent(QWheelEvent *event)
{
  int numDegrees = event->delta() / 8;
  int numSteps = numDegrees / 15;
  if(numSteps > 0)
  {
    pixelsPerUnit *= 1.05;
  }
  else
  {
    pixelsPerUnit /= 1.05;
  }

  QVector3D point( event->x(), height()-event->y(), 0 );
  point = point.unproject( viewMatrix4x4, projectionMatrix4x4, QRect(0,0,width(),height()));
  QVector2D mousePos(point[0], point[1]);

  resizeGL(width(), height());

  point = QVector3D( event->x(), height()-event->y(), 0 );
  point = point.unproject( viewMatrix4x4, projectionMatrix4x4, QRect(0,0,width(),height()));
  QVector2D mouseNewPos(point[0], point[1]);

  canvasTopX -= mouseNewPos[0] - mousePos[0];
  canvasTopY -= mouseNewPos[1] - mousePos[1];

  resizeGL(width(), height());
  update();
}

void OpenGLCanvasContext::onNewCurveButtonClicked()
{
  isDrawingBezierCurve = true;
  bezierCurves.push_back(CompositeBezierCurve());
  if(isMouseInCanvas)
  {
    bezierCurves[bezierCurves.size()-1].pushPassingPoint(QVector2D(0.0f, 0.0f), 1.0f);
  }
}

void OpenGLCanvasContext::onShowControlPointsClicked(int checkState)
{
  if(checkState == Qt::CheckState::Checked)
  {
    isControlPointsVisible = true;
  }
  else
  {
    isControlPointsVisible = false;
  }
  update();
}


void OpenGLCanvasContext::onClearCanvasClicked()
{
  isDrawingBezierCurve = false;
  hasPointSelected = false;
  emit unselectedAllPoints();
  emit finishedDrawingCurve();
  bezierCurves.clear();
  update();
}


void OpenGLCanvasContext::changeCoordinateInput(float x, float y, float w)
{
  if(hasPointSelected)
  {
    if(movingPointType == PASSING_POINT)
    {
      bezierCurves[movingPointBezierIndex].movePassingPoint(movingPointIndex, x, y, w);
    }
    else if(movingPointType == DERIVATIVE_POINT)
    {
      bezierCurves[movingPointBezierIndex].moveDerivativePoint(movingPointIndex, x, y, w);
    }
    update();
  }
}

