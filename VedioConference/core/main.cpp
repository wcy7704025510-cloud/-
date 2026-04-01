#include "videoconferencedialog.h"

#include <QApplication>
#include "ckernel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ckernal::GetInstance();
    return a.exec();
}
