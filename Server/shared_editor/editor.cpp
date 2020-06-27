/*
 * Classe implementa l'editor condiviso
 * 
 * prova di git
 */

#include "editor.h"
#include "../server/s_thread.h"

Editor::Editor(QString Id, QString fName)
{

    qDebug() << "sono in " << Q_FUNC_INFO << "; nome file: " << fName;
    this->nomeFile = "files/"+fName;
    this->DocId = Id;
    dim = 0;

    file = new QFile(this->nomeFile);
    if ( file->open(QIODevice::ReadWrite | QIODevice::Text) ){
        this->loadMap();
    } else {
        qDebug() << endl
                 << " !!!!!!!!!!!!!!!!!!!!!!!!! Errore apertura file"
                 << file->errorString()
                 << endl
                 << this->nomeFile;
        Logger::getLog().write("Errore apertura file: " + this->nomeFile);

       throw std::logic_error("Errore apertura file");
    }
    file->close();


    qDebug() << file->size();


}

bool Editor::loadMap()
{
    // TODO verifica eccezioni
    QByteArray ba;
    ba = file->readAll();
    qDebug() << "sono in " << Q_FUNC_INFO;
    qDebug() << ba;
    if (!ba.isEmpty()){
        QDataStream out(&ba, QIODevice::ReadWrite);
        out >> this->symList;
    }

    return true;
}


//bool Editor::sendMap(QString nomeUtente)
//{
//    QSharedPointer<MySocket> socket = this->sendList_.value(nomeUtente);
//    // prepara messaggio

//    QByteArray ba;
//    QXmlStreamWriter wr(&ba);
//    int bodySize = this->dim;           // dimensione del file da inviare

//    wr.writeStartDocument();
//    wr.writeStartElement(FBODY);

//    wr.writeTextElement(DIMTOT, QString::number(bodySize));

//    wr.writeEndElement();
//    wr.writeEndDocument();

//    int dim = ba.size();
//    QByteArray len;
//    len = QByteArray::number(dim, 16);
//    len.prepend(8 - len.size(), '0');

//    ba.prepend(len);
//    ba.prepend(INIT);

//    if (!this->send(socket, ba)){
//        qDebug() << "err invio";
//    }
//    return sendBody(socket);
//}

//bool Editor::sendBody(QSharedPointer<MySocket> &sock)
//{
//    QByteArray ba;
//    QDataStream stream(&ba, QIODevice::ReadWrite);
//    stream << this->symList;
//    sock.get()->write(ba);
//    return true;
//}

/**
 * @brief Editor::getSymMap
 * @return
 * restituisce QByteArray con mappa di simboli serializzata e pronta all'invio
 */
QByteArray Editor::getSymMap()
{
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::ReadWrite);
    stream << this->symList;
    qDebug() << "size qByteArray to send = " << ba.size();
    return ba;
}


Editor::~Editor()
{
    // salva symVec su file
    try {
        save();
    } catch (std::exception& e) {
        qDebug() << e.what();
        Logger::getLog().write("Eccezione nel salvataggio del file " + this->nomeFile );
    }
}

bool Editor::process(Message &msg)
{
    Message::msgType tipo = msg.getTipo();
    if (tipo == Message::msgType::Rem_In){
        if (this->localInsert(msg) == true) {
            this->remoteSend(msg);
        } else {
            Comando err = Comando( Comando::Insert_Err);
            QSharedPointer<MySocket> ptr = this->sendList_.value(msg.getUser());
            this->send(ptr, err.toByteArray());
        }
    } else if (tipo == Message::msgType::Rem_Del) {
        // call localDel
        this->localDelete(msg);
        this->remoteSend(msg);
    } else if(tipo == Message::msgType::Cur){
        this->remoteSend(msg);
    } else {
        return false;
    }
    return false;
}

bool Editor::send(QSharedPointer<MySocket> &sock, QByteArray ba)
{
    sock.get()->write(ba);
    return true;
}

bool Editor::sendToAll(Comando &cmd)
{
    if (this->sendList_.isEmpty())
        return true;
    qDebug() << "--------------------------------------{";
    for(QSharedPointer<MySocket> sock : this->sendList_.values()){
        this->send(sock, cmd.toByteArray());
    }
    qDebug() << "--------------------------------------}";
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
bool Editor::addUser(utente &user, QSharedPointer<MySocket> & sock)
{
    qDebug() << "snon in " << Q_FUNC_INFO;
    this->sendList_.insert(user.getUsername(), sock);
    // preparo messaggio lista di cursori
    QStringList list = this->sendList_.keys();
    Comando cmd(Comando::Up_Cursor);
    cmd.insertMulti(UNAME, list).insert(DOCID, this->DocId);

    // invio a tutti
    this->sendToAll(cmd);

    return true;
}



bool Editor::removeUser(const QString &nomeUser)
{
    if (this->sendList_.contains(nomeUser)){
        this->sendList_.remove(nomeUser);
        Comando cmd(Comando::Up_Cursor);
        QStringList list = this->sendList_.keys();
        cmd.insertMulti(UNAME, list).insert(DOCID, this->DocId);
        this->sendToAll(cmd);
        return true;
    }
    return false;
}

bool Editor::findUser(const QString &nomeUser)
{
    return this->sendList_.contains(nomeUser);
}

bool Editor::isEmpty()
{
    return this->sendList_.isEmpty();
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
    Symbol newSym = msg.getSym();
    QVector<qint32> index = msg.getIndici();

    int posCursor = this->localPosCursor(index);
    if (posCursor > -1){
        if (!msg.getSym().getChar().isNull()){
            this->symList.insert(posCursor, newSym);
        }else if (posCursor < symList.size()){
            newSym.setCar( this->symList.at(posCursor).getCar() );
                    this->symList.replace(posCursor, newSym);
        }
        return true;
    }
    return false;
}

int Editor::localPosCursor(QVector<qint32> &index)
{
    int newPos = -1;
    if (symList.isEmpty()){
        return 0;
    }
    for (int pos = 0; pos < symList.size() && newPos == -1; pos++){
        Symbol s = this->symList.at(pos);
        int i = 0;
        for (qint32 val : s.getIndex()){
            if ( val < index.value(i, 0) ){
                break;
            }
            if ( val > index.value(i, 0) ){
                newPos = pos;
                break;
            }
            i++;
        }
    }
    if (newPos == -1){
        newPos = symList.size();
    }
    return newPos;
}

/**
 * @brief Editor::findLocalPosCursor
 * @param index
 * @return
 * cerca posizione con idici uguali
 */
int Editor::findLocalPosCursor(QVector<qint32> &index)
{
    for (Symbol s : this->symList){
        auto idx = s.getIndex();
        if ( idx == index){
            int pos = symList.indexOf(s);
            return pos;
        }
    }
    return -1; // non trovato
}

bool Editor::localDelete(Message msg)
{
    QString user = msg.getUser();
    QVector<qint32> index = msg.getIndici();

    int posCursor = this->findLocalPosCursor(index);
    if (posCursor > -1){
        symList.remove(posCursor);
        return true;
    }
    return false;
}

/**
 * @brief Editor::remoteInsert
 * @param sym
 * @return
 */
bool Editor::remoteSend(Message msg)
{
    auto map = msg.toMap();
    Comando cmd(map);
    return this->sendToAll(cmd);
}

/**
 * @brief Editor::save
 * @return
 * scandisce la mappa e scrive sul file i caratteri
 */
bool Editor::save()
{
    qDebug() << "sono in " << Q_FUNC_INFO;
    QByteArray ba;
    QDataStream out (&ba, QIODevice::ReadWrite);
    out << this->symList;

    qDebug() << ba;

    file->open(QIODevice::WriteOnly);

    file->write(ba);
    file->close();

    qDebug() << "Salvataggio di: " << this->nomeFile << endl;
    return true;
}
