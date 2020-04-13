#ifndef MESSAGE_H
#define MESSAGE_H

#include <QMap>
#include <QXmlStreamWriter>
#include <QDebug>

#include "symbol.h"
#include "../utente/utente.h"
#include "../server/cmdstring.h"

class Message
{
public:
    enum msgType {Rem_In, Rem_Del, Cur};

private:
    msgType tipo;
    Symbol sym;
    QString user;
    QString fileId;
    QVector<qint32> indici;

public:
    explicit Message();
    bool prepareMsg(QMap<QString, QString> comando, QString username);
    msgType getTipo() const;
    Symbol getSym() const;
    QString getUser() const;
    QString getFile() const;
    QMap<QString, QString> toMap();
    QVector<qint32> getIndici() const;
    void setIndici(const QVector<qint32> &value);
};

#endif // MESSAGE_H
