#include "BezierScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include <QPainterPath>
#include <math.h>
BezierScene::BezierScene(QObject* parent)
	: QGraphicsScene(parent),
    ponintindex(-1),
    currBezierId(-1),
	m_tval(100)
{

}

void BezierScene::addBezierCurve()
{
    ponintindex = 0;
}

void BezierScene::setTval(int tval)
{
	m_tval = tval;
    for(int i=0;i<BezierCurves.size();i++)
        buildBezierPath(i);

}

void BezierScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (ponintindex==-1 || mouseEvent->button() != Qt::LeftButton)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        return;
    }
    BezierCtlPoint  *ctlpoint;
    if(ponintindex ==0)
    {
        ctlpoint = new BezierCtlPoint;
        memset(ctlpoint,0,sizeof(BezierCtlPoint));
        BezierCurves.append(ctlpoint);
    }else
        ctlpoint= BezierCurves.last();


	QPointF point = mouseEvent->scenePos();

    QColor color;

    if(ponintindex==0)
        color = Qt::red;
    else if(ponintindex==1)
        color = QColor(255,128,0);
    else if(ponintindex==2)
        color = Qt::yellow;
    else if(ponintindex==3)
        color = Qt::green;

    QGraphicsEllipseItem* cpoint = addEllipse(QRectF(point.x() - 10.0, point.y() - 10.0, 20.0, 20.0), QPen(color), QBrush(color));
    cpoint->setData(1000001,QVariant(BezierCurves.size()-1));
    cpoint->setFlag(QGraphicsItem::ItemIsMovable, true);
    cpoint->setFlag(QGraphicsItem::ItemIsSelectable, true);
    cpoint->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    ctlpoint->m_controlPoint[ponintindex] = point;
    ctlpoint->pointItems[ponintindex]= cpoint;

    if (ponintindex == 3)
	{
		double unit = 1.0 / m_tval;
		double vectortimes = 0.0;
		QPainterPath path;
		for (int i=0;i< m_tval+1;i++)
		{
            QPointF ht =  pow(1.0-vectortimes, 3) * ctlpoint->m_controlPoint[0]+
                         (3.0 * vectortimes* pow(1.0 - vectortimes, 2))* ctlpoint->m_controlPoint[1]+
                         (3.0 * pow(vectortimes, 2)*(1.0 - vectortimes))* ctlpoint->m_controlPoint[2]+
                         pow(vectortimes, 3)* ctlpoint->m_controlPoint[3];
			vectortimes += unit;
			if (i==0)
			{
				path.moveTo(ht);
			}
			else{
				path.lineTo(ht);
			}	
		}
        QPen pen(QColor(0,128,128));
        pen.setWidthF(2.0);
        ctlpoint->m_pathitem = addPath(path, pen);
        currBezierId = BezierCurves.size()-1;
    }
}

void BezierScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void BezierScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

    if( ponintindex ==-1 && mouseEvent->button() == Qt::LeftButton && BezierCurves.size()>0)
    {
        if(selectedItems().size()>0)
            currBezierId = selectedItems().first()->data(1000001).toInt();
        buildBezierPath(currBezierId);
    }
    else if(ponintindex ==3)
    {
        ponintindex = -1;
        emit addCPoint();
    }else
        ponintindex++;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void BezierScene::buildBezierPath(int id)
{

    if( id == -1)
        return;
    QGraphicsPathItem  *pathitem = BezierCurves.at(id)->m_pathitem;
    if ( pathitem )
    {
        QPointF p0=BezierCurves.at(id)->m_controlPoint[0];
        QPointF p0c=BezierCurves.at(id)->pointItems[0]->scenePos();
        p0+=p0c;
        QPointF p1=BezierCurves.at(id)->m_controlPoint[1];
        QPointF p1c=BezierCurves.at(id)->pointItems[1]->scenePos();
        p1+=p1c;
        QPointF p2=BezierCurves.at(id)->m_controlPoint[2];
        QPointF p2c=BezierCurves.at(id)->pointItems[2]->scenePos();
        p2+=p2c;
        QPointF p3=BezierCurves.at(id)->m_controlPoint[3];
        QPointF p3c=BezierCurves.at(id)->pointItems[3]->scenePos();
        p3+=p3c;
        double unit = 1.0 / m_tval;
        double vectortimes = 0.0;
        QPainterPath path;
        for (int i = 0; i < m_tval + 1; i++)
        {
            QPointF ht = pow(1.0 - vectortimes, 3) * p0 +
                (3.0 * vectortimes * pow(1.0 - vectortimes, 2)) * p1 +
                (3.0 * pow(vectortimes, 2) * (1.0 - vectortimes)) *p2 +
                pow(vectortimes, 3) *p3;
            vectortimes += unit;
            if (i == 0)
            {
                path.moveTo(ht);
            }
            else {
                path.lineTo(ht);
            }
        }
        pathitem->setPath(path);
    }
}
