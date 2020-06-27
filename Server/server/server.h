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
    void deleteThread(s_thread &t);

private:
    Logger *log;
    s_thread *newThread;
    std::mutex m;
    int numThread = 0;
    int maxThread = MAX_THREAD;
    db *mioDB;
    QVector<s_thread*> tVec;

    void incomingConnection(int socketID);
};

#endif // SERVER_H
