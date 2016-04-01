#ifndef POLYGONSCENE_H
#define POLYGONSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QPointF>

class PolygonScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PolygonScene(QObject *parent = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void resetBuildingScene();
    void completeBuilding();
    void setAllowedToComplete(bool allowed);

signals:
    void mouseMove(QPointF pos);
    void allowed(bool allowed);
    void calculated(bool convex, float area);

protected:
    bool isAllowedToPlace();
    bool isAllowedToComplete();
    bool willBeAbleToComplete();
    void setLineColor(QGraphicsLineItem *line, Qt::GlobalColor color);

    bool eventFilter(QObject *watched, QEvent *event);

private:
    QList<QPointF> points;
    QList<QGraphicsLineItem *> lines;
    QGraphicsLineItem *nextLine, *endLine, *potentialEndingLine;
    bool allowedToPlace, allowedToComplete;
    bool completed;
};

#endif // POLYGONSCENE_H
