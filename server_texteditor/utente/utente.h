#ifndef utente_H
#define utente_H

#include <QObject>
#include <QMap>
#include <iostream>
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
public:
    QString getUsername();
    QString getNick();
    QString getPass();
    void setNick(QString nick);
    void setSocket(QTcpSocket *sock);
    bool prepareUtente(QMap<QString, QString> map);
    bool isConnected(){ return this->isConnect; }
    void setConn(bool setConn);
    QTcpSocket *getSocket() const;

signals:


public slots:


};

#endif // utente_H





