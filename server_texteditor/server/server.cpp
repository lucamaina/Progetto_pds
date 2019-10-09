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

void server::deleteThread(s_thread &t)
{
    m.lock();
    numThread--;
    m.unlock();
    qDebug() << "delete: " << this->newThread;
    t.~s_thread();

}

void server::incomingConnection(int socketID)
{

    int n;
    m.lock();
    if (numThread < maxThread){
        numThread++;
        n = numThread;
    } else {
        qDebug() << "connessione rifiutata, max utenti raggiunti";
        m.unlock();
        return;
    }
    m.unlock();

    qDebug() << "Connecting from " << socketID << " thread num: " << n;
    this->newThread = new s_thread(socketID);
    connect(newThread, &s_thread::deleteThreadSig, this, &server::deleteThread, Qt::ConnectionType::DirectConnection);
    // TODO exception

    newThread->run();

}
