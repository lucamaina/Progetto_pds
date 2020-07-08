#include "server.h"

server::server(QObject *parent) : QTcpServer (parent)
{
    Logger &lg = Logger::getLog();
    this->log = &lg;
    log->write("Start server con "+ QString::number(MAX_THREAD) + " thread");
    qDebug() << "Start server con "+ QString::number(MAX_THREAD) + " thread" << "[Password Hash3-256]";
    mioDB = new db(1);
    if (!mioDB->conn()){
        log->write("Impossibilile aprire db");
        qDebug() << "Impossibilile aprire db";
        return;
    }
    mioDB->setUpUtenti();
    tVec.reserve(MAX_THREAD);
    tVec.clear();

    //creazione cartella per i documenti
    QDir dir = QDir::current();

    if (!dir.exists("files"))
        dir.mkdir("files");
}

void server::startServer()
{
    if (!this->listen(QHostAddress::Any, 2000)) {
        qDebug() << "Errore in 'listen'";
    } else {
        qDebug() << "Listening..." << endl;
    }

}

/**
 * @brief server::saveAll
 * chiamato se capita eccezione, salva tutti gli editor
 */
void server::saveAll()
{
    this->m.lock();

    Network::getNetwork().salvaTutto();

    this->m.unlock();
}

void server::deleteThread(s_thread &t)
{
    m.lock();
    numThread--;
    tVec.removeOne(&t);
    t.exitThread();
    m.unlock();
    qDebug() << "sono in " << Q_FUNC_INFO;
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
        try {
            newThread->run();
        } catch (std::exception &e) {
            this->saveAll();
            log->write(e.what());
            qDebug() << "sono in server: " << e.what();
            std::cin >> a;
        }
    } else {
        m.unlock();
        qDebug() << "connessione rifiutata, max utenti raggiunti";
        return;
    }
}
