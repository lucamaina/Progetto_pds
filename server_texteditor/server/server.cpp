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

void server::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "Connecting from "
             << socketDescriptor;
    s_thread *newThread = new s_thread(socketDescriptor);
    connect(newThread, SIGNAL(finished()), this, SLOT(deleteLater()), Qt::ConnectionType::DirectConnection);

    // TODO exception
    newThread->run();
}
