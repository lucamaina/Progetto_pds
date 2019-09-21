#include "message.h"

bool Message::prepareMsg(QMap<QString, QString> comando)
{
    if (comando.isEmpty()){ return false;   }
    QString nomecmd, nomeFile, utente;
    msgType  tipo;
    if (comando.contains(CMD) && comando.contains(FILES) && comando.contains(UNAME)){
        nomecmd = comando.value(CMD);
        nomeFile = comando.value(FILES);
        utente = comando.value(UNAME);
    } else {    return false;   }

    if (nomecmd == ADD_FILE){
        tipo = Add_File;
    } else if (nomecmd == REM_IN) {
        tipo = Rem_In;
    } else if (nomecmd == REM_DEL) {
        tipo = Rem_Del;
    } else {
        return false;               // comando non riconosciuto, messaggio ignorato
    }

    this->file = nomeFile;
    this->user = utente;
    this->tipo = tipo;

    if (this->tipo == Rem_In || this->tipo == Rem_Del){
        this->sym = new Symbol();   // TODO
    } else {
        this->sym = nullptr;
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
    return file;
}

Message::Message()
{

}
