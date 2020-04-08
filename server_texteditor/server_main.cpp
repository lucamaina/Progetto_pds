/*
 * Classe per la gestione del flusso principale dei thread del server.
 * Thread padre apre connessione di default a db per verificare il funzionamento.
 *
 */


#include "server_main.h"
#include "logger/logger.h"
#include "QMap"
#include <QtGui/QTextCharFormat>
#include <QtFontDatabaseSupport/QtFontDatabaseSupport>
#include <memory>
#include <QDataStream>
void scriviXML();
void sum(int& a){
    qDebug() <<"sum: " << a+a;
}
server_main::server_main(QObject *parent) : QObject(parent)
{
    this->myServer = new server();
    myServer->startServer();

    Logger::getLog().write("Start new logger process");
}



void scriviXML(){
    QByteArray ba;
    QXmlStreamWriter wr(&ba);
    wr.writeStartDocument();
    wr.writeTextElement(FILES, "");
    wr.writeEndDocument();

    int dim = ba.size();
    QByteArray len;
    len = QByteArray::number(dim, 16);
    len.prepend(8 - len.size(), '0');

    ba.prepend(len);
    ba.prepend(INIT);
    qDebug() << QString(ba);

}
