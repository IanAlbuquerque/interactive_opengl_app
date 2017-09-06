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
  connect(ui->openGLCanvas, SIGNAL(selectedPoint(float,float,float)), this, SLOT(onSelectedPoint(float,float,float)));
  connect(ui->openGLCanvas, SIGNAL(unselectedAllPoints()), this, SLOT(onUnselectedAllPoints()));
  connect(ui->xSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onCoordinateSpinBoxChange(double)));
  connect(ui->ySpinBox, SIGNAL(valueChanged(double)), this, SLOT(onCoordinateSpinBoxChange(double)));
  connect(ui->wSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onCoordinateSpinBoxChange(double)));
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

void MainWindow::onSelectedPoint(float x, float y, float w)
{
  ui->coordinatesBox->setEnabled(true);
  ui->xSpinBox->setValue(x);
  ui->ySpinBox->setValue(y);
  ui->wSpinBox->setValue(w);
}

void MainWindow::onUnselectedAllPoints()
{
  ui->coordinatesBox->setEnabled(false);
}

void MainWindow::onCoordinateSpinBoxChange(double value)
{
  double x = ui->xSpinBox->value();
  double y = ui->ySpinBox->value();
  double w = ui->wSpinBox->value();
  ui->openGLCanvas->changeCoordinateInput((float) x, (float) y, (float) w);
}
