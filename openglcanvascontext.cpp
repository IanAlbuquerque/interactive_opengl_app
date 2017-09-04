#include "openglcanvascontext.h"
#include <cstdio>

#include <QMouseEvent>

OpenGLCanvasContext::OpenGLCanvasContext(QWidget* parent)
  : QOpenGLWidget(parent), shaderProgram(nullptr)
{
  isDrawingBezierCurve = false;
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
  glPointSize(8.0f);

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

  for(int i=0; i < bezierCurves.size(); i++)
  {
    drawBezierCurve(bezierCurves[i]);
  }
}

void OpenGLCanvasContext::drawBezierCurve(BezierCurve bezierCurve)
{
  std::vector<QVector3D> bezierPoints;
  std::vector<QVector3D> controlPoints;
  std::vector<QVector2D>* ptControlPoints2D = bezierCurve.getControlPoints();

  for(int i=0; i<100; i++)
  {
     bezierPoints.push_back(QVector3D(bezierCurve.getPoint((float)i/100.0f), 0.0f));
  }

  bezierCurvesBuffer.bind();
  bezierCurvesBuffer.allocate( &bezierPoints[0], (int) bezierPoints.size() * sizeof(QVector3D) );
  shaderProgram->enableAttributeArray(0);
  shaderProgram->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));
  shaderProgram->setUniformValue("color", QVector3D(0,1,0));
  glDrawArrays(GL_LINE_STRIP, 0, (int)bezierPoints.size());

  for(unsigned int i = 0; i < ptControlPoints2D->size(); i++)
  {
    controlPoints.push_back(QVector3D((*ptControlPoints2D)[i], 0.0f));
  }

  controlPointsBuffer.bind();
  controlPointsBuffer.allocate( &controlPoints[0], (int) controlPoints.size() * sizeof(QVector3D) );
  shaderProgram->enableAttributeArray(0);
  shaderProgram->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));
  shaderProgram->setUniformValue("color", QVector3D(1,0,0));
  glDrawArrays(GL_POINTS, 0, (int) controlPoints.size());
  glDrawArrays(GL_LINE_STRIP, 0, (int) controlPoints.size());

  delete ptControlPoints2D;
}


void OpenGLCanvasContext::resizeGL(int width, int height)
{
  glViewport(0, 0, width, height);
}


void OpenGLCanvasContext::mousePressEvent(QMouseEvent *event)
{

}


void OpenGLCanvasContext::mouseMoveEvent(QMouseEvent *event)
{

}


void OpenGLCanvasContext::mouseReleaseEvent(QMouseEvent *event)
{
  if(isDrawingBezierCurve)
  {
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( viewMatrix4x4, projectionMatrix4x4, QRect(0,0,width(),height()));
    point.setZ(0.f);

    bezierCurves[bezierCurves.size()-1].pushControlPoint(QVector2D(point[0], point[1]), 1.0f);

    update();
  }
}

void OpenGLCanvasContext::onNewCurveButtonClicked()
{
  isDrawingBezierCurve = true;
  bezierCurves.push_back(BezierCurve());
}

