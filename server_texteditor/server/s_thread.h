#ifndef S_THREAD_H
#define S_THREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include "utente/utente.h"
#include "database/db.h"

#include <QXmlStreamWriter>

class s_thread : public QThread
{
    Q_OBJECT

public:
    s_thread(int ID, QObject *parent = nullptr);
    void run();

public slots:
    void readyRead();
    void disconnected();

private:
    int sockID;
    QTcpSocket *socket;
    utente *user;
    QBuffer buffer;

    bool scriviXML();
    void leggiXML(uint dim);


};

#endif // S_THREAD_H
