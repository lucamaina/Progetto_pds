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
