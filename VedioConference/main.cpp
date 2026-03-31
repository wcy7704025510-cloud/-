#include"./ckernal.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //VedioConferenceDialog w;
    //w.show();
    Ckernal::GetInstance();
    return a.exec();
}
