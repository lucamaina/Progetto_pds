#ifndef COMANDO_H
#define COMANDO_H

#include <QMap>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>

#include "cmdstring.h"

class Comando
{
public:
    enum cmdType {Insert_Ok, Remove_Ok, Insert_Err, Remove_Err};

private:
    QMap<QString, QString> cmdMap;

public:
    explicit Comando(QMap<QString, QString>& cmd) : cmdMap(cmd) {}
    Comando(cmdType tipo);
    QByteArray toByteArray(void);
    bool verifyCMD(QList<QString> &list);
};

#endif // COMANDO_H
