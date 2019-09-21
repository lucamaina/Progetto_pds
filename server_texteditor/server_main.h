#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H

#include <QObject>
#include <database/db.h>
#include "server/server.h"

class server_main : public QObject
{
    Q_OBJECT

private:
    db* mydb;
    server *myServer;

public:
    explicit server_main(QObject *parent = nullptr);



signals:

public slots:
};

#endif // SERVER_MAIN_H
