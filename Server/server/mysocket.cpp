#include "mysocket.h"

MySocket::MySocket(int sockId)
{
    this->sockId = sockId;
    this->buffer.reserve(4096*10);
    this->command.reserve(4096);
    // this->sock.open(QIODevice::ReadWrite);
    this->sock.setSocketDescriptor(sockId);

    this->connect(&this->sock, &QTcpSocket::readyRead, this, &MySocket::readyRead, Qt::DirectConnection);
    this->connect(&this->sock, &QTcpSocket::disconnected, this, &MySocket::disconnected, Qt::DirectConnection);
}

/*
MySocket::~MySocket()
{
    qDebug().noquote() << "sono in " << Q_FUNC_INFO << " per socket: " << this->sockId;
}
*/

void MySocket::leggiXML(QByteArray data)
{
    qDebug() << endl << " -> Leggo XML from client: " << this->sockId << endl << data;
    QMap<QString, QString> command;
    QXmlStreamReader stream(data);

    while (!stream.atEnd() && !stream.hasError() ){
        QXmlStreamReader::TokenType token = stream.readNext();
        token = stream.readNext();
        // leggo elemento con nome del comando
        if (token == QXmlStreamReader::StartElement){
            //qDebug() << "comando: " << stream.name();
            QString cmd = stream.name().toString();
            command.insert(CMD, cmd);
        }
        token = stream.readNext();
        // leggo elemnto variabile
        while ( token == QXmlStreamReader::StartElement ){
            QString name = stream.name().toString(), text = stream.readElementText();
            command.insert(name, text);
            //qDebug() << "   start elem: " << name << " val: " << text;
            token = stream.readNext();
        }
    }

    if (stream.hasError()){
        qDebug() << "err in lettura XML" << stream.errorString();
    } else {
        // qDebug() << "finito lettura xml no errori " << stream.errorString();
        emit s_dispatchCmd(command);
    }
}

bool MySocket::write(QByteArray data)
{

        if (sock.isOpen()){
            if ( sock.isWritable() ){
                qint64 ret = this->sock.write(data, data.size());
                if ( ret != -1){
                    qDebug() << " <- To Client: "<< sockId << endl
                             << data;
                    return true;
                } else {
                    qDebug() << endl << "!!! Errore scrittura socket: "<< this->sockId << " !!!" << endl << sock.errorString();
                }
            }   else {
                    qDebug() << endl << "!!! Errore non Writable socket: "<< this->sockId << " !!!" << endl << data;
            }
        } else {
            qDebug() << endl << "!!! Errore non Aperto socket: "<< this->sockId << " !!!" << endl << data;
        }


    return false;
}

bool MySocket::write(QMap<QString, QString> comando)
{
    qDebug() << "write: " << comando.size();
    QByteArray data = Comando(comando).toByteArray();
    return this->write( data );
}

void MySocket::readyRead()
{
    QByteArray out;
    QByteArray tmp;
    uint dim;
    char v[4096] = {};

    out.reserve(4096);
    tmp.reserve(16);

    if (this->sock.bytesAvailable() > 0){
        if (this->sock.read(v, 4096) < 0){
            qDebug() << "errore in socket::read()";
        }
        this->buffer.append( v );
    } else {
        return;
    }

    while (buffer.contains(INIT)){
        // presente token iniziale del messaggio
        int idx = buffer.indexOf(INIT, 0);
        if ( idx >= 0){
            buffer.remove(0, idx);
            command.clear();
            command.append(buffer);       // |command| contiene token <INIT> e quello che viene ricevuto subito dopo
            command.remove(0, INIT_DIM);       // rimuovo <INIT> da |out|

            if (command.size() < LEN_NUM){
                return;
            }

            tmp = command.left(LEN_NUM);                  // in |tmp| copio 8 byte che descrivono la dimensione del messaggio
            command.remove(0,LEN_NUM);
            dim = tmp.toUInt(nullptr, 16); // |dim| ha dimensione di tmp in decimale

            uint len = static_cast<uint>(command.size());
            uint rimane = 0;
            if (len < dim){
                rimane = dim - len;
            }

            while (rimane > 0){
                uint i = dim - len;
                sock.waitForReadyRead(100);      // finisco di leggere il resto del messaggio
                if (this->sock.read(v, i) < 0){
                    qDebug() << "errore in socket::read()";
                }
                this->command.append( v );
                len = static_cast<uint>(command.size());
                rimane = dim - len;
            }
            command.remove(static_cast<int>(dim), 4096);
            buffer.remove(0, LEN_NUM + INIT_DIM + static_cast<int>(dim));

            leggiXML(command);
            // finito di leggere i byte del messaggio ritorno al ciclo iniziale
        } else {
            // token nonn trovato
        }
    }
}

void MySocket::disconnected()
{
    emit s_disconnected();
}
