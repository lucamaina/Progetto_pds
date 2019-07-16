/*
 * Classe per gestione delle connessioni al database MYSQL in localhost.
 * Ogni thread figlio deve gestire la sua connessione.
 *
 *
 */

#include "db.h"

db::db(QObject *parent) : QObject(parent)
{
    this->myDb = QSqlDatabase::addDatabase("QMYSQL");
    myDb.setHostName("localhost");
    myDb.setDatabaseName("web_editor");
    myDb.setUserName("serverUser");
    myDb.setPassword("pass");
    bool ok = myDb.open();
    qDebug() << "database opened:" << ok;
}

bool db::conn()
{
    //TODO
    return false;
}

void db::query(QString querySrc)
{
    QSqlQuery query;
    if (query.exec(querySrc) == false){
        QSqlError err = query.lastError();
        qDebug() << err.text();
    }
}
