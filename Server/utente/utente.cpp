/*
 * Classe per gestire i dettagli degli utenti connessi.
 *
 *
 *
 */


#include "utente.h"



/**
 * @brief utente::utente
 * @param map   mappa del comando
 * costruttore dell'utente a partire dalla mappa costruita dall'XML ricevuto
 */
bool utente::prepareUtente(QMap<QString, QString> map, bool isConnect){
    this->isConnect = isConnect;
    return this->prepareUtente(map);
}

QTcpSocket *utente::getSocket() const
{
    return mioSocket;
}


QString utente::getUsername(){   return this->username;}

QString utente::getNick(){  return this->nickname;}

QString utente::getPass(){  return this->password;}

void utente::setNick(QString nick)
{
    this->nickname = nick;
}

void utente::setSocket(QTcpSocket &sock)
{
    this->mioSocket = &sock;
}

bool utente::prepareUtente(QMap<QString, QString> map)
{
    QCryptographicHash hash(QCryptographicHash::Sha3_256);
    if (map.empty()){   return false;   }
    if (!map.contains(UNAME) || !map.contains(PASS)){   return false;   }
    QString nametmp = map.values(UNAME).first();
    if (nametmp.isEmpty()) return false;
    QString passtmp = map.values(PASS).first();
    QString passHash = hash.hash(passtmp.toUtf8(), QCryptographicHash::Sha3_256).toHex();

    if (passtmp.isEmpty()) return false;
    this->username = nametmp;
    this->password = passHash;

    if (map.contains(NICK) && !map.values(UNAME).first().isEmpty()){
        QString nicktmp = map.values(UNAME).first();
        this->nickname = nicktmp;
    }
    return true;
}

void utente::setConn(bool setConn){
    this->isConnect = setConn;
}

