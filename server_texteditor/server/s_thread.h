#ifndef S_THREAD_H
#define S_THREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QMap>
#include <QString>

#include "utente/utente.h"
#include "database/db.h"
#include "cmdstring.h"

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
    QTcpSocket *socket = nullptr;
    utente *user = nullptr;
    QByteArray buffer;
    db *conn;

    /****************************************************************************
     * metodi controllo dei comandi ricevuti ************************************/
    void leggiXML(QByteArray data);
    void dispatchCmd(QMap<QString, QString> cmd);

    /****************************************************************************
     * metodi controllo dei messaggi inviati ************************************/
    bool scriviXML();
    bool sendMSG(QByteArray data);

    /****************************************************************************
     * metodi accesso a database *************************************************/
    void connectDB(QMap<QString, QString> comando);
    void loginDB(QMap<QString, QString> comando);
    void registerDB();

};

#endif // S_THREAD_H
