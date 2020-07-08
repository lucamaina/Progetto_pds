#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <iostream>
#include <QBuffer>

#include "cmdstring.h"
#include "comando.h"

class MySocket : public QObject
{
    Q_OBJECT
private:
    int sockId;
    QTcpSocket sock;
    QByteArray buffer, command;

public:
    explicit MySocket(int sockId);
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
};

#endif // MYSOCKET_H
