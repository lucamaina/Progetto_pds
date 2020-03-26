#include "comando.h"

/**
 * @brief Comando::Comando
 * @param tipo
 * crea comandi standard
 */
Comando::Comando(Comando::cmdType tipo)
{
    switch (tipo) {
    case (Insert_Ok):
        cmdMap.insert(CMD, OK);
        cmdMap.insert(MEX, INS_OK);
        break;
    case (Remove_Ok): cmdMap.insert(CMD, OK);
        break;
    case (Insert_Err):
        cmdMap.insert(CMD, ERR);
        cmdMap.insert(MEX, INS_ERR);
        break;
    case (Remove_Err):
        cmdMap.insert(CMD, ERR);
        cmdMap.insert(MEX, REM_ERR);
        break;
    case (Up_Cursor):
        cmdMap.insert(CMD, UP_CRS);
        break;
    default:
        cmdMap.insert(CMD, "Error");
        break;
    }
}

QByteArray Comando::toByteArray()
{
    QList<QString> list = {CMD};
    if (!this->verifyCMD(list)){ return "";   }

    QByteArray ba;
    QXmlStreamWriter wr(&ba);
    wr.writeStartDocument();
    wr.writeStartElement(this->cmdMap.value(CMD));
    cmdMap.remove(CMD);

    for(QString key : cmdMap.keys()){
        wr.writeTextElement(key, cmdMap.value(key));
    }
    wr.writeEndElement();
    wr.writeEndDocument();

    int dim = ba.size();
    QByteArray len;
    len = QByteArray::number(dim, 16);
    len.prepend(8 - len.size(), '0');

    ba.prepend(len);
    ba.prepend(INIT);
    return ba;
}

/**
 * @brief s_thread::verifyCMD
 * @param cmd
 * @param list
 * @return true se tutti i campi di list sono chiavi di cmd
 * verifica presenza dei campi nella mappa dei comandi
 */
bool Comando::verifyCMD(QList<QString> &list)
{
    QString val;
    foreach (val, list){
        if (!this->cmdMap.contains(val)){    return false;   }
    }
    return true;
}

void Comando::insert(QString key, QString value)
{
    this->cmdMap.insert(key, value);
}

Comando& Comando::insertMulti(QString tipo, QStringList &elenco)
{
    int i = 1;
    for (QString s : elenco){
        this->cmdMap.insert(tipo+QString::number(i++), s);
    }
    return *this;
}

QList<QString> Comando::keys()
{
    return this->cmdMap.keys();
}

QList<QString> Comando::values()
{
    return this->cmdMap.values();
}

QString Comando::toString()
{
    QString str;
    for (QString key : this->keys()){
        str.append(key+" : "+cmdMap.value(key)+"\n");
    }
    return str;
}

