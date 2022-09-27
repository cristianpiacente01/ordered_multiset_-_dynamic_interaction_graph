// 866020 Piacente Cristian c.piacente@campus.unimib.it

#include "graphwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphWidget w;
    w.show(); // faccio direttamente la show del mio widget, senza un widget parent come una MainWindow
    return a.exec();
}
