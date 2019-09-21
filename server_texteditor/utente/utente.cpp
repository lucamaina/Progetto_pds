/*
 * Classe per gestire i dettagli degli utenti connessi.
 *
 *
 *
 */


#include "utente.h"

utente::utente(QString username, QString password, QObject *parent)
    : QObject(parent), username(username), password(password)
{

}

/**
 * @brief utente::utente
 * @param map   mappa del comando
 * costruttore dell'utente a partire dalla mappa costruita dall'XML ricevuto
 */
bool utente::prepareUtente(QMap<QString, QString> map){
    if (map.empty()){   return false;   }
    if (!map.contains(UNAME) || !map.contains(PASS)){   return false;   }
    QString nametmp = map.values(UNAME).first();
    if (nametmp.isEmpty()) return false;
    QString passtmp = map.values(PASS).first();
    if (passtmp.isEmpty()) return false;
    this->username = nametmp;
    this->password = passtmp;

    if (map.contains(NICK) && !map.values(UNAME).first().isEmpty()){
        QString nicktmp = map.values(UNAME).first();
        this->nickname = nicktmp;
    }
    return true;
}

QString utente::getUsername(){   return this->username;}

QString utente::getNick(){  return this->nickname;}

QString utente::getPass(){  return this->password;}

void utente::setNick(QString nick)
{
    this->nickname = nick;
}

void utente::setConn(bool setConn){
    this->isConnect = setConn;
}

