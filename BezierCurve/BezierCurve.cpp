#include "BezierCurve.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
BezierCurve::BezierCurve(QWidget *parent)
    : QWidget(parent)
{
    QWidget* leftwid = new QWidget;
    leftwid->setFixedWidth(200);
    m_addPoint = new QPushButton(("增加控制"));
    m_t = new QLineEdit();
    m_t->setFixedSize(100, 50);
    m_sett = new QPushButton(("设置t区间:"));
    QVBoxLayout* leftlay = new QVBoxLayout();
    leftlay->addWidget(m_addPoint);
    QHBoxLayout* linelay = new QHBoxLayout(); 
    linelay->addWidget(m_sett);
    linelay->addWidget(m_t);
    leftlay->addLayout(linelay);
    leftwid->setLayout(leftlay);

    m_scene = new BezierScene();
    m_scene->setSceneRect(QRectF(0, 0, 2000, 2000));
    m_view = new QGraphicsView(m_scene);

    QHBoxLayout* mainlay = new QHBoxLayout;
    mainlay->addWidget(leftwid);
    mainlay->addWidget(m_view);
    setLayout(mainlay);
    connect(m_addPoint, SIGNAL(clicked(bool)), this, SLOT(onaddCurveClick()));
    connect(m_sett, SIGNAL(clicked(bool)), this, SLOT(onSetTClick()));
    connect(m_scene, SIGNAL(addCPoint()), this, SLOT(addCPoint()));

    setMinimumSize(600, 400);
}

BezierCurve::~BezierCurve()
{
}
void BezierCurve::onSetTClick()
{
    bool b = false;
    int val = m_t->text().toInt(&b);
    if (b)
        m_scene->setTval(val);
}
void BezierCurve::onaddCurveClick()
{
    m_addPoint->setEnabled(false);
    m_scene->addBezierCurve();
}

void BezierCurve::addCPoint()
{
    m_addPoint->setEnabled(true);
}

