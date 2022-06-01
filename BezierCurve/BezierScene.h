#pragma once
#include <QGraphicsScene>


struct BezierCtlPoint
{
   QPointF m_controlPoint[16];
   QGraphicsPathItem* m_pathitem;
   QGraphicsEllipseItem *pointItems[16];
};

class BezierScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit BezierScene(QObject* parent = nullptr);
    void addBezierCurve();
    void setTval(int tval);

signals:
    void addCPoint();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

private:
    int ponintindex;
    QVector<BezierCtlPoint*> BezierCurves;
    int currBezierId;

    int m_tval;

    void buildBezierPath(int id);
};

