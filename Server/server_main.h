#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H

#include "server/server.h"

class server_main
{

private:
    server *myServer;

public:
    explicit server_main()
    {
        this->myServer = new server();
        myServer->startServer();
        Logger::getLog().write("Start new logger process");
    }
};

#endif // SERVER_MAIN_H
