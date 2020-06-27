#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/QTcpSocket>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextCharFormat>

#include "BrowseWindow/browsewindow.h"
#include "../NuovoFileRemotoWindoow/nuovofileremoto.h"
#include "../NuovoFileRemotoWindoow/usermanager.h"
#include "cmdstring.h"
#include "shared_editor/editor.h"

class Client : public QObject
{
    Q_OBJECT


public:
    explicit Client(QObject *parent = nullptr);

    // gestori chiamate da icone di texteditor
    void handleBrowse(QMap<QString,QString> cmd);
    bool handleNuovoFile();
    void handleAddUser();
    void handleFileExit();
    void listUser(QMap<QString,QString> cmd);
    void connectToHost();

    bool remoteInsert(QChar c, QTextCharFormat format, QVector<qint32> index);  // vecchia -> bool remoteInsert(QChar& c, QTextCharFormat format, double index, int posy, int anchor);
    bool remoteInsert(Symbol sym);

    bool remoteDelete(QChar c, QVector<qint32> index);
    bool remoteDelete(Symbol s);

    void inserimentoRemoto(QMap<QString,QString> cmd);
    bool inserimentoLocale(qint32 pos, QChar car, QTextCharFormat format);

    bool cancellamentoLocale(int posCursor);
    void cancellamentoRemoto(QMap<QString,QString> cmd);

    void loadFile(QMap<QString,QString> cmd);
    QByteArray serialize(const QTextCharFormat &format);
    QTextCharFormat deserialize(QByteArray &s);

    bool isLogged(void){
        return this->logged;
    }

    /****************************************************************************
     ***************** metodi gestione cursori **********************************/

    bool upCursor(QMap<QString,QString> cmd);

    // puntatore all'editor remoto
    Editor* remoteFile;
    QString getUsername() const;

signals:
    void s_changeTitle(QString utente, QString nomeFile, QString docid);
    void addMe();
    void s_upCursor(QStringList&);
    void s_changeCursor(QString& nomeUser, int pos);
    void s_setVisbleFileActions(bool set);
    void s_setVisbleEditorActions(bool set);
    void spostaCursSignal(int& pos, int& anchor, char& car, QString& user); //fa anche l'inserimento
    void cancellaSignal(int& posX, int& posY, int& anchor,char& car, QString& user);
    void deleteListSig(); //cancella la lista di utenti online dal widget
    void cambiaFile(QString& file);
    void nuovoStile(QString& stile,QString& param);
    void openFileSignal(QString& filename);
    void s_clearEditor();
    void toStatusBar(QString s);
    void s_brows();
    void s_setText(QChar c, QTextCharFormat f, int posCur);
    void s_removeText(int posCur);
    void s_logOut();
    void s_loadEditor(QString& str);

    void s_userList(QList<QString> &list);
    void s_warning(QString str);

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
    void handleMyCursorChange(int& pos, int& anchor);
    void remoteOpen(QString& name, QString &docID);

    void remoteAdd(QString& name);

    bool sendMsg(QMap<QString, QString> cmd);
    bool sendMsg(QByteArray ba);

    void sendAddUsers(QStringList& lista);
    void sendRemoveUsers(QStringList& lista);


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
    QByteArray buffer_, command;
    UserManager *finestraUsers;
    nuovoFileRemoto *finestraAddFile;

    /****************************************************************************
     ***************** metodi controllo dei comandi ricevuti ********************/

    bool leggiXML(QByteArray data);
    void dispatchCmd(QMap<QString, QString> cmd);
    void dispatchOK(QMap <QString, QString> cmd);
    void dispatchERR(QMap <QString,QString> cmd);
    void spostaCursori(QMap <QString,QString> cmd);

    /****************************************************************************
     ***************** metodi controllo dei comandi inviati ********************/

    void showUserError(QMap<QString, QString> cmd);

};

#endif // CLIENT_H
