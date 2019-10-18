#ifndef CLIENT_H
#define CLIENT_H

#include <BrowseWindow/browsewindow.h>
#include <QObject>
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/QTcpSocket>
#include "cmdstring.h"
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <QTextEdit>

class Client : public QObject
{
    Q_OBJECT


public:
    explicit Client(QObject *parent = nullptr);
    void handleBrowse(QMap<QString,QString> cmd);
    void remoteInsert(QChar& c, int posx, int posy);
    void inserimento(QMap<QString,QString> cmd);
signals:

    void spostaCursSignal(int& posX, int& posY,char& car, QString& user);
    void deleteListSig(); //cancella la lista di utenti online dal widget

public slots:
    void connected();
    void disconnected();
    void handleLogin(QString& username, QString& password);
    void handleLogout();
    void handleRegistration(QString& username, QString& password);
    void readyRead();
    void handleMyCursorChange(int& posX,int& posY);
    void remoteOpen(QString& name);
    void remoteAdd(QString& name);
    bool sendMsg(QMap<QString, QString> cmd);
    bool sendMsg(QByteArray ba);

private:
    QMap<QString,QString> files;
    QString username;
    QString docID;
    bool logged,connectedDB;
    QTcpSocket* socket;
    QByteArray buffer;
    QString tempUser;
    QString tempPass;

    /****************************************************************************
     ***************** metodi controllo dei comandi ricevuti ********************/

    void leggiXML(QByteArray data);
    void dispatchCmd(QMap<QString, QString> cmd);
    void dispatchOK(QMap <QString, QString> cmd);
    void dispatchERR(QMap <QString,QString> cmd);
    void spostaCursori(QMap <QString,QString> cmd);
    /****************************************************************************
     ***************** metodi controllo dei comandi inviati ********************/




};

#endif // CLIENT_H
