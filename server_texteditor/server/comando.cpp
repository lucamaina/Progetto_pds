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
    }
}

QByteArray Comando::toByteArray()
{
    QList<QString> list = {CMD};
    if (!this->verifyCMD(list)){ return "";   }

    QByteArray ba;
    QXmlStreamWriter wr(&ba);
    wr.writeStartDocument();
    wr.writeTextElement(this->cmdMap.value(CMD), "");

    foreach(QString key, cmdMap.keys()){
        if (key != CMD) {
            QString elem, val;
            elem = this->cmdMap.firstKey();
            val = this->cmdMap.value(elem);
            qDebug() << elem << " : " << val;
            wr.writeTextElement(elem, val);
        }
    }

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

