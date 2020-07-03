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
//    QXmlStreamReader stream(&sock);

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

void MySocket::readyRead()
{
    QByteArray out;
    QByteArray tmp;
    qint64 dim;
    char v[4096] = {};
    int error = 0;

    out.reserve(4096);
    tmp.reserve(16);

    // LEGGO <INIT>
    if (this->sock.bytesAvailable() >= INIT_DIM){
        if (this->sock.read(v, INIT_DIM) < INIT_DIM){
            qDebug() << "errore in socket::read() at INIT";
            return;
        }
        this->buffer.append( v );
    } else {
        return;
    }

    // LEGGO DIMENSIONE
    if (this->sock.bytesAvailable() >= LEN_NUM){
        if (this->sock.read(v, LEN_NUM) < LEN_NUM){
            qDebug() << "errore in socket::read() at LEN_NUM";
            return;
        }
        tmp.clear();
        tmp.append(v);
        dim = static_cast<qint64>( tmp.toInt(nullptr, 16) );
    } else {
        return;
    }

    // LEGGO COMANDO
    command.clear();
    while (dim > 0 && error == 0){
        qint64 byteRead = this->sock.read(v, dim);
        command.append(v);
        dim = dim - byteRead;
    }

    leggiXML(command);
}

void MySocket::disconnected()
{
    emit s_disconnected();
}
