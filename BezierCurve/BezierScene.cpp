#include "BezierScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include <QPainterPath>
#include <QMatrix>
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

    int co = (ponintindex+1)%4;
    if(co==1)
        color = Qt::red;
    else if(co==2)
        color = QColor(255,128,0);
    else if(co==3)
        color = Qt::yellow;
    else if(co==0)
        color = Qt::green;

    QGraphicsEllipseItem* cpoint = addEllipse(QRectF(point.x() - 5.0, point.y() - 5.0, 10.0, 10.0), QPen(color), QBrush(color));
    cpoint->setData(1000001,QVariant(BezierCurves.size()-1));
    cpoint->setFlag(QGraphicsItem::ItemIsMovable, true);
    cpoint->setFlag(QGraphicsItem::ItemIsSelectable, true);
    cpoint->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    ctlpoint->m_controlPoint[ponintindex] = point;
    ctlpoint->pointItems[ponintindex]= cpoint;

    if (ponintindex == 15)
	{

		QPainterPath path;
        QVector<QPointF> points1,*plastpoints=&points1;
        QVector<QPointF> points2,*pcurrpoints=&points2;
        double unitv = 1.0 / m_tval;
        double vvectortimes = 0.0;

        for(int j =0;j<m_tval+1;j++ )
        {
            double a1 = pow(1.0-vvectortimes, 3),a2 =(3.0 * vvectortimes* pow(1.0 - vvectortimes, 2)),
                   a3 = (3.0 * pow(vvectortimes, 2)*(1.0 - vvectortimes)),a4=pow(vvectortimes, 3);
            QPointF vt0 = a1  * ctlpoint->m_controlPoint[0]+
                         a2 * ctlpoint->m_controlPoint[1]+
                         a3 * ctlpoint->m_controlPoint[2]+
                         a4* ctlpoint->m_controlPoint[3];
            QPointF vt1 =  a1 * ctlpoint->m_controlPoint[4]+
                         a2* ctlpoint->m_controlPoint[5]+
                         a3* ctlpoint->m_controlPoint[6]+
                         a4* ctlpoint->m_controlPoint[7];

            QPointF vt2 =  a1 * ctlpoint->m_controlPoint[8]+
                         a2* ctlpoint->m_controlPoint[9]+
                         a3* ctlpoint->m_controlPoint[10]+
                         a4* ctlpoint->m_controlPoint[11];
            QPointF vt3 =  a1 * ctlpoint->m_controlPoint[12]+
                         a2* ctlpoint->m_controlPoint[13]+
                         a3* ctlpoint->m_controlPoint[14]+
                         a4* ctlpoint->m_controlPoint[15];

            vvectortimes +=unitv;

            double unith = 1.0 / m_tval;
            double hvectortimes = 0.0;
            pcurrpoints->clear();
            for (int i=0;i< m_tval+1;i++)
            {
                QPointF ht =  pow(1.0-hvectortimes, 3) * vt0+
                             (3.0 * hvectortimes* pow(1.0 - hvectortimes, 2))* vt1+
                             (3.0 * pow(hvectortimes, 2)*(1.0 - hvectortimes))* vt2+
                             pow(hvectortimes, 3)* vt3;
                pcurrpoints->append(ht);
                hvectortimes += unith;
                if (i==0)
                {
                    path.moveTo(ht);
                }
                else{
                    path.lineTo(ht);
                }

            }
            if(plastpoints->size()>0)
            {
                for(int i=0;i<plastpoints->size();i++)
                {
                    path.moveTo(plastpoints->at(i));
                    path.lineTo(pcurrpoints->at(i));
                }
            }
            QVector<QPointF> *tempppoints =plastpoints;
            plastpoints = pcurrpoints;
            pcurrpoints = tempppoints;
        }

        QPen pen(QColor(0,128,128));
        pen.setWidthF(0.5);
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
    else if(ponintindex ==15)
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
        QPointF p4=BezierCurves.at(id)->m_controlPoint[4];
        QPointF p4c=BezierCurves.at(id)->pointItems[4]->scenePos();
        p4+=p4c;
        QPointF p5=BezierCurves.at(id)->m_controlPoint[5];
        QPointF p5c=BezierCurves.at(id)->pointItems[5]->scenePos();
        p5+=p5c;
        QPointF p6=BezierCurves.at(id)->m_controlPoint[6];
        QPointF p6c=BezierCurves.at(id)->pointItems[6]->scenePos();
        p6+=p6c;
        QPointF p7=BezierCurves.at(id)->m_controlPoint[7];
        QPointF p7c=BezierCurves.at(id)->pointItems[7]->scenePos();
        p7+=p7c;
        QPointF p8=BezierCurves.at(id)->m_controlPoint[8];
        QPointF p8c=BezierCurves.at(id)->pointItems[8]->scenePos();
        p8+=p8c;
        QPointF p9=BezierCurves.at(id)->m_controlPoint[9];
        QPointF p9c=BezierCurves.at(id)->pointItems[9]->scenePos();
        p9+=p9c;
        QPointF p10=BezierCurves.at(id)->m_controlPoint[10];
        QPointF p10c=BezierCurves.at(id)->pointItems[10]->scenePos();
        p10+=p10c;
        QPointF p11=BezierCurves.at(id)->m_controlPoint[11];
        QPointF p11c=BezierCurves.at(id)->pointItems[11]->scenePos();
        p11+=p11c;
        QPointF p12=BezierCurves.at(id)->m_controlPoint[12];
        QPointF p12c=BezierCurves.at(id)->pointItems[12]->scenePos();
        p12+=p12c;
        QPointF p13=BezierCurves.at(id)->m_controlPoint[13];
        QPointF p13c=BezierCurves.at(id)->pointItems[13]->scenePos();
        p13+=p13c;
        QPointF p14=BezierCurves.at(id)->m_controlPoint[14];
        QPointF p14c=BezierCurves.at(id)->pointItems[14]->scenePos();
        p14+=p14c;
        QPointF p15=BezierCurves.at(id)->m_controlPoint[15];
        QPointF p15c=BezierCurves.at(id)->pointItems[15]->scenePos();
        p15+=p15c;


        QPainterPath path;
        QVector<QPointF> points1,*plastpoints=&points1;
        QVector<QPointF> points2,*pcurrpoints=&points2;
        double unitv = 1.0 / m_tval;
        double vvectortimes = 0.0;

        for(int j =0;j<m_tval+1;j++ )
        {
            double a1 = pow(1.0-vvectortimes, 3),a2 =(3.0 * vvectortimes* pow(1.0 - vvectortimes, 2)),
                   a3 = (3.0 * pow(vvectortimes, 2)*(1.0 - vvectortimes)),a4=pow(vvectortimes, 3);
            QPointF vt0 = a1  * p0+
                         a2 * p1+
                         a3 * p2+
                         a4* p3;
            QPointF vt1 =  a1 * p4+
                         a2* p5+
                         a3* p6+
                         a4* p7;

            QPointF vt2 =  a1 * p8+
                         a2* p9+
                         a3* p10+
                         a4* p11;
            QPointF vt3 =  a1 * p12+
                         a2* p13+
                         a3* p14+
                         a4* p15;

            vvectortimes +=unitv;

            double unith = 1.0 / m_tval;
            double hvectortimes = 0.0;
            pcurrpoints->clear();
            for (int i=0;i< m_tval+1;i++)
            {
                QPointF ht =  pow(1.0-hvectortimes, 3) * vt0+
                             (3.0 * hvectortimes* pow(1.0 - hvectortimes, 2))* vt1+
                             (3.0 * pow(hvectortimes, 2)*(1.0 - hvectortimes))* vt2+
                             pow(hvectortimes, 3)* vt3;
                pcurrpoints->append(ht);
                hvectortimes += unith;
                if (i==0)
                {
                    path.moveTo(ht);
                }
                else{
                    path.lineTo(ht);
                }

            }
            if(plastpoints->size()>0)
            {
                for(int i=0;i<plastpoints->size();i++)
                {
                    path.moveTo(plastpoints->at(i));
                    path.lineTo(pcurrpoints->at(i));
                }
            }
            QVector<QPointF> *tempppoints =plastpoints;
            plastpoints = pcurrpoints;
            pcurrpoints = tempppoints;
        }
        pathitem->setPath(path);
    }
}
