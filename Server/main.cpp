#include <QCoreApplication>
#include <server_main.h>
#include <QDebug>

bool asdscriviXML(void);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Server texteditor distribuito";
    try {
        server_main server = server_main();
        (void) server; // suppress warning of non used variable
    } catch (std::exception e) {
        qDebug() << e.what();
    }

    return a.exec();
}
