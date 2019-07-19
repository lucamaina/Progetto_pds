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

QString utente::getUsername(){   return "'"+this->username+"'";}

QString utente::getNick(){  return "'"+this->nickname+"'";}

QString utente::getPass(){  return "'"+this->password+"'";}

void utente::setNick(QString nick)
{
    this->nickname = nick;
}
