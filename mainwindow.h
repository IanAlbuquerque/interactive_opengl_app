#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void onNewCurveButtonClicked();
  void onFinishDrawingCurve();
  void onSelectedPoint(float x, float y, float w);
  void onUnselectedAllPoints();
  void onCoordinateSpinBoxChange(double value);

private:
  Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
