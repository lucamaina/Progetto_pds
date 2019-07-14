#include <QCoreApplication>
#include <server_main.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Server texteditor distribuito";
    server_main* server = new server_main();
    return a.exec();
}
