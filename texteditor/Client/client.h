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
    void disconnected();
    void handleLogin(QString& username, QString& password);
    void handleRegistration(QString& username, QString& password);
    void readyRead();
    void handleMyCursorChange(int& posX,int& posY);

private:
    QString username;
    bool logged,connesso;
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
