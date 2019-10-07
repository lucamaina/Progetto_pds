#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <database/db.h>
#include <QTcpServer>
#include <QDebug>
#include "s_thread.h"
#include "../shared_editor/network.h"

class server : public QTcpServer
{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);
    void startServer();

signals:

public slots:
    void deleteThread();

private:
    void incomingConnection(int socketID);

    Logger *log;    
    s_thread *newThread;

};

#endif // SERVER_H
