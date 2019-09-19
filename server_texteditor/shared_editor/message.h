#ifndef MESSAGE_H
#define MESSAGE_H

#include <QMap>

#include "symbol.h"
#include "../utente/utente.h"
#include "../server/cmdstring.h"

class Message
{
public:
    enum msgType {Rem_In, Rem_Del, Remove_File};

private:
    msgType tipo;
    Symbol* sym;
    QString user;
    QString file;

public:
    explicit Message();
    bool prepareMsg(QMap<QString, QString> comando);
    msgType getTipo() const;
    Symbol *getSym() const;
    QString getUser() const;
    QString getFile() const;
};

#endif // MESSAGE_H
