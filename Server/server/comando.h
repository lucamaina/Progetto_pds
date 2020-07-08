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
    enum cmdType { Insert_Ok,
                    Remove_Ok,
                    Insert_Err,
                    Remove_Err,
                    Up_Cursor};

private:
    QMap<QString, QString> cmdMap;

public:
    explicit Comando(QMap<QString, QString>& cmd) : cmdMap(cmd) {}
    Comando(cmdType tipo);
    QByteArray toByteArray(void);
    bool verifyCMD(QList<QString> &list);
    void insert(QString key, QString value);
    Comando &insertMulti(QString tipo, QStringList & elenco);
    QList<QString> keys();
    QList<QString> values();
    QString value(QString key);
    QString toString();
};

#endif // COMANDO_H
