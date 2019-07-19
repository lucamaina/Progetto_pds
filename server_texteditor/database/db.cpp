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

QSqlQuery db::query(QString querySrc)
{
    QSqlQuery query;
    if (query.exec(querySrc) == false){
        QSqlError err = query.lastError();
        qDebug() << err.text();
    }
    return query;
}

bool db::userLogin(utente &user)
{
    QString SQLquery;
    SQLquery = "SELECT NickName FROM utenti WHERE UserName = " + user.getUsername() + " AND Password = " + user.getPass();
    QSqlQuery res = this->query(SQLquery);

    while (res.next()){
        QString nick = res.value(0).toString();
        user.setNick(nick);
        return true;
    }
    return false;
}

bool db::userReg(utente &user)
{
    // @TODO transazione
    QString SQLquery;
    SQLquery = "SELECT COUNT(*) FROM utenti WHERE UserName = " + user.getUsername();

    QSqlQuery res = this->query(SQLquery);

    while (res.next()){
        QString presente = res.value(0).toString();
        if (presente == "0") {
            // posso registrare
            SQLquery = "INSERT INTO utenti (UserName, NickName, Password) VALUES ("
                    + user.getUsername() +", "
                    + user.getNick() +", "
                    +user.getPass() +")";
            this->query(SQLquery);

            return true;
        } else {
            return false;
        }
    }
    return false;
}
