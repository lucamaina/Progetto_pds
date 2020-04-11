#ifndef utente_H
#define utente_H

#include <QObject>
#include <QDebug>
#include <QMap>
#include <iostream>
#include <QCryptographicHash>

#include "../server/cmdstring.h"

class QTcpSocket;

class utente
{
private:
    QString username;
    QString nickname;
    QString password;
    bool isConnect = false;
    QTcpSocket *mioSocket;

public:
    utente(QString username, QString password)
        : username(username), password(password){}
    utente(){}
    ~utente() = default;

public:
    QString getUsername();
    QString getNick();
    QString getPass();
    void setNick(QString nick);
    void setSocket(QTcpSocket &sock);
    bool prepareUtente(QMap<QString, QString> map);
    bool prepareUtente(QMap<QString, QString> map, bool isConnect);
    bool isConnected(){ return this->isConnect; }
    void setConn(bool setConn);
    QTcpSocket *getSocket() const;
    void clear(){
        username = nickname = password = "";
        isConnect = false;
        mioSocket = nullptr;
    }

};

#endif // utente_H





