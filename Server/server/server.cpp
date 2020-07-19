#include "server.h"

server::server(QObject *parent) : QTcpServer (parent)
{
    this->log = &Logger::getLog();
    log->write("Start server con "+ QString::number(MAX_THREAD) + " thread");
    qDebug() << "Start server con "+ QString::number(MAX_THREAD) + " thread" << "[Password Hash3-256]";
    mioDB = std::make_shared<db>(db(1));
    if (!mioDB->conn()){
        log->write("Impossibilile aprire db");
        qDebug() << "Impossibilile aprire db";
        return;
    }
    mioDB->setUpUtenti();
    threadPool.reserve(MAX_THREAD);
    threadPool.clear();
    this->manageFilesDirectory();
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
    this->mutexLockThreadPool.lock();

    Network::getNetwork().salvaTutto();

    this->mutexLockThreadPool.unlock();
}

void server::deleteThread(s_thread &oldThread)
{
    mutexLockThreadPool.lock();
    numThread--;
    threadPool.removeOne(&oldThread);
    oldThread.exitThread();
    mutexLockThreadPool.unlock();
    this->log->write("Client distrutto: " + QString::number(oldThread.getSockID()) );
    qDebug() << "sono in " << Q_FUNC_INFO << " Client distrutto: " << oldThread.getSockID();
}

void server::incomingConnection(int socketID)
{
    int actualThreadNumber;
    mutexLockThreadPool.lock();
    if (numThread < maxThread){
        numThread++;
        actualThreadNumber = numThread;

        qDebug() << "Connecting from " << socketID << " thread num: " << actualThreadNumber;

        threadPool.push_back( new s_thread(socketID) );
        s_thread *newThread = threadPool.last();
        mutexLockThreadPool.unlock();

        connect(newThread, &s_thread::sigDeleteThread, this, &server::deleteThread, Qt::ConnectionType::DirectConnection);
        startNewThread(newThread);
    } else {
        mutexLockThreadPool.unlock();
        qDebug() << "connessione rifiutata, max utenti raggiunti";
        return;
    }
}

void server::manageFilesDirectory()
{
    QDir dir = QDir::current();
    if (!dir.exists("files"))
        dir.mkdir("files");
}

void server::startNewThread(s_thread *newThread)
{
    if (newThread == nullptr){
        return;
    }

    try {
        newThread->connectDB(this->mioDB);
        newThread->run();
    } catch (std::exception &e) {
        this->saveAll();
        log->write(e.what());
        qDebug() << "sono in server: " << e.what();
    } catch (...) {
        qDebug() << "qualsiasi eccezione da " << newThread->getSockID();
        log->write("qualsiasi eccezione"+ QString::number(newThread->getSockID()));
    }
}
