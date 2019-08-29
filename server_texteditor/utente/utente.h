#ifndef utente_H
#define utente_H

#include <QObject>
#include <QMap>
#include "../server/cmdstring.h"

class utente : public QObject
{
    Q_OBJECT
public:
    explicit utente(QString username, QString password, QObject *parent = nullptr);
    utente(){}
    QString getUsername();
    QString getNick();
    QString getPass();
    void setNick(QString nick);
    bool prepareUtente(QMap<QString, QString> map);

signals:


public slots:

private:
    QString username;
    QString nickname;
    QString password;
    bool isConnect = false;

};

#endif // utente_H





