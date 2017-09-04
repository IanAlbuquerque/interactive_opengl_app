#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connect(ui->newCurveButton, SIGNAL(clicked()), ui->openGLCanvas, SLOT(onNewCurveButtonClicked()));
}

MainWindow::~MainWindow()
{
  delete ui;
}
