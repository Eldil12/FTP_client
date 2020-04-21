#include <QApplication>
#include "login.h"
#include <QTextCodec>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    FTPAPI ftpAPI;

    Login w(ftpAPI);
    w.show();

    return a.exec();
}
