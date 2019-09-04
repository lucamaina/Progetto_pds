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

QSqlQuery db::query(QString querySrc)
{
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

QSqlQuery db::query(QString querySrc, QVector<QString> values)
{
    QSqlQuery query(this->myDb);
    if (myDb.isOpen()){
        qDebug() << "db aperto prima di exec";
    }
    query.prepare(querySrc);
    for (int idx = 0; idx < values.length(); idx++){
        query.bindValue(idx, values[idx]);
    }
    if (query.exec() == false){
        QSqlError err = query.lastError();
        qDebug() << err.text();
    }
    return query;
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


bool db::conn(utente & user){
    myDb.setUserName(user.getUsername());
    myDb.setPassword(user.getPass());
    bool ok = myDb.open();
    qDebug() << "database opened:" << ok << " connectionName: " << QString(myDb.connectionName());
    Logger::getLog().write("Nuova connessione da utente" + QString(myDb.userName()));
    user.setConn(ok);
    return ok;
}


bool db::userLogin(utente &user)
{
    QString s = user.getUsername();


    QString SQLquery;
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
    this->myDb.transaction();       // start transaction

    QVector<QString> values;
    values.push_back(user.getUsername());
    values.push_back(user.getPass());
    QSqlQuery res = this->query(queryLOGIN, values);

    res.next();
    int count = res.value(0).toInt();
    if (count != 0){
        this->myDb.rollback();
        return false;
    }
    values.clear();
    values.push_back(user.getUsername());
    values.push_back(user.getNick());
    values.push_back(user.getPass());
    res = this->query(queryREG, values);
    // verifica res
    this->myDb.commit();
    return false;
}

bool db::isOpen(){
    return this->myDb.isOpen();
}
