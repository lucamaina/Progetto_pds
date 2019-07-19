#ifndef DB_H
#define DB_H

#include <QObject>
#include <QtSql>
#include "utente/utente.h"

class db : public QObject
{
    Q_OBJECT
public:
    explicit db(QObject *parent = nullptr);
    bool conn();
    QSqlQuery query(QString querySrc);
    bool userLogin(utente &user);
    bool userReg(utente &user);


signals:

public slots:

private:
    QSqlDatabase myDb;


};

#endif // DB_H
