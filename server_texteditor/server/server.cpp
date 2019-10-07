#include "server.h"

server::server(QObject *parent) : QTcpServer (parent)
{
    Logger &lg = Logger::getLog();
    this->log = &lg;
    log->write("Costruttore server");
}

void server::startServer()
{
    if (!this->listen(QHostAddress::Any, 2000)) {
        qDebug() << "Errore in 'listen'";
    } else {
        qDebug() << "Listening...";
    }

}

void server::deleteThread()
{
    qDebug() << "delete: " << this->newThread;
    this->newThread->~s_thread();
}

void server::incomingConnection(int socketID)
{
    qDebug() << "Connecting from "
             << socketID;
    this->newThread = new s_thread(socketID);
    connect(newThread, SIGNAL(finished()), this, SLOT(deleteThread()), Qt::ConnectionType::DirectConnection);
    // TODO exception

    newThread->run();

}
