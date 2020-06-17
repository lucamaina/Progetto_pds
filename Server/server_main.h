#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H

#include "server/server.h"

/** modifiche per togliere qobject non usato **/

class server_main /*: public QObject*/
{
//    Q_OBJECT

private:
    // db* mydb;
    server *myServer;

public:
    explicit server_main() /*QObject *parent = nullptr);*/
    {
        this->myServer = new server();
        myServer->startServer();
        Logger::getLog().write("Start new logger process");
    }
};

#endif // SERVER_MAIN_H
