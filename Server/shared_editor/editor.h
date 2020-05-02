#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QMap>
#include <mutex>
#include <QDebug>
#include <QTcpSocket>
#include <QPointer>
#include <limits>
#include <logger/logger.h>
#include <exception>

#include "message.h"
#include "symbol.h"
#include "../server/comando.h"

class MySocket;
class Network;

class Editor : public QObject
{
    Q_OBJECT

private:
    // std::mutex m;
    QString nomeFile;
    QString DocId;      // id usato nel db

    QFile *file;
    qint32 dim;
    qint32 maxDim = std::numeric_limits<qint32>::max();

    QVector<Symbol> symList;
    QMap<QString, QSharedPointer<MySocket>> sendList_;


public:
    explicit Editor(QString Id, QString nome);

    static Editor& getFile();
    bool loadMap();
    bool sendMap(QString nomeUtente);
    bool sendBody(QSharedPointer<MySocket> &sock);

    QByteArray getSymMap();

    bool loadFile(const QString &nomeUser, const int dimFile);        // leggo socket dell'utente e carico il file
    ~Editor();

        bool save();
        bool isEmpty();

    /****************************************************************************
     * metodi controllo utenti **************************************************/
    bool addUser(utente &nomeUser, QSharedPointer<MySocket> &sock);

    bool removeUser(const QString &nomeUser);  // utente& nomeUser
    bool findUser(const QString &nomeUser);

    /****************************************************************************
     * metodi di insert e delete ************************************************/
    bool localInsert(Message msg);
    bool localDelete(Message msg);

    int localPosCursor(QVector<qint32> &index);
    int findLocalPosCursor(QVector<qint32> & index);

    bool deserialise(Message msg);

    bool cursorChange(Message msg);
    bool remoteSend(Message msg);

    bool send(QSharedPointer<MySocket> &sock, QByteArray ba);

    bool sendToAll(Comando &cmd);
    bool rispErr(Message &msg);
    bool process(Message& msg);

    /****************************************************************************
     * metodi di serialize e deserialize ****************************************/
    bool serialize();
    bool deserialise(QByteArray &ba);


private:


    // metodi non definiti
    Editor(const Editor & hold) = delete;
    const Editor &operator=(const Editor & hold) = delete;

signals:

public slots:


};

#endif // EDITOR_H
