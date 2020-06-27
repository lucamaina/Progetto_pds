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

#include "mysocket.h"

class s_thread : public QThread
{
    Q_OBJECT

public:
    s_thread(int ID, QObject *parent = nullptr);
    void exitThread();
    void save();
    void run();

public slots:
    void disconnected();
    void dispatchCmd(QMap<QString, QString> &cmd);
    void dispatchCmd(Comando &cmd);

signals:
    void deleteThreadSig(s_thread &t);

private:
    int sockID;
    QString docID;

    QSharedPointer<MySocket> sp_socket;    // socket con shared pointer per QObject
    std::unique_ptr<utente> up_user;
    std::unique_ptr<db> up_conn;

    QStringList toQStringList(QMap<QString, QString> cmd);

    bool sendBody(QByteArray &ba);
    bool verifyCMD(QMap<QString, QString> &cmd, const QList<QString> &list);

    /****************************************************************************
     * metodi controllo dei messaggi inviati ************************************/
    bool clientMsg(QByteArray data);
    bool clientMsg_(QMap<QString, QString> comando);
    bool clientMsg(QMap<QString, QString> comando);

    /****************************************************************************
     * metodi accesso a database *************************************************/
    void connectDB();
    void disconnectDB();
    void loginDB(QMap<QString, QString> &comando);
    void logoffDB(QMap<QString, QString> &comando);
    void logOffDB();
    void registerDB(QMap<QString, QString> &comando);
    void addFileDB(QMap<QString, QString> &comando);
    void browsFile(QMap<QString, QString> &comando);
    void openFile(QMap<QString, QString> &comando);
    void exitClientFromFile(QMap<QString, QString> &comando);
    void getUsers(QMap<QString, QString> &comando);
    void addUsersDB(QMap<QString, QString> &comando);
    void remUsersDB(QMap<QString, QString> &comando);

    /****************************************************************************
     * metodi accesso a netowrk *************************************************/
    void sendMsg(QMap<QString, QString> comando);
};

#endif // S_THREAD_H
