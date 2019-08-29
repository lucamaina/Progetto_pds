#include "s_thread.h"

s_thread::s_thread(int ID, QObject *parent) : QThread(parent)
{
    this->sockID = ID;
}


void s_thread::run()
{
    qDebug() << "Thread running, ID: "
             << this->sockID;
    this->socket = new QTcpSocket();
    socket->setSocketDescriptor(sockID);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::ConnectionType::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::ConnectionType::DirectConnection);
    qDebug() << "Client connesso";

/*    this->conn = new db();

    this->user = new utente("user1@asd.it", "0");
    this->conn->userLogin(*user);
    qDebug() << "nick: " << user->getNick();

    user = new utente("asd", "1");
    user->setNick("nico");
    this->conn->userReg(*user);
    */
}

/**
 * @brief s_thread::leggiXML
 * @param dim
 * metodo di lettura del file XML. Gli elementi del file vengono mappati su una map di key, value
 * e quindi mandati a una funzione che esegue distinzione tra i comandi
 */
void s_thread::leggiXML(QByteArray qb)
{
    qDebug() << "Leggo XML";
    QMap<QString, QString> command;

    qDebug() << qb;
    QXmlStreamReader stream(qb);

    while (!stream.atEnd() && !stream.hasError() ){
        QXmlStreamReader::TokenType token = stream.readNext();
        // leggo start document
        if (token == QXmlStreamReader::StartDocument){
            qDebug() << "start doc: " << token << " - " << stream.readElementText();
        }
        token = stream.readNext();
        // leggo elemento con nome del comando
        if (token == QXmlStreamReader::StartElement){
            qDebug() << "comando: " << stream.name();
            QString cmd = stream.name().toString();
            command.insert("cmd", cmd);
        }
        token = stream.readNext();
        // leggo elemnto variabile
        while ( token == QXmlStreamReader::StartElement ){
            qDebug() << "start elem: " << stream.name();
            qDebug() << "val elem: " << stream.readElementText();
            QString name = stream.name().toString(), text = stream.readElementText();
            command.insert(name, text);
            token = stream.readNext();
        }
    }

    // verifica correttezza xml
    // TODO in caso di messaggio non corretto riparti da stato corretto
    if (stream.hasError()){
        qDebug() << "finito lettura xml con errore" << stream.errorString();
    } else {
        qDebug() << "finito lettura xml no errori " << stream.errorString();
        this->dispatchCmd(command);
    }
}

/**
 * @brief s_thread::dispatchCmd
 * @param cmd
 * legge nella mappa il nome del comando e richiama le funzioni corrette
 */
void s_thread::dispatchCmd(QMap<QString, QString> cmd){
    auto comando = cmd.find("cmd");
    if (comando.value() == CONN) {
        this->connectDB();
    } else if (comando.value() == LOGIN) {

    } else if (comando.value() == REG) {

    } else if (comando.value() == REM_IN) {

    } else if (comando.value() == REM_DEL) {

    } else if (comando.value() == DISC) {

    } else if (comando.value() == FILE) {

    }
}

void s_thread::connectDB(){
    this->conn = new db();
    if (conn->conn() == false){
        // ritorna messaggio al client di fallimento
        sendMSG("impossibile connettersi al db");
    } else {
        // messaggio di successo al client
        sendMSG("connessione al db riuscita");
    }
}


bool s_thread::sendMSG(QByteArray data){
    if (socket->isOpen() && socket->isWritable()){
        if (this->socket->write(data, data.size()) == -1){
            // errore
            qDebug() << "errore scrittura verso client";
        }
    }
    return false;
}


/*********************************************************************************************************
 ************************ public slots *******************************************************************
 *********************************************************************************************************/

/**
 * @brief s_thread::readyRead
 * metodo chiamato se il socket ha un buffer da leggere, quando riconosce un intero lo usa come dimensione del
 * file xml da leggere e chiama il metodo per la lettura.
 *
 * |buffer| usato come contenitore sporco,
 * |out|
 */
void s_thread::readyRead()
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));  // disconnetto lo slot cosi da non avere più chiamate nello stesso

    QByteArray out;
    QByteArray tmp;
    uint dim;

    //buffer.resize(4096);
    out.resize(1024);
    tmp.resize(16);

    while (socket->bytesAvailable() > 0){
        buffer.append(socket->readAll());
        qDebug() << buffer;

        if (buffer.contains(INIT)){
            // presente token iniziale del messaggio
            int idx =buffer.indexOf(INIT, 0);
            if ( idx >= 0){
                buffer.remove(0, idx);
                out = buffer;       // |out| contiene token <INIT> e quello che viene ricevuto subito dopo
                buffer.clear();     // elimino rimananze
                out.remove(0, INIT_DIM);       // rimuovo <INIT> da |out|

                while (out.size() < LEN_NUM){             // ricevo la dimensione del messaggio
                    socket->waitForReadyRead(100);
                    out.append(socket->read(LEN_NUM - static_cast<uint>(out.size())));
                }
                qDebug() << "out: " << out;
                tmp = out.left(LEN_NUM);                  // in |tmp| copio 8 byte che descrivono la dimensione del messaggio
                out.remove(0,LEN_NUM);
                dim = tmp.toUInt(nullptr, 16); // |dim| ha dimensione di tmp in decimale
                qDebug() << dim;

                uint rimane = dim - static_cast<uint>(out.size());
                while (rimane > 0){
                    uint i = dim - static_cast<uint>(out.size());
                    socket->waitForReadyRead(100);      // finisco di leggere il resto del messaggio
                    out.append(socket->read(i));
                    rimane = dim - static_cast<uint>(out.size());
                }
                qDebug() << "bene qui";

                leggiXML(out);
                // finito di leggere i byte del messaggio ritorno al ciclo iniziale
            } else {
                // token nonn trovato
            }
        }
        // in |buffer| non c'è il token <INIT>, continuo a ricevere
    }
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));  // ripristino connessione allo slot
}

/**
 * @brief s_thread::disconnected
 * metodo chiamato quando viene disconesso il socket dal client.
 */
void s_thread::disconnected()
{
    qDebug() << "client disconnesso "
             << this->sockID;
    this->socket->deleteLater();
    exit(0);
}
