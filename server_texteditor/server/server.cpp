#include "server.h"

server::server(QObject *parent) : QTcpServer (parent)
{
    Logger &lg = Logger::getLog();
    this->log = &lg;
    log->write("Costruttore server");
    mioDB = new db(1);
    if (!mioDB->conn()){
        log->write("Impossibilile aprire db");
        qDebug() << "Impossibilile aprire db";
        return;
    }
    mioDB->setUpUtenti();
    tVec.resize(MAX_THREAD);
    tVec.clear();
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
    qDebug() << "delete: " << &t;
    t.~s_thread();
    tVec.removeOne(&t);

}

void server::incomingConnection(int socketID)
{

    int n;
    m.lock();
    if (numThread < maxThread){
        numThread++;
        n = numThread;

        qDebug() << "Connecting from " << socketID << " thread num: " << n;
        s_thread *newThread = new s_thread(socketID);
        tVec.push_back(newThread);
        connect(newThread, &s_thread::deleteThreadSig, this, &server::deleteThread, Qt::ConnectionType::DirectConnection);
        // TODO exception

        newThread->run();

    } else {
        qDebug() << "connessione rifiutata, max utenti raggiunti";
        m.unlock();
        return;
    }
    m.unlock();
}
