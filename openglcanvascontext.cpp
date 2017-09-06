#include "openglcanvascontext.h"
#include <cstdio>

#include <QMouseEvent>

OpenGLCanvasContext::OpenGLCanvasContext(QWidget* parent)
  : QOpenGLWidget(parent), shaderProgram(nullptr)
{
  isDrawingBezierCurve = false;
  isMouseInCanvas = false;
  isControlPointsVisible = false;
  setMouseTracking(true);
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
  glPointSize(5.0f);

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

  projectionMatrix4x4.ortho(-10.f,10.f,-10.f,10.f,-1.f,1.0f);

  shaderProgram->setUniformValue("transformMatrix", projectionMatrix4x4*viewMatrix4x4);
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
      shaderProgram->setUniformValue("color", QVector3D(1.0,0,0));
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

    for(int i = 0; i < derivativePoints.size(); i++)
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
  }
}


void OpenGLCanvasContext::resizeGL(int width, int height)
{
  glViewport(0, 0, width, height);
}

void OpenGLCanvasContext::mouseDoubleClickEvent(QMouseEvent * event)
{
  if(isDrawingBezierCurve)
  {
    isDrawingBezierCurve = false;
    emit finishedDrawingCurve();
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
}


void OpenGLCanvasContext::leaveEvent(QEvent *event)
{
  isMouseInCanvas = false;

  if(isDrawingBezierCurve)
  {
    bezierCurves[bezierCurves.size()-1].deletePassingPoint(bezierCurves[bezierCurves.size()-1].numPassingPoints()-1);

    update();
  }
}


void OpenGLCanvasContext::mousePressEvent(QMouseEvent *event)
{

}


void OpenGLCanvasContext::mouseMoveEvent(QMouseEvent *event)
{
  if(isDrawingBezierCurve)
  {
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( viewMatrix4x4, projectionMatrix4x4, QRect(0,0,width(),height()));
    point.setZ(0.f);

    int lastPassingPointIndex = bezierCurves[bezierCurves.size()-1].numPassingPoints() - 1;
    bezierCurves[bezierCurves.size()-1].movePassingPoint(lastPassingPointIndex, point[0], point[1]);

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
  emit finishedDrawingCurve();
  bezierCurves.clear();
  update();
}

