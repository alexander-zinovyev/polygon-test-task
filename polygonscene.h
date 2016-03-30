#ifndef POLYGONSCENE_H
#define POLYGONSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class PolygonScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PolygonScene(QObject *parent = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);

signals:
    void mouseMove(QPointF pos);
};

#endif // POLYGONSCENE_H
