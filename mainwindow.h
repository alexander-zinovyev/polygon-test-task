#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "polygonscene.h"
#include <QGraphicsView>
#include <QMainWindow>
#include <QPointF>

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
    void sceneMouseMove(QPointF scenePos);

private:
    Ui::MainWindow *ui;
    PolygonScene *scene;
    QGraphicsView *view;
};

#endif // MAINWINDOW_H
