#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QObject>
#include <QProgressDialog>
#include <QTcpSocket>
#include <iostream>
#include <QtNetwork/qhostaddress.h>
#include "cmdstring.h"
#include "comando.h"

class MySocket : public QObject
{
    Q_OBJECT
private:
    int sockId, dimFile;
    QTcpSocket sock;
    QByteArray buffer, command, fileBuffer;

public:
    explicit MySocket(int sockId);
    void leggiXML(QByteArray data);
    bool write(QByteArray data);
    bool write(QMap<QString, QString> comando);
    bool connectToHost();
    void connectReadyRead(bool set);
    void connectFileReadyRead(bool set);
    void startLoadFromRemote(int size);
    void stopLoadFromRemote(QByteArray &qba);
    QByteArray leggiMap(int size);

signals:
    void s_dispatchCmd(Comando &cmd);
    void s_dispatchCmd(QMap<QString, QString> &cmd);
    void s_disconnected();
    void s_connected();
    void s_loadFile(QByteArray& qba);

public slots:
    void readyRead();
    void fileReadyRead();
    void disconnected();
    void connected();
};

#endif // MYSOCKET_H
