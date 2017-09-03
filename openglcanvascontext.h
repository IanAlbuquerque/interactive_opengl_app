#ifndef OPENGLCANVASCONTEXT_H
#define OPENGLCANVASCONTEXT_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class OpenGLCanvasContext
    : public QOpenGLWidget, protected QOpenGLFunctions
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

private:
  std::vector<QVector3D> points;

  QOpenGLShaderProgram* shaderProgram;
  QOpenGLBuffer pointsBuffer;

  QMatrix4x4 viewMatrix4x4;
  QMatrix4x4 projectionMatrix4x4;
};

#endif // OPENGLCANVASCONTEXT_H
