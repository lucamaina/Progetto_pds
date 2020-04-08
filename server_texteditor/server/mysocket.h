#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <iostream>

#include "cmdstring.h"
#include "comando.h"

class MySocket : public QObject
{
    Q_OBJECT
private:
    int sockId;
    QTcpSocket sock;
    // nuova versione ready read
    QByteArray buffer, command;

public:
    MySocket(int sockId);
    // ~MySocket() = default;

    void leggiXML(QByteArray data);

    bool write(QByteArray data);
    bool write(QMap<QString, QString> comando);

signals:
    void s_dispatchCmd(Comando &cmd);
    void s_dispatchCmd(QMap<QString, QString> &cmd);
    void s_disconnected();

public slots:
    void readyRead();
    void disconnected();

//    void s_receiveCmd(Comando & cmd);

    //  void s_receiveCmd(QMap<QString, QString> & cmd);
};


#endif // MYSOCKET_H
