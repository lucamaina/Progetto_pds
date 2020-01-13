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
#include <memory>

#include "../shared_editor/editor.h"
#include "../shared_editor/network.h"
#include "../shared_editor/message.h"

class s_thread : public QThread
{
    Q_OBJECT

public:
    s_thread(int ID, QObject *parent = nullptr);
    ~s_thread();
    void run();

public slots:
    void readyRead();
    void disconnected();

signals:
    void deleteThreadSig(s_thread &t);

private:
    int sockID;
    QString docID = "";
    QTcpSocket *socket = nullptr;
//    std::shared_ptr<QTcpSocket> socket;
    //utente *user = nullptr;     // usare unique_ptr
    std::unique_ptr<utente> up_user;
    //db *conn;
    std::unique_ptr<db> up_conn;

    // nuova versione ready read
    QByteArray buffer, command;

    void readBody(QByteArray &ba, int dim);


    /****************************************************************************
     * metodi controllo dei comandi ricevuti ************************************/
    void leggiXML(QByteArray data);
    void dispatchCmd(QMap<QString, QString> cmd);

    /****************************************************************************
     * metodi controllo dei messaggi inviati ************************************/
    bool scriviXML(QMap<QString, QString> comando);
    bool clientMsg(QByteArray data);
    bool clientMsg(QMap<QString, QString> comando);

    bool sendBody(QByteArray &ba);


    /****************************************************************************
     * metodi accesso a database *************************************************/
    void connectDB();
    void disconnectDB();
    void loginDB(QMap<QString, QString> &comando);
    void logoffDB(QMap<QString, QString> &comando);
    void registerDB(QMap<QString, QString> &comando);
    void addFileDB(QMap<QString, QString> &comando);
    void browsFile(QMap<QString, QString> &comando);
    void openFile(QMap<QString, QString> &comando);
    void loadFile(QMap<QString, QString> &comando);

    /****************************************************************************
     * metodi accesso a netowrk *************************************************/
    void sendMsg(QMap<QString, QString> comando);

    bool verifyCMD(QMap<QString, QString> &cmd, QList<QString> &list);
};

#endif // S_THREAD_H
