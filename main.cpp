#include "cpemessenger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CPEMessenger w;
    w.show();

    return a.exec();
}
