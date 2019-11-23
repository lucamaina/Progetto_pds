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
#include "shared_editor/editor.h"
#include <QTextCharFormat>

class Client : public QObject
{
    Q_OBJECT


public:
    explicit Client(QObject *parent = nullptr);
    void handleBrowse(QMap<QString,QString> cmd);
    void remoteInsert(QChar& c, QTextCharFormat format, int posx, int posy, int anchor);
    void remoteDelete(QChar& c, int posx, int posy, int anchor);
    void inserimento(QMap<QString,QString> cmd);
    void nuovoFile(QMap<QString,QString> cmd);
    QString serialize(const QTextCharFormat &format);
    QTextCharFormat deserialize(QByteArray &s);



    Editor* remoteFile;


signals:
    void addMe();
    void spostaCursSignal(int& posX, int& posY, int& anchor, char& car, QString& user); //fa anche l'inserimento
    void cancellaSignal(int& posX, int& posY, int& anchor,char& car, QString& user);
    void deleteListSig(); //cancella la lista di utenti online dal widget
    void cambiaFile(QString& file);
    void nuovoStile(QString& stile,QString& param);
    // void openFileSignal(QString& filename);

public slots:
    void connected();
    void disconnected();
    void connectSlot();
    void handleLogin(QString& username, QString& password);
    void handleLogout();
    void dispatchStile(QMap <QString,QString>cmd);
    void handleRegistration(QString& username, QString& password);
    void handleStile(QString& stile,QString& param);
    void readyRead();
    void pasteSlot(QString& clipboard);
    void handleMyCursorChange(int& posX,int& posY, int& anchor);
    void remoteOpen(QString& name, QString &docID);
    void remoteAdd(QString& name);
    bool sendMsg(QMap<QString, QString> cmd);
    bool sendMsg(QByteArray ba);
    void handleNuovoFile(QString& filename);

private:
    QMap<QString,QString> files;
    QString username;
    QString docID;
    QString filename;
    bool logged,connectedDB;
    QTcpSocket* socket;
    QByteArray buffer;
    QString tempUser;
    QString tempPass;
    // nuova versione ready read
    QByteArray buffer_, command;
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
