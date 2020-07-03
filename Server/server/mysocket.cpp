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
            QString cmd = stream.name().toString();
            command.insert(CMD, cmd);
        }
        token = stream.readNext();
        // leggo elemento variabile
        while ( token == QXmlStreamReader::StartElement ){
            QString name = stream.name().toString(), text = stream.readElementText();
            command.insert(name, text);
            token = stream.readNext();
        }
    }

    if (stream.hasError()){
        qDebug() << "!!! Errore in lettura XML" << stream.errorString();
    } else {
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
            qDebug() << endl << "!!! Errore socket non Aperto: "<< this->sockId << " !!!" << endl << data;
        }
    return false;
}

bool MySocket::write(QMap<QString, QString> comando)
{
    qDebug() << "write: " << comando.size();
    QByteArray data = Comando(comando).toByteArray();
    return this->write( data );
}

//void MySocket::readyRead()
//{
//    QByteArray out;
//    QByteArray tmp;
//    uint dim;
//    qint64 dimRead = 0;

//    char v[4096] = {};

//    out.reserve(4096);
//    tmp.reserve(16);

//    if (this->sock.bytesAvailable() > 0){
//        if (this->sock.read(v, 4096) < 0){
//            qDebug() << "errore in socket::read()";
//        }
//        this->buffer.append( v );
//    } else {
//        return;
//    }

//    while (buffer.contains(INIT)){
//        // presente token iniziale del messaggio
//        int idx = buffer.indexOf(INIT, 0);
//        if ( idx >= 0){
//            buffer.remove(0, idx);
//            command.clear();
//            command.append(buffer);         // |command| contiene token <INIT> e quello che viene ricevuto subito dopo
//            command.remove(0, INIT_DIM);    // rimuovo <INIT> da |out|

//            if (command.size() < LEN_NUM){
//                return;
//            }

//            tmp = command.left(LEN_NUM);    // in |tmp| copio 8 byte che descrivono la dimensione del messaggio
//            command.remove(0,LEN_NUM);
//            dim = tmp.toUInt(nullptr, 16);  // |dim| ha dimensione di tmp in decimale

//            uint len = static_cast<uint>(command.size());
//            uint rimane = 0;
//            if (len < dim){
//                rimane = dim - len;
//            }

//            while (rimane > 0){
//                qDebug()<<dim<<len<<dim-len;
//                //socket->waitForReadyRead(100);      // finisco di leggere il resto del messaggio
//                dimRead = sock.read(v, rimane);
//                if ( dimRead < 0){
//                    qDebug() << "errore in socket::read()";
//                }
//                this->command.append( v, static_cast<int>(dimRead) );
//                len = static_cast<uint>(command.size());
//                rimane = dim - len;
//            }
//            command.remove(static_cast<int>(dim), 4096);
//            buffer.remove(0, LEN_NUM + INIT_DIM + static_cast<int>(dim));

//            leggiXML(command);

////            while (rimane > 0){
////                uint i = dim - len;
////                //sock.waitForReadyRead(100); // finisco di leggere il resto del messaggio
////                if (this->sock.read(v, rimane) < 0){
////                    qDebug() << "errore in socket::read()";
////                }
////                this->command.append( v );
////                len = static_cast<uint>(command.size());
////                rimane = dim - len;
////            }
///
///
////            command.remove(static_cast<int>(dim), 4096);
////            buffer.remove(0, LEN_NUM + INIT_DIM + static_cast<int>(dim));

////            leggiXML(command);
//            // finito di leggere i byte del messaggio ritorno al ciclo iniziale
//        } else {
//            // token nonn trovato
//        }
//    }
//}

    void MySocket::readyRead(){
        QByteArray out;
        QByteArray tmp;
        uint dim;
        char v[4096] = {};
        qint64 dimRead = 0;

        out.reserve(4096);
        tmp.reserve(16);

        if (sock.bytesAvailable() > 0){
            dimRead = sock.read(v, 4096);
            if ( dimRead < 0){
                qDebug() << "errore in socket::read()";
            }
            this->buffer.append( v, static_cast<int>(dimRead) );
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
                    qDebug()<<dim<<len<<dim-len;
                    //socket->waitForReadyRead(100);      // finisco di leggere il resto del messaggio
                    dimRead = sock.read(v, rimane);
                    if ( dimRead < 0){
                        qDebug() << "errore in socket::read()";
                    }
                    this->command.append( v, static_cast<int>(dimRead) );
                    len = static_cast<uint>(command.size());
                    rimane = dim - len;
                }


                command.remove(static_cast<int>(dim), 4096);
                buffer.remove(0, LEN_NUM + INIT_DIM + static_cast<int>(dim));

//                if (sock.bytesAvailable() > 0){
//                emit readyRead();
//                }

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
