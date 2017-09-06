#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connect(ui->newCurveButton, SIGNAL(clicked()), ui->openGLCanvas, SLOT(onNewCurveButtonClicked()));
  connect(ui->clearCanvasButton, SIGNAL(clicked()), ui->openGLCanvas, SLOT(onClearCanvasClicked()));
  connect(ui->newCurveButton, SIGNAL(clicked()), this, SLOT(onNewCurveButtonClicked()));
  connect(ui->showControlPointsCheck, SIGNAL(stateChanged(int)), ui->openGLCanvas, SLOT(onShowControlPointsClicked(int)));
  connect(ui->openGLCanvas, SIGNAL(finishedDrawingCurve()), this, SLOT(onFinishDrawingCurve()));
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::onNewCurveButtonClicked()
{
  ui->newCurveButton->setEnabled(false);
}

void MainWindow::onFinishDrawingCurve()
{
  ui->newCurveButton->setEnabled(true);
}
