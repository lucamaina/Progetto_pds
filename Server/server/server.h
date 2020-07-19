#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <database/db.h>
#include <QTcpServer>
#include <QDebug>
#include <exception>

#include "../database/db.h"
#include "s_thread.h"
#include "cmdstring.h"
#include "../shared_editor/network.h"



class server : public QTcpServer
{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);
    void startServer();
    void saveAll();

public slots:
    void deleteThread(s_thread &oldThread);

private:
    Logger *log;
    s_thread *newThread;
    std::mutex mutexLockThreadPool;
    int numThread = 0;
    int maxThread = MAX_THREAD;
    std::shared_ptr<db> mioDB;
    QVector<s_thread*> threadPool;

    void incomingConnection(int socketID);
    void manageFilesDirectory();
    void startNewThread(s_thread *newThread);
};

#endif // SERVER_H
