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
    connect(scene, SIGNAL(allowed(bool)), this, SLOT(sceneAllowedComplete(bool)));
    connect(scene, SIGNAL(calculated(bool,float)), this, SLOT(scenePolygonCalculated(bool,float)));

    this->setCentralWidget(view);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sceneMouseMove(QPointF scenePos) {
    this->statusBar()->showMessage(QString::number(scenePos.x()) + "; " + QString::number(scenePos.y()));
}

void MainWindow::sceneAllowedComplete(bool allowed) {
    ui->actionComplete->setEnabled(allowed);
}

void MainWindow::scenePolygonCalculated(bool convex, float area) {
    this->statusBar()->showMessage(QString("Built a polygon. It is ") + ((convex) ? "" : "not ") + "convex, it's area = " + QString::number(area));
}

void MainWindow::on_actionReset_triggered()
{
    scene->resetBuildingScene();
}

void MainWindow::on_actionComplete_triggered()
{
    scene->completeBuilding();
}
