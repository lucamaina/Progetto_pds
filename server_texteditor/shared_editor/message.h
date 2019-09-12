#ifndef MESSAGE_H
#define MESSAGE_H

#include <QMap>

#include "symbol.h"
#include "../utente/utente.h"
#include "../server/cmdstring.h"

class Message
{

public:
    explicit Message();
    bool prepareMsg(QMap<QString, QString> comando);


private:
    enum msgType {Rem_In, Rem_Del, Add_File, Remove_File};
    msgType tipo;
    Symbol* sym;
    QString user;
    QString file;

};

#endif // MESSAGE_H
