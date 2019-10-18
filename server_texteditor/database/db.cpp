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
    if (!myDb.isOpen()){
        qDebug() << "err db non aperto prima di exec query";
    }
    if (query.exec(querySrc) == false){
        QSqlError err = query.lastError();
        qDebug() << err.text();
    }
    qDebug() << "last query" << query.executedQuery();
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
    qDebug() << "last query" << query.executedQuery() << " values: " << values;
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
    Logger::getLog().write("Nuova connessione con connectionName: " + QString(myDb.connectionName()));
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
    this->myDb.transaction();   // apro transazione

    QVector<QString> values;
    values.push_back(user.getUsername());
    values.push_back(user.getPass());
    qDebug() << values;
    QSqlQuery res = this->query(queryLOGIN, values);

    if (res.first()){
        QString s = res.value(1).toString();
        bool connesso = res.value(0).toBool();
        qDebug() <<"res: "<< connesso << " s: "<<s;
        if (connesso == true){
            // utente già connesso
            this->myDb.rollback();      // chiudo transazione con rollback
            return false;
        } else {
            values.clear();
            values.push_back(user.getUsername());
            QSqlQuery res = this->query(queryUpdateLOGIN, values);
            user.setNick(s);
            this->myDb.commit();    // chiudo connessione con commit
            return true;
        }

    }
    this->myDb.rollback();      // chiudo transazione con rollback
    return false;
}

bool db::userReg(utente &user)
{
    this->myDb.transaction();       // start transaction

    QVector<QString> values;
    values.push_back(user.getUsername());
    values.push_back(user.getPass());
    QSqlQuery res = this->query(queryPreReg, values);

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
    return true;
}

bool db::userLogOut(utente &user)
{
    QVector<QString> values;
    values.push_back(user.getUsername());
    QSqlQuery res = this->query(queryLOGOUT, values);
    return true;
}

/**
 * @brief db::disconn
 * @param user
 * @return
 * Segna l'user come non loggato e chiude connessione al db
 */
bool db::disconn(utente &user){
    this->userLogOut(user);
    if ( !this->myDb.isOpen() ){    return false;   }
    this->myDb.close();
    return true;
}

/**
 * @brief db::addFile
 * @param user
 * @param nomefile
 * @return
 * verifica presenza di user e assenza di file con stesso nome, aggiunge file e lo collega a user
 */
bool db::addFile(utente &user, QString nomefile)
{
    bool connesso;
    this->myDb.transaction();
    // verifica utente
    QVector<QString> val;
    val.push_back(user.getUsername());
    val.push_back(user.getPass());
    QSqlQuery res = this->query(queryLOGIN, val);
    if (res.first()){
        QString nick = res.value(1).toString();
        connesso = res.value(0).toBool();
        if (!connesso){
            qDebug() << "utente non connesso " << user.getUsername();
            this->myDb.rollback();
            return false;
        }
    } else {
        qDebug() << "utente non connesso " << user.getUsername();
        this->myDb.rollback();
        return false;
    }
    val.clear();

    // verifica file
    val.push_back(nomefile);
    res = this->query(queryFILEexist, val);
    if (res.first()){
        int count = res.value(0).toInt();
        if (count != 0){
            qDebug() << "file già presente" << user.getUsername();
            this->myDb.rollback();
            return false;
        } else {
            qDebug() << "file aggiungibile" << user.getUsername();
        }
    }
    val.clear();

    // aggiunge file
    val.push_back(nomefile);
    val.push_back(nomefile);
    res = this->query(queryFILEadd, val);
    if (!res.isValid()){
        this->myDb.rollback();
        return false;
    }
    val.clear();

    // collega file - utente


    return true;
}

bool db::canUserOpenFile(utente &user, QString FileId)
{
    QVector<QString> val;
    val.push_back(user.getUsername());
    val.push_back(FileId);
    QSqlQuery res = this->query(queryOPEN, val);
    if (res.first()){
        int count = res.value(0).toInt();
        if (count == 0)
            return false;
        else
            return true;
    }
    return false;
}

QMap<QString, QString> db::browsFile(utente &user)
{
    int counter = 1;
    QMap<QString, QString> risp;
    risp.insert(CMD, BROWS);
    QVector<QString> values;
    values.push_back(user.getUsername());
    qDebug() << values;
    QSqlQuery res = this->query(queryBROWS, values);

    while (res.next()){
        QString DocId = res.value(0).toString();
        QString FileSysPath = res.value(1).toString();
        QString LinkPath = res.value(2).toString();
        risp.insert(FNAME + QString::number(counter), FileSysPath);
        risp.insert(DOCID + QString::number(counter), DocId);
        counter++;
        qDebug() <<"res: "<< DocId << ", "<<FileSysPath<<", "<<LinkPath;
    }
    return risp;
}

void db::setUpUtenti()
{
    QSqlQuery res = this->query(querySetUpUtenti);
}
