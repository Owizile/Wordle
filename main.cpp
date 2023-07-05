#include "wordlewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WordleWindow w;
    w.show();
    return a.exec();
}
