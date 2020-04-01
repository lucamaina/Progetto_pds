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
        QStringList lista = indici.split(";", QString::SkipEmptyParts);
        for (QString idx : lista){
            this->indici.push_back(idx.toInt());
        }
        qDebug() << "sono in " << Q_FUNC_INFO << endl
                 << lista;
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
    switch (this->tipo) {
    case Rem_In : map.insert(CMD, REM_IN); break;
    case Rem_Del : map.insert(CMD, REM_DEL); break;
    case Message::Cur : map.insert(CMD, CRS); break;
    }
    map.insert(DOCID, this->fileId);
    return map;
}

QVector<qint32> Message::getIndici() const
{
    return indici;
}

void Message::setIndici(const QVector<qint32> &value)
{
    indici = value;
}

Message::Message()
{
    
}
