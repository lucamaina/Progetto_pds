#include "server.h"

server::server(QObject *parent) : QTcpServer (parent)
{

}

void server::startServer()
{
    if (!this->listen(QHostAddress::Any, 2000)) {
        qDebug() << "Errore in 'listen'";
    } else {
        qDebug() << "Listening...";
    }
}

void server::incomingConnection(int socketID)
{
    qDebug() << "Connecting from "
             << socketID;
    s_thread *newThread = new s_thread(socketID);
    connect(newThread, SIGNAL(finished()), newThread, SLOT(deleteLater()));
    newThread->run();
}