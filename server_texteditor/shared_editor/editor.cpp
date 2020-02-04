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
    // TODO verifica eccezioni
    QByteArray ba;
    this->file->open(QIODevice::ReadWrite);

    ba = file->readAll();
    ba = ba.fromBase64(ba, QByteArray::Base64Encoding | QByteArray::KeepTrailingEquals);

    if (!ba.isEmpty()){
        QDataStream out(&ba, QIODevice::ReadWrite);
        out >> this->symMap;
    }

    return true;
}


bool Editor::sendMap(QString nomeUtente)
{
    QTcpSocket *sock = this->sendList.value(nomeUtente);
    // prepara messaggio

    QByteArray ba;
    QXmlStreamWriter wr(&ba);
    int bodySize = this->symMap.size();           // dimensione del file da inviare

    wr.writeStartDocument();
    wr.writeStartElement(FBODY);

    wr.writeTextElement(DIMTOT, QString::number(bodySize));
  //  wr.writeTextElement(BODY, body);

    wr.writeEndElement();
    wr.writeEndDocument();

    int dim = ba.size();
    QByteArray len;
    len = QByteArray::number(dim, 16);
    len.prepend(8 - len.size(), '0');

    ba.prepend(len);
    ba.prepend(INIT);
    qDebug() << QString(ba);

    if (!this->send(sock, ba)){
        qDebug() << "err invio";
    }
    return sendBody(sock);
}

bool Editor::sendBody(QTcpSocket *sock )
{
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::ReadWrite);
    stream << this->symMap;
    qDebug() << "size qByteArray = " << ba.size();
    sock->write(ba);
    return true;
}

/**
 * @brief Editor::getSymMap
 * @return
 * restituisce QByteArray con mappa di simboli serializzata e pronta all'invio
 */
QByteArray Editor::getSymMap()
{
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::ReadWrite);
    stream << this->symMap;
    ba = ba.toBase64(QByteArray::Base64Encoding | QByteArray::KeepTrailingEquals);
    qDebug() << "size qByteArray to send = " << ba.size();
    return ba;
}

QString Editor::mapToSend()
{
    QString str = "";
    foreach (Symbol s, this->symMap.values()){
        str.append(s.getChar());
    }
    return str;
}

bool Editor::loadFile(const QString &nomeUser, const int dimFile)
{
    if (!this->sendList.contains(nomeUser)){    return false;   }


    return true;
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
        if (this->localInsert(msg) == true) {
            this->remoteSend(msg);
        } else {
            // rispondi con err
        }
    } else if (tipo == Message::msgType::Rem_Del) {
        // call localDel
        this->localDelete(msg);
        this->remoteSend(msg);
    } else if(tipo == Message::msgType::Cur_Cng){
        this->cursorChange(msg);
        this->remoteSend(msg);
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
    qDebug() << "To client: " << ba;
    return true;
}

bool Editor::rispErr(Message &msg)
{
    return this->send(this->sendList.value(msg.getUser()),
                      Comando(Comando::Insert_Err).toByteArray());
}

bool Editor::deserialise(QByteArray &ba)
{
    //TODO
    // problemi in serilizzare mappa
    // this->symMap.clear();
    QMap<double, Symbol> map;
    map.clear();
    QByteArray p;
    QDataStream o(&p, QIODevice::ReadWrite);
    o << this->symMap;

    QDataStream i(&p, QIODevice::ReadWrite);
    i >> map;
    p = p.toBase64(QByteArray::Base64Encoding | QByteArray::KeepTrailingEquals);
    QByteArray _new = QByteArray::fromBase64(ba);
    QDataStream stream(&_new, QIODevice::ReadWrite);
    map.clear();
    stream >> map;
    qDebug() << "size qByteArray = " << ba.size();
    return true;
}

void Editor::editProva()
{
    /*
    this->symMap.clear();
    this->symMap.insert(1, Symbol("asd", 'a', 1, "aaaaaaaaaaaa"));
    symMap.insert(2, Symbol("asd", 'b', 2, "bbbbbbbbbbbbb"));
    symMap.insert(3, Symbol("qwe", 'c', 3, "ccccccccccccccc"));
    symMap.insert(4, Symbol("qwe", 'd', 4, "ddddddddddd"));
    symMap.insert(5, Symbol("asd", 'e', 5, "eeeeeeeeeeeeeee"));

    QJsonObject json;
    QByteArray s;

    QMapIterator<double, Symbol> i(symMap);
    while (i.hasNext()) {
        i.next();
        s.clear();
        QDataStream out(&s, QIODevice::WriteOnly);
        QJsonValue jval( QString(s.toBase64()) );
        qDebug() << jval.toString();
        json.insert(QString::number(i.key()), jval.toString());
    }

    QJsonDocument Doc(json);


    QFile *f= new QFile("mappa_sv");
    f->open(QIODevice::ReadWrite | QIODevice::Text);
    //QByteArray s;
    s.clear();
    QDataStream out(&s, QIODevice::WriteOnly);
    out << symMap;

    QMap<double,Symbol> deserial;
    QDataStream in(&s, QIODevice::ReadWrite);
    in >> deserial;

    f->write(s);
    f->write("\n\n");
    f->write(s.toHex());
    f->write("\n\n");
    f->write(s.toBase64(QByteArray::Base64Encoding | QByteArray::KeepTrailingEquals));
    f->write("\n\n");
    f->write(Doc.toJson());
    f->close();
    */
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

bool Editor::isEmpty()
{
    return this->userList.isEmpty();
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

bool Editor::localDelete(Message msg)
{
    this->symMap.remove(msg.getSym()->getIndex());
    return true;
}

/**
 * @brief Editor::remoteInsert
 * @param sym
 * @return
 */
bool Editor::remoteSend(Message msg)
{
    QByteArray ba = msg.toQByteArray();

    foreach (QString ut, this->sendList.keys()){
        if (ut != msg.getUser()){
            this->send(sendList.value(ut), ba);
        } else {
            // invia messaggio di ok
            this->send(sendList.value(ut), Comando(Comando::Insert_Ok).toByteArray());
        }
    }
    return true;
}


bool Editor::cursorChange(Message msg)
{
    return remoteSend(msg);
}
/**
 * @brief Editor::save
 * @return
 * scandisce la mappa e scrive sul file i caratteri
 */
bool Editor::save()
{
    QByteArray ba;
    QDataStream out (&ba, QIODevice::ReadWrite);
    out << this->symMap;
    file->open(QIODevice::WriteOnly | QIODevice::Text);

    file->write(ba.toBase64(QByteArray::Base64Encoding | QByteArray::KeepTrailingEquals));
    file->close();

    qDebug() << "Salvataggio di: " << this->nomeFile << endl;
    return true;
}
