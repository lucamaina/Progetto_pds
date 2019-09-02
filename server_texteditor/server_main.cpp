/*
 * Classe per la gestione del flusso principale dei thread del server.
 * Thread padre apre connessione di default a db per verificare il funzionamento.
 *
 */


#include "server_main.h"
#include "logger/logger.h"

server_main::server_main(QObject *parent) : QObject(parent)
{
    this->myServer = new server();
    myServer->startServer();

    Logger::getLog().write("Start new logger process");

    //mydb = new db();
}


