#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPointF>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    scene(new PolygonScene()),
    view(new QGraphicsView(scene))
{
    ui->setupUi(this);

    //set view cursor
    view->viewport()->setCursor(Qt::CrossCursor);

    //translate mouse events to graphicsscene
    view->setMouseTracking(true);
    connect(scene, SIGNAL(mouseMove(QPointF)), this, SLOT(sceneMouseMove(QPointF)));

    this->setCentralWidget(view);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sceneMouseMove(QPointF scenePos) {
    this->statusBar()->showMessage(QString::number(scenePos.x()) + "; " + QString::number(scenePos.y()));
}
