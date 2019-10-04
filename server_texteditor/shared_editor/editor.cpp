/*
 * Classe implementa l'editor condiviso
 * 
 * prova di git
 */

#include "editor.h"

Editor::Editor(QObject *parent) : QObject(parent)
{

}

Editor::Editor(QString Id, QString fName)
{
    this->nomeFile = fName;
    this->DocId = Id;
    this->refCount = 1;
    this->file = new QFile(this->nomeFile);
    file->open(QIODevice::ReadWrite | QIODevice::Text);
    // file->write("qwertt");
    qDebug() << file->size();

    if (this->file->isOpen()){
        this->loadMap();

    }
    file->close();
}

bool Editor::loadMap()
{
    QByteArray ba = this->file->readLine();
    double idx = 1;
    while (!ba.isNull()) {
        for (int i = 0; i< ba.size(); i++){
            QChar c = ba.at(i);
            Symbol sym = Symbol("server", c, idx);      // username = server
            this->symMap.insert(idx, sym);
            idx++;
        }
        ba = this->file->readLine();
    }
    return true;
}

bool Editor::sendMap(QString nomeUtente)
{
    QTcpSocket *sock = this->sendList.value(nomeUtente);
    // prepara messaggio

    QByteArray ba;
    QXmlStreamWriter wr(&ba);
    QString body = this->mapToSend();

    wr.writeStartDocument();
    wr.writeStartElement(FBODY);

    wr.writeTextElement(DIMTOT, QString::number(body.size()));
    wr.writeTextElement(BODY, body);

    wr.writeEndElement();
    wr.writeEndDocument();

    int dim = ba.size();
    QByteArray len;
    len = QByteArray::number(dim, 16);
    len.prepend(8 - len.size(), '0');

    ba.prepend(len);
    ba.prepend(INIT);
    qDebug() << QString(ba);

    this->send(sock, ba);
    return true;
}

QString Editor::mapToSend()
{
    QString str = "";
    foreach (Symbol s, this->symMap.values()){
        str.append(s.getChar());
    }
    return str;
}

Editor::~Editor()
{
    // salva symVec su file
    try {
        save();
    } catch (std::exception& e) {
        qDebug() << e.what();
        // TODO altro
    }
}

bool Editor::process(Message &msg)
{
    Message::msgType tipo = msg.getTipo();
    if (tipo == Message::msgType::Rem_In){
        this->localInsert(msg);
        this->remoteInsert(msg);
    } else if (tipo == Message::msgType::Rem_Del) {
        // call localDel
    } else if (tipo == Message::msgType::Remove_File) {
        // call removeFile
    } else {
        return false;
    }
    return false;
}

bool Editor::send(QTcpSocket* t, QByteArray ba)
{
    if (t->write(ba, ba.length()) == -1){
        return false;
    }
    return true;
}



/*********************************************************************************************************
 ************************ metodi di gestione di utente ***************************************************
 *********************************************************************************************************/

/**
 * @brief Editor::addUser
 * @param user
 * @return
 * aggiungo socket relativo all'utente alla lista
 */
bool Editor::addUser(utente &user)
{
    if (this->sendList.contains(user.getUsername())){
        return true;
    } else {
        this->sendList.insert(user.getUsername(), user.getSocket());
    }
    return true;
}

bool Editor::removeUser(const QString &nomeUser)
{
    if (this->sendList.contains(nomeUser)){
        this->sendList.remove(nomeUser);
        return true;
    }
    return false;
}

bool Editor::findUser(const QString &nomeUser)
{
    return this->sendList.contains(nomeUser);
}


/*********************************************************************************************************
 ************************ metodi di gestione della mappa **************************************************
 *********************************************************************************************************/

/**
 * @brief Editor::localInsert
 * @param msg
 * @return
 * eseguo inserimento in locale
 */
bool Editor::localInsert(Message msg)
{
    this->symMap.insert(msg.getSym()->getIndex(), *msg.getSym());
    return true;
}

/**
 * @brief Editor::remoteInsert
 * @param sym
 * @return
 */
bool Editor::remoteInsert(Message msg)
{
    QByteArray ba = msg.toQByteArray();
    foreach (QString ut, this->sendList.keys()){
        if (ut != msg.getUser()){
            this->send(sendList.value(ut), ba);
        }
    }
    return true;
}

/**
 * @brief Editor::save
 * @return
 * scandisce la mappa e scrive sul file i caratteri
 */
bool Editor::save()
{
    QString s = "";
    Symbol sym;
    file->open(QIODevice::WriteOnly | QIODevice::Text);
    foreach (sym, this->symMap.values()){
        s.append(sym.getChar());
        if (s.size() == 4096){
            this->file->write(s.toUtf8());
            s.clear();
        }
    }
    file->write(s.toUtf8());
    file->close();
    return true;
}
