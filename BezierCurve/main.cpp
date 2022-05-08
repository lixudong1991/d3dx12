#include "BezierCurve.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BezierCurve w;
    w.show();
    return a.exec();
}
