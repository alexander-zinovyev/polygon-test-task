#include "polygonscene.h"
#include <QGraphicsEllipseItem>
#include <QDebug>
#include <QStatusBar>

PolygonScene::PolygonScene(QObject *parent) : QGraphicsScene(0, 0, 450, 450, parent)
{ }

void PolygonScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    int radius = 5;
    QGraphicsEllipseItem *ellipse = this->addEllipse(mouseEvent->scenePos().x() - radius, mouseEvent->scenePos().y() - radius, 2 * radius, 2 * radius);
    ellipse->setBrush(Qt::black);
    qDebug() << this->sceneRect() << "pos" << mouseEvent->scenePos();
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void PolygonScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    emit mouseMove(mouseEvent->scenePos());
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}
