/*
 * Classe per gestione delle connessioni al database MYSQL in localhost.
 * Ogni thread figlio deve gestire la sua connessione.
 *
 *
 */

#include "db.h"

/**
 * @brief db::db
 * @param parent
 */
db::db(int connName, QObject *parent) : QObject(parent)
{
    this->myDb = QSqlDatabase::addDatabase("QMYSQL", QString::number(connName));
    myDb.setHostName("localhost");
    myDb.setDatabaseName("web_editor");
    Logger::getLog().write("Nuova connessione di nome " + QString::number(connName));

/* spostato in conn
    {
        myDb.setUserName("serverUser");
        myDb.setPassword("pass");
        bool ok = myDb.open();
        qDebug() << "database opened:" << ok;
    }
*/
}

/**
 * @brief db::conn
 * @return
 */
bool db::conn()
{
    myDb.setUserName("serverUser");
    myDb.setPassword("pass");
    bool ok = myDb.open();

    qDebug() << "database opened:" << ok << " connectionName: " << QString(myDb.connectionName());
    // TODO aggiungere log
    return ok;
}

QSqlQuery db::query(QString querySrc)
{
    qDebug() << QSqlDatabase::drivers();

    QSqlQuery query(this->myDb);
    if (myDb.isOpen()){
        qDebug() << "db aperto prima di exec";
    }
    if (query.exec(querySrc) == false){
        QSqlError err = query.lastError();
        qDebug() << err.text();
    }
    return query;
}


bool db::conn(utente & user){
    myDb.setUserName(user.getUsername());
    myDb.setPassword(user.getPass());
    bool ok = myDb.open();
    qDebug() << "database opened:" << ok << " connectionName: " << QString(myDb.connectionName());
    Logger::getLog().write("Nuova connessione da utente" + QString(myDb.userName()));
    return ok;
}


bool db::userLogin(utente &user)
{
    QString s = user.getUsername();


    QString SQLquery;
    // SQLquery = "SELECT NickName FROM utenti WHERE UserName = " + user.getUsername() + " AND Password = " + user.getPass();
    SQLquery = "SELECT NickName FROM utenti WHERE UserName = 'asd' AND Password = '1'";

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

bool db::isOpen(){
    return this->myDb.isOpen();
}
