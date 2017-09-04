#ifndef OPENGLCANVASCONTEXT_H
#define OPENGLCANVASCONTEXT_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QObject>
#include <vector>

#include "beziercurve.h"

class OpenGLCanvasContext
    : public QOpenGLWidget,
      protected QOpenGLFunctions
{
public:
  explicit OpenGLCanvasContext(QWidget* parent = 0);
  ~OpenGLCanvasContext();

  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
  void onNewCurveButtonClicked();

private:
  Q_OBJECT

  std::vector<BezierCurve> bezierCurves;

  QOpenGLShaderProgram* shaderProgram;
  QOpenGLBuffer controlPointsBuffer;
  QOpenGLBuffer bezierCurvesBuffer;

  QMatrix4x4 viewMatrix4x4;
  QMatrix4x4 projectionMatrix4x4;

  bool isDrawingBezierCurve;

  void drawBezierCurve(BezierCurve bezierCurve);
};

#endif // OPENGLCANVASCONTEXT_H
