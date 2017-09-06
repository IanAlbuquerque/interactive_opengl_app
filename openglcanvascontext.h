#ifndef OPENGLCANVASCONTEXT_H
#define OPENGLCANVASCONTEXT_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QObject>
#include <vector>

#include "compositebeziercurve.h"

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

  void mouseDoubleClickEvent(QMouseEvent * event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void enterEvent(QEvent *event) override;

public slots:
  void onNewCurveButtonClicked();
  void onClearCanvasClicked();
  void onShowControlPointsClicked(int checkState);

signals:
    void finishedDrawingCurve();

private:
  Q_OBJECT

  std::vector<CompositeBezierCurve> bezierCurves;

  QOpenGLShaderProgram* shaderProgram;
  QOpenGLBuffer controlPointsBuffer;
  QOpenGLBuffer bezierCurvesBuffer;

  QMatrix4x4 viewMatrix4x4;
  QMatrix4x4 projectionMatrix4x4;

  bool isDrawingBezierCurve;
  bool isMouseInCanvas;
  bool isControlPointsVisible;
  bool isMovingPoint;
  bool hasPointSelected;
  int movingPointType;
  int movingPointBezierIndex;
  int movingPointIndex;
  bool isMovingCanvas;

  float canvasTopX;
  float canvasTopY;
  float pixelsPerUnit;

  QVector2D lastMousePos;

  void drawBezierCurve(CompositeBezierCurve bezierCurve);
};

#endif // OPENGLCANVASCONTEXT_H
