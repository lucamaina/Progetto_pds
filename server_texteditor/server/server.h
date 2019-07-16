#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <database/db.h>
#include <QTcpServer>
#include <QDebug>
#include "s_thread.h"

class server : public QTcpServer
{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);
    void startServer();

signals:

public slots:

private:
    void incomingConnection(int socketID);

};

#endif // SERVER_H
