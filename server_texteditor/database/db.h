/**
asdq
  **/


#ifndef DB_H
#define DB_H

#include <QObject>
#include <QtSql>
#include "../utente/utente.h"
#include "../logger/logger.h"

class db : public QObject
{
    Q_OBJECT
public:
    explicit db(QObject *parent = nullptr);
    explicit db(int connName, QObject *parent = nullptr);
    bool conn();
    bool conn(utente & user);
    QSqlQuery query(QString querySrc);
    QSqlQuery query(QString querySrc, QVector<QString> values);
    bool userLogin(utente & user);
    bool userLogOut(utente & user);
    bool userReg(utente & user);
    bool isOpen(){  return this->myDb.isOpen(); }
    bool disconn(utente& user);
    bool addFile(utente & user, QString nomefile);
    bool canUserOpenFile(utente & user, QString nomefile);
    QMap<QString, QString> browsFile(utente & user);

signals:

public slots:

private:
    QSqlDatabase myDb;
    //utente user;
    Logger *log;

    void setUpUtenti();
};

#endif // DB_H
