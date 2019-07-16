/*
 * Classe per la gestione del flusso principale dei thread del server.
 * Thread padre apre connessione di default a db per verificare il funzionamento.
 *
 */


#include "server_main.h"

server_main::server_main(QObject *parent) : QObject(parent)
{
    this->myServer = new server();
    myServer->startServer();
    //mydb = new db();
}


