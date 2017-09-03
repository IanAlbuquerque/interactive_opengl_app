#include "openglcanvascontext.h"
#include <cstdio>

#include <QMouseEvent>

const char* vertexShaderSource = R"(
    #version 330 core

    layout( location = 0 ) in vec3 vertexPos;
    uniform mat4 transformMatrix;

    void main()
    {
        gl_Position = transformMatrix * vec4( vertexPos, 1 );
    }
)";


const char* fragmentShaderSource = R"(
    #version 330 core

    uniform vec3 color;
    out vec3 finalColor;

    void main()
    {
        finalColor = color;
    }
)";


OpenGLCanvasContext::OpenGLCanvasContext(QWidget* parent)
  : QOpenGLWidget(parent), shaderProgram(nullptr)
{
  // empty
}


OpenGLCanvasContext::~OpenGLCanvasContext()
{
    makeCurrent();
    pointsBuffer.destroy();
    doneCurrent();
    delete shaderProgram;
}


void OpenGLCanvasContext::initializeGL()
{
    initializeOpenGLFunctions();

    makeCurrent();

    glViewport(0,0,width(),height());

    //Layout de ponto e linha:
    glEnable(GL_POINT_SMOOTH);
    glEnable( GL_LINE_SMOOTH );
    glLineWidth(1.0f);
    glPointSize(8.0f);

    shaderProgram = new QOpenGLShaderProgram();
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shaderProgram->link();

    if (!shaderProgram->isLinked())
    {
        //TODO: Exibir erro de link e fechar o programa
    }

    shaderProgram->bind();

    pointsBuffer.create();

    projectionMatrix4x4.ortho(-10.f,10.f,-10.f,10.f,-1.f,1.0f);

    shaderProgram->setUniformValue("transformMatrix", projectionMatrix4x4*viewMatrix4x4);
}


void OpenGLCanvasContext::paintGL()
{
    shaderProgram->bind();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if(!points.empty() && pointsBuffer.isCreated())
    {
        pointsBuffer.bind();
        pointsBuffer.allocate( &points[0], (int)points.size()*sizeof(QVector3D) );
        shaderProgram->enableAttributeArray(0);
        shaderProgram->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));

        //Desenha o poligono
        shaderProgram->setUniformValue("color", QVector3D(1,0,0)); //Vermelho
        glDrawArrays(GL_LINE_STRIP, 0, (int)points.size());

        //Desenha os pontos
        shaderProgram->setUniformValue("color", QVector3D(1,1,0)); //Amarelo
        glDrawArrays(GL_POINTS, 0, (int)points.size());
    }
}


void OpenGLCanvasContext::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}


void OpenGLCanvasContext::mousePressEvent(QMouseEvent *event)
{
  printf("test");
}


void OpenGLCanvasContext::mouseMoveEvent(QMouseEvent *event)
{

}


void OpenGLCanvasContext::mouseReleaseEvent(QMouseEvent *event)
{
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( viewMatrix4x4, projectionMatrix4x4, QRect(0,0,width(),height()));
    point.setZ(0.f);

    points.push_back( point );

    update();
}


