#include "message.h"

bool Message::prepareMsg(QMap<QString, QString> comando, QString username)
{
    if (comando.isEmpty()){ return false;   }
    QString nomecmd, formato, remoteUSer;
    QChar car;
    QString indici;

    if (!comando.contains(CMD) ||
            !comando.contains(DOCID) ||
            !comando.contains(UNAME) ){
        qDebug() << "Errore nel comando: " << endl
                 << comando;
        return false; // problema nel comando
    }

    nomecmd = comando.value(CMD);
    this->fileId = comando.value(DOCID);
    remoteUSer = comando.value(UNAME);

    if (remoteUSer != username){
        qDebug() << "Errore username ricevuti";
        return false;
    }
    this->user = username;

    if (nomecmd == CRS){
        this->tipo = Message::Cur;
    } else if (nomecmd == REM_IN) {
        this->tipo = Message::Rem_In;
    } else if (nomecmd == REM_DEL) {
        this->tipo = Message::Rem_Del;
    } else {
        return false;
    }

    if (comando.contains(IDX)){ // indice come stringa con elenco di interi divisi da ;
        indici = comando.value(IDX);
        QStringList lista = indici.split(";");
        for (QString idx : lista){
            this->indici.push_back(idx.toInt());
        }
    }

    if (comando.contains(CAR)){
        car = comando.value(CAR).at(0);
    } else {
        car = QChar::Null;
    }

    if (comando.contains(FORMAT)){
        formato = comando.value(FORMAT);
    } else {
        formato = "";
    }

    QByteArray format = QByteArray(formato.toLatin1());
    this->sym = Symbol(user, car, this->indici, format);

    return true;
}

Message::msgType Message::getTipo() const
{
    return tipo;
}

Symbol Message::getSym() const
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
    QMap<QString, QString> map = this->sym.toMap();
    QString tipe = REM_IN;
    if (this->tipo == Message::Rem_Del)
        tipe = REM_DEL;
    map.insert(CMD, tipe);
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
/*
    foreach (QString key, comando.keys()) {
        wr.writeTextElement(key, comando.value(key));
    }*/
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
