#include "network.h"

Network::Network(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(sigSend()), this, SLOT(dispatch()));
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
    std::lock_guard<std::mutex> lg(mQueue);
    this->msgQueue.push_back(msg);
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

void Network::createEditor(QString nomeFile, utente &user)
{
    // verifica nome file non sia già presente
    if (this->editorMap.contains(nomeFile)){
        // file già presente, non posso crearlo
        throw std::exception();
    }
    Editor *ed = new Editor(nomeFile);
    ed->addUser(user);
    this->editorMap.insert(nomeFile, ed);
}

void Network::createEditor(QMap<QString, QString> cmd)
{

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
        }
    }
}
