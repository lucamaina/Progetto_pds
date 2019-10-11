#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QMap>
#include <mutex>
#include <QDebug>
#include <QTcpSocket>
#include "message.h"
#include "symbol.h"

class s_thread;

class Editor : public QObject
{
    Q_OBJECT

private:
    // std::mutex m;
    QString nomeFile;
    QString DocId;      // id usato nel db
    QFile *file;
    int refCount;
    // QVector<Symbol> *symVec;
    QMap<double, Symbol> symMap;
    QVector<utente> userList;
    QMap<QString, QTcpSocket*> sendList;


public:
    explicit Editor(QObject *parent = nullptr);
    explicit Editor(QString Id, QString nome);

    static Editor& getFile();
    bool loadMap();
    bool sendMap(QString nomeUtente);
    bool sendBody(QTcpSocket *sock);

    QString mapToSend();
    ~Editor();

    /****************************************************************************
     * metodi controllo utenti **************************************************/
    bool addUser(utente &nomeUser);
    bool removeUser(const QString &nomeUser);  // utente& nomeUser
    bool findUser(const QString &nomeUser);

    /****************************************************************************
     * metodi di insert e delete ************************************************/
    bool localInsert(Message msg);
    bool localDelete(Message msg);
    bool cursorChange(Message msg);
    bool remoteSend(Message msg);
    bool process(Message& msg);
    bool send(QTcpSocket *t, QByteArray ba);


private:
    bool save();

    // metodi non definiti
    Editor(const Editor & hold) = delete;
    const Editor &operator=(const Editor & hold) = delete;

signals:

public slots:


};

#endif // EDITOR_H
