#include "message.h"

bool Message::prepareMsg(QMap<QString, QString> comando, QString username)
{
    if (comando.isEmpty()){ return false;   }
    QString nomecmd, fileId, formato;
    QChar car;
    msgType  tipo;
    double idx;
    int cursor;
    // il comando non contiene username perchè viene preso l'utente collegato
    if (comando.contains(CMD) && comando.contains(DOCID)){
        nomecmd = comando.value(CMD);
        fileId = comando.value(DOCID);
        car = comando.value(CAR).at(0);
        idx = comando.value(IDX).toDouble();
        cursor = comando.value(CUR).toInt();
        formato = comando.value(FORMAT);
    } else {    return false;   }

    if (nomecmd == REM_IN) {
        tipo = Rem_In;
    } else if (nomecmd == REM_DEL) {
        tipo = Rem_Del;
    } else {
        return false;               // comando non riconosciuto, messaggio ignorato
    }

    this->fileId = fileId;
    this->user = username;
    this->tipo = tipo;

    if (this->tipo == Rem_In || this->tipo == Rem_Del){
        QByteArray format = QByteArray(formato.toLatin1());
        this->sym = new Symbol(user, car, idx, format);
    } else {
        this->sym = nullptr;
        return false;
    }

    return true;
}

Message::msgType Message::getTipo() const
{
    return tipo;
}

Symbol *Message::getSym() const
{
    return sym;
}

QString Message::getUser() const
{
    return user;
}

QString Message::getFile() const
{
    return fileId;
}

QMap<QString, QString> Message::toMap()
{
    QMap<QString, QString> map = this->sym->toMap();
    QString tipe = REM_IN;
    if (this->tipo == Message::Rem_Del)
        tipe = REM_DEL;
    map.insert(CMD, tipe);
    map.insert(UNAME, this->user);
    map.insert(DOCID, this->fileId);
    map.insert(DOCID, this->fileId);
    return map;
}

QByteArray Message::toQByteArray()
{
    QByteArray ba;
    QXmlStreamWriter wr(&ba);
    QMap<QString, QString> comando = this->toMap();
    wr.writeStartDocument();
    wr.writeStartElement(comando.value(CMD));
    comando.remove(CMD);

    foreach (QString key, comando.keys()) {
        wr.writeTextElement(key, comando.value(key));
    }
    {
        wr.writeTextElement(DOCID, this->fileId);
    }
    wr.writeEndElement();
    wr.writeEndDocument();

    int dim = ba.size();
    QByteArray len;
    len = QByteArray::number(dim, 16);
    len.prepend(8 - len.size(), '0');

    ba.prepend(len);
    ba.prepend(INIT);
    qDebug() << QString(ba);

    return ba;
}

Message::Message()
{
    
}
