#ifndef DB_H
#define DB_H

/****************************************************************************
 * query prepared definite **************************************************/
#define queryLOGIN "SELECT connesso, NickName FROM utenti WHERE UserName = ? AND Password = ?;"
#define queryUpdateLOGIN "UPDATE utenti SET connesso = '1' WHERE UserName = :UserName;"

#define queryLOGOUT "UPDATE utenti SET connesso = '0' WHERE UserName = :UserName;"

#define queryPreReg "SELECT Count(*) FROM utenti WHERE UserName = :UserName AND Password = :Password FOR UPDATE;"
#define queryREG "INSERT INTO utenti(UserName, NickName, Password, connesso) VALUE (:UserName, :NickName, :Password, '1');"

#define queryFILEexist "SELECT Count(*) FROM documenti WHERE FileSysPath = :FileSysPath;"
#define queryFILEadd "INSERT INTO documenti(FileSysPath, LinkPath) VALUE (:FileSysPath, :LinkPath);"

#define queryBROWS "SELECT * FROM documenti WHERE DocId IN (SELECT DocId FROM relazioni WHERE UserName = :UserName);"
#define queryOPEN "SELECT Count(*) FROM relazioni WHERE UserName = :UserName AND DocId = :DocId;"

#define querySetUpUtenti "UPDATE utenti SET connesso = '0';"

#define queryPROVA "SELECT connesso FROM utenti where UserName = 'asd' AND Password = '1' FOR UPDATE;"
#define queryP "SELECT connesso FROM utenti where UserName = ?;"

#define queryGetDocId "SELECT MAX(DocId) FROM relazioni as r, documenti as d WHERE r.DocId = d.DocId AND r.UserName = :UerName AND d.FileSysPath = :nomeFile;"


#include <QtSql>
#include "../utente/utente.h"
#include "../logger/logger.h"

class db
{

public:
    explicit db();
    explicit db(int connName);
    bool conn();
    bool conn(utente & user);
    void setUpUtenti();
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
    QString getDocId(const QString nomeFile, const QString nomeUtente);

private:
    QSqlDatabase myDb;
    //utente user;
    Logger *log;


};

#endif // DB_H
