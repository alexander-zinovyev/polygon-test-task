#include "polygonscene.h"
#include <QGraphicsEllipseItem>

PolygonScene::PolygonScene(QObject *parent) : QGraphicsScene(0, 0, 450, 450, parent)
{
    this->setAllowedToComplete(false);
    this->installEventFilter(this);
}

void PolygonScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QGraphicsScene::mousePressEvent(mouseEvent);

    if (this->completed || !this->allowedToPlace) {
        return;
    }

    int radius = 2;
    QGraphicsEllipseItem *ellipse = this->addEllipse(mouseEvent->scenePos().x() - radius, mouseEvent->scenePos().y() - radius, 2 * radius, 2 * radius);
    ellipse->setBrush(Qt::black);

    if (!points.isEmpty()) {
        QPointF &lastpoint = points.last();
        lines.push_back(this->addLine(lastpoint.x(), lastpoint.y(), mouseEvent->scenePos().x(), mouseEvent->scenePos().y()));
    }
    points.push_back(mouseEvent->scenePos());

    if (nextLine == nullptr) {
        nextLine = this->addLine(QLineF(points.last(), mouseEvent->scenePos()), QPen(Qt::green));
    } else {
        nextLine->setLine(QLineF(points.last(), mouseEvent->scenePos()));
    }

    if (points.size() >= 2 && endLine == nullptr) {
        endLine = this->addLine(QLineF(points.first(), mouseEvent->scenePos()), QPen(Qt::green, 1, Qt::DashLine));
    }

    if (points.size() > 2) {
        if (potentialEndingLine == nullptr) {
            potentialEndingLine = this->addLine(QLineF(points.first(), points.last()), QPen(Qt::transparent, 1, Qt::DashLine));
        } else {
            potentialEndingLine->setLine(QLineF(points.first(), points.last()));
        }
    }

    this->setAllowedToComplete(isAllowedToComplete());
}

void PolygonScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (completed) {
        return;
    }

    emit mouseMove(mouseEvent->scenePos());

    //check if its allowed to place next point
    this->allowedToPlace = isAllowedToPlace();

    //draw next & ending line
    if (nextLine != nullptr) {
        QLineF line = nextLine->line();
        line.setP2(mouseEvent->scenePos());
        nextLine->setLine(line);

        QPen pen = nextLine->pen();
        if (points.size() == 1) {
            pen.setColor(Qt::black);
            pen.setStyle(Qt::DashLine);
        } else {
            pen.setColor((this->allowedToPlace) ? Qt::green : Qt::red);
            pen.setStyle(Qt::SolidLine);
        }
        nextLine->setPen(pen);
    }
    if (endLine != nullptr) {
        QLineF line = endLine->line();
        line.setP2(mouseEvent->scenePos());
        endLine->setLine(line);

        setLineColor(endLine, (this->allowedToPlace && willBeAbleToComplete()) ? Qt::green : Qt::red);
    }
}

bool PolygonScene::eventFilter(QObject *watched, QEvent *event) {
    if (this->completed)
        return false;

    if (event->type() == QEvent::Leave) {
        if (points.size() > 2) {
            setLineColor(potentialEndingLine, (allowedToComplete) ? Qt::green : Qt::red);
        }
        setLineColor(nextLine, Qt::transparent);
        setLineColor(endLine, Qt::transparent);
    } else if (event->type() == QEvent::Enter) {
        setLineColor(potentialEndingLine, Qt::transparent);
    }
    return false;
}

void PolygonScene::setLineColor(QGraphicsLineItem *line, Qt::GlobalColor color) {
    if (line != nullptr) {
        QPen pen = line->pen();
        pen.setColor(color);
        line->setPen(pen);
    }
}

bool PolygonScene::isAllowedToPlace() {
    for (auto it = lines.begin(); it != lines.end(); it++) {
        if (nextLine != nullptr && it != --lines.end()) {
            if (nextLine->collidesWithItem(*it))
                return false;
        }
    }
    return true;
}

bool PolygonScene::isAllowedToComplete() {
    if (points.size() <= 2)
        return false;
    else if (points.size() == 3) {
        return (potentialEndingLine != nullptr && !potentialEndingLine->contains(points.at(1)));
    }

    for (auto it = ++lines.begin(); it != --lines.end(); it++) {
        if (potentialEndingLine->collidesWithItem(*it))
            return false;
    }
    return true;
}

bool PolygonScene::willBeAbleToComplete() {
    for (auto it = ++lines.begin(); it != lines.end(); it++) {
        if (endLine->collidesWithItem(*it))
            return false;
    }
    return true;
}

void PolygonScene::setAllowedToComplete(bool allowed) {
    if (this->allowedToComplete != allowed) {
        this->allowedToComplete = allowed;
        emit this->allowed(allowed);
    }
}

void PolygonScene::resetBuildingScene() {
    this->clear();
    points.clear();
    lines.clear();
    nextLine = nullptr;
    endLine = nullptr;
    potentialEndingLine = nullptr;

    completed = false;
    this->setAllowedToComplete(false);
}

void PolygonScene::completeBuilding() {
    this->completed = true;

    //remove helper lines
    if (nextLine != nullptr) {
        this->removeItem(nextLine);
        nextLine = nullptr;
    }
    if (endLine != nullptr) {
        this->removeItem(endLine);
        endLine = nullptr;
    }

    //build QPolygon based on our points & move it to back
    QGraphicsPolygonItem *polygon = this->addPolygon(QPolygonF(points.toVector()), QPen(Qt::transparent), QBrush(Qt::green));
    polygon->setZValue(-1);

    //set potential ending line as actual (set its color & style)
    QPen pen = potentialEndingLine->pen();
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    potentialEndingLine->setPen(pen);

    //determine polygon type by checking signs of all cross products - they must be equal
    //if not - polygon is not convex
    bool convex = true;

    int sign = 0;
    for (auto it = points.begin(), it2 = it + 1, it3 = it + 2; it3 != points.end(); it++, it2++, it3++) {
        int dx1 = it2->x() - it->x(),
            dy1 = it2->y() - it->y(),
            dx2 = it3->x() - it2->x(),
            dy2 = it3->y() - it2->y();

        int zcrossproduct = dx1*dy2 - dy1*dx2;
        if (zcrossproduct != 0) {
            if (sign == 0) {
                sign = zcrossproduct / std::abs(zcrossproduct);
            } else if (sign != zcrossproduct / std::abs(zcrossproduct)) {
                convex = false;
                break;
            }
        }
    }

    //calculate polygon area
    //area = 1/2 * abs((P[i].x() + P[i+1].x()) * (P[i].y() - P[i + 1].y()))
    int s = 0;
    auto it = points.begin(), prev = it++;
    for ( ; it != points.end(); it++, prev++) {
        s += (prev->x() + it->x()) * (prev->y() - it->y());
    }
    it = points.begin();
    s += (prev->x() + it->x()) * (prev->y() - it->y());

    float area = 0.5f * std::abs(s);

    emit calculated(convex, area);
}
