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
void s_thread::leggiXML(uint dim)
{
    qDebug() << "Leggo XML";
    QMap<QString, QString> command;

    disconnect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));   // tolgo collegamento allo slot per leggere tutti i caratteri
    socket->waitForReadyRead();

    uint nbite = static_cast<uint>( this->socket->bytesAvailable() );
    while (nbite < dim){
        socket->waitForReadyRead();
        nbite = static_cast<uint>( this->socket->bytesAvailable() );
    }

    QByteArray qb = this->socket->read(dim);
    if (static_cast<uint>(qb.size()) < dim){
        // @TODO rileggo o exception
    }
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
    qDebug() << "finito lettura xml " << stream.errorString();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));  // ripristino connessione allo slot
    this->dispatchCmd(command);
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
}



/*********************************************************************************************************
 ************************ public slots *******************************************************************
 *********************************************************************************************************/

/**
 * @brief s_thread::readyRead
 * metodo chiamato se il socket ha un buffer da leggere, quando riconosce un intero lo usa come dimensione del
 * file xml da leggere e chiama il metodo per la lettura.
 */
void s_thread::readyRead()
{
    // leggo 4 byte di intero iniziale in hex
    QByteArray out;
    if (this->socket->bytesAvailable() >= 8){
        out = this->socket->read(8);
        uint dim = out.toUInt(nullptr, 16);   // dim in decimale
        qDebug() << "char: " << out << "int: " << dim;
        leggiXML(dim);
    }
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
