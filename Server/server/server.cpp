#include "server.h"

server::server(QObject *parent) : QTcpServer (parent)
{
    Logger &lg = Logger::getLog();
    this->log = &lg;
    log->write("Start server con "+ QString::number(MAX_THREAD) + " thread");
    qDebug() << "Start server con "+ QString::number(MAX_THREAD) + " thread";
    qDebug() << "Password Hash3-256";
    mioDB = new db(1);
    if (!mioDB->conn()){
        log->write("Impossibilile aprire db");
        qDebug() << "Impossibilile aprire db";
        return;
    }
    mioDB->setUpUtenti();
    tVec.reserve(MAX_THREAD);
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
    qDebug() << "sono in " << Q_FUNC_INFO;
    tVec.removeOne(&t);
    t.exitThread();
}

void server::incomingConnection(int socketID)
{
    char a;
    int n;
    m.lock();
    if (numThread < maxThread){
        numThread++;
        n = numThread;

        qDebug() << "Connecting from " << socketID << " thread num: " << n;

        tVec.push_back( new s_thread(socketID) );
        s_thread *newThread = tVec.last();
        m.unlock();

        connect(newThread, &s_thread::deleteThreadSig, this, &server::deleteThread, Qt::ConnectionType::DirectConnection);
        // connect(newThread, &s_thread::finished, newThread, &s_thread::test, Qt::DirectConnection);
        // TODO exception
        try {
            newThread->run();
        } catch (std::exception &e) {
            // TODO migliorare gestione
            log->write(e.what());
            qDebug() << "sono in server: " << e.what();
            std::cin >> a;
        }

    } else {
        qDebug() << "connessione rifiutata, max utenti raggiunti";
        return;
    }
}
