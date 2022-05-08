#pragma once

#include <QWidget>
#include <qlineedit.h>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "BezierScene.h"
class BezierCurve : public QWidget
{
    Q_OBJECT

public:
    BezierCurve(QWidget *parent = nullptr);
    ~BezierCurve();
protected:

private slots:
    void onSetTClick();
    void onaddCurveClick();
    void addCPoint();
private:
    QPushButton* m_addPoint;
    QLineEdit* m_t;
    QPushButton* m_sett;
    QGraphicsView* m_view;
    BezierScene* m_scene;

    
   
};
