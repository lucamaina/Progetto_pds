#include <QCoreApplication>
#include <server_main.h>
#include <QDebug>

bool asdscriviXML(void);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Server texteditor distribuito";

    server_main* server = new server_main();
    qDebug() << server;
    return a.exec();
}
