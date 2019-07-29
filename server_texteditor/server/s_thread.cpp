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

void s_thread::disconnected()
{
    qDebug() << "client disconnesso "
             << this->sockID;
    this->socket->deleteLater();
    exit(0);
}

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

void s_thread::leggiXML(uint dim)
{
    qDebug() << "Leggo XML";

    disconnect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));   // tolgo collegamento allo slot
    socket->waitForReadyRead();
    uint nbite = static_cast<uint>( this->socket->bytesAvailable() );
    while (nbite < dim){
        socket->waitForReadyRead();
        nbite = static_cast<uint>( this->socket->bytesAvailable() );
    }

    QByteArray qb = this->socket->read(dim);
    qDebug() << qb;
    QXmlStreamReader stream(qb);

    while (!stream.atEnd() && !stream.hasError() ){
        QXmlStreamReader::TokenType token = stream.readNext();
        if (token == QXmlStreamReader::StartDocument){
            qDebug() << "start doc: " << token << " - " << stream.readElementText();
        }
        // qDebug() << token << " - " << stream.readElementText();
        if (token == QXmlStreamReader::StartElement){
            qDebug() << "start elem: " << stream.name();
            QString cmd = stream.name().toString();
            if (cmd == LOGIN){
                // chiama metodo login
            }else if (cmd == REG) {
                // chiama metodo
            }
        }

    }
    qDebug() << "finito lettura xml " << stream.errorString();
}

