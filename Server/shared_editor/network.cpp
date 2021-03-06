#include "network.h"

Network::Network(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(sigSend()), this, SLOT(dispatch()),  Qt::ConnectionType::DirectConnection);
}

Network& Network::getNetwork(){
    static std::unique_ptr<Network> p (new Network());
    return *p;
}

/**
 * @brief Network::push
 * @param msg
 * metodo per mettere in coda il nuovo messaggio ricevuto
 */
void Network::push(Message &msg)
{   // lock se arrivano più messaggi
    std::unique_lock<std::mutex> lg(mQueue);
    this->msgQueue.push_back(msg);
    lg.unlock();
    emit sigSend();
    return;
}

bool Network::createEditor(QString fileId, QString nomeFile, utente &user, QSharedPointer<MySocket> &sock)
{
    // verifica nome file non sia già presente
    if (this->editorMap.contains(fileId)){
        // file già presente, non posso crearlo
        return false;
    }

    try {
        Editor *ed = new Editor(fileId, nomeFile);
        ed->addUser(user, sock);
        this->editorMap.insert(fileId, ed);
    } catch (std::logic_error le) {
        throw le;
    }

    return true;
}

void Network::sendToEdit(Message &msg)
{
    // cerca editor e lo manda
    if (!this->editorMap.contains(msg.getFile())){
        // messaggio ignorato per mancanza di file
        return;
    }
    Editor *ed = this->editorMap.value(msg.getFile());
    if (ed->findUser(msg.getUser())){
        ed->process(msg);
    } else {
        // utente non ha permesso di scrivere
        return;
    }
}

/**
 * @brief Network::filePresent
 * @param fileId
 * @return true / false
 * verifica presenza del file nella mappa degli editor
 */
bool Network::filePresent(QString fileId)
{
    if (this->editorMap.contains(fileId))
        return true;
    return false;
}

bool Network::addRefToEditor(QString fileId, utente &user, QSharedPointer<MySocket>&sock)
{
    if (this->editorMap.contains(fileId)){
        return editorMap.value(fileId)->addUser(user, sock);
    } else {
        return false;
    }
}

bool Network::remRefToEditor(QString fileId, QString user)
{
    if (this->editorMap.contains(fileId)){
        Editor *ed = editorMap.value(fileId);
        ed->removeUser(user);
        qDebug() << "sono in " << Q_FUNC_INFO << ed->isEmpty();
        if(ed->isEmpty()){ // ref count
            qDebug() << "salvo editor";
            ed->save();
            this->editorMap.remove(fileId);
        }
        return true;
    } else {
        return false;
    }
}

Editor &Network::getEditor(QString docId)
{
    return *this->editorMap.value(docId);
}

void Network::salvaTutto()
{
    for (Editor *e : this->editorMap.values()) {
        e->save();
    }
}

/**
 * @brief Network::dispatch
 * invia il messaggio all'editor corretto
 */
void Network::dispatch()
{
    // qDebug() << "sono in " << Q_FUNC_INFO;
    if (!msgQueue.isEmpty()){
        Message msg = msgQueue.dequeue();
        sendToEdit(msg);
    }
}
