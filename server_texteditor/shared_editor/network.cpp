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
{
    std::unique_lock<std::mutex> lg(mQueue);
    this->msgQueue.push_back(msg);
    lg.unlock();
    emit sigSend();
}

/**
 * @brief Network::msgRead
 * @param msg
 * legge i valori del messaggio e cerca il rapporto con file e utente
 */
void Network::msgRead(Message &msg)
{
    // se file esiste e user presente nell'editor lo passa
    if ( this->editorMap.contains(msg.getFile()) ){

    }
}

bool Network::createEditor(QString fileId, QString nomeFile, utente &user)
{
    // verifica nome file non sia già presente
    if (this->editorMap.contains(fileId)){
        // file già presente, non posso crearlo
        return false;
    }
    Editor *ed = new Editor(fileId, nomeFile);
    ed->addUser(user);
    this->editorMap.insert(fileId, ed);

    // TODO leggi file e carica symMap

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
 * @brief Network::sendToUsers
 * @param msg
 * @return
 * invia msg a tutti gli altri utenti
 */
bool Network::sendToUsers(Message &msg)
{
    QString sendUser = msg.getUser();

    return true;
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


bool Network::addRefToEditor(QString fileId, utente &user)
{
    if (this->editorMap.contains(fileId)){
        editorMap.value(fileId)->addUser(user);
        return true;
    } else {
        return false;
    }
}

Editor &Network::getEditor(QString docId)
{
    return *this->editorMap.value(docId);
}

/**
 * @brief Network::dispatch
 * invia il messaggio all'editor corretto
 */
void Network::dispatch()
{
    std::lock_guard<std::mutex> lg(mQueue);
    if (!msgQueue.isEmpty()){
        while (msgQueue.size() > 0){
            Message msg = msgQueue.dequeue();
            // leggo messaggio e lo consegno
            sendToEdit(msg);
            // send to altri utenti

        }
    }
}
