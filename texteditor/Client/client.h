#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/QTcpSocket>
#include "cmdstring.h"
#include <QXmlStreamWriter>

class Client : public QObject
{
    Q_OBJECT


public:
    explicit Client(QObject *parent = nullptr);

signals:

public slots:
    void connected();
    void handleLogin(QString& username, QString& password);
    void readyRead();

private:
    QString username, password;
    bool logged;
    QTcpSocket* socket;
    QByteArray buffer;


    /****************************************************************************
     ***************** metodi controllo dei comandi ricevuti ********************/

    void leggiXML(QByteArray data);
    void dispatchCmd(QMap<QString, QString> cmd);

    /****************************************************************************
     ***************** metodi controllo dei comandi inviati ********************/

    bool sendMsg(QMap<QString, QString> cmd);
    bool sendMsg(QByteArray ba);


};

#endif // CLIENT_H
