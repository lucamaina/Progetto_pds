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
    // TODO modifica per invio grandi file
    qint64 ret;
    QBuffer qbuf;
    qbuf.setData(data);
    qbuf.open(QIODevice::ReadWrite);

    if (sock.isOpen()){
        if ( sock.isWritable() ){
            ret = this->sock.write(qbuf.data(), data.size());
            bool b = this->sock.waitForBytesWritten();
            auto err = sock.error();
            if ( ret == data.size()){
                qDebug() << " <- To Client: "<< sockId << endl
                         << "write byte: " << ret << " - "
                         << data.left(100);
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
    QByteArray data = Comando(comando).toByteArray();
    return this->write( data );
}

void MySocket::readyRead()
{
    QByteArray tmp;
    qint64 dim, dataBlk;
    char v[4096] = {};
    int error = 0;

    tmp.reserve(16);

    while (sock.bytesAvailable()){
        // LEGGO <INIT>
        buffer.clear();
        if (this->sock.bytesAvailable() >= INIT_DIM){
            if (this->sock.read(v, INIT_DIM) < INIT_DIM){
                qDebug() << "errore in socket::read() at INIT";
                return;
            }
            this->buffer.append(v, INIT_DIM);
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
            tmp.append(v, LEN_NUM);
            dim = static_cast<qint64>( tmp.toInt(nullptr, 16) );
        } else {
            return;
        }

        // LEGGO COMANDO
        command.clear();
        while (dim > 0 && error == 0){
            if (dim > 4096){
                dataBlk = 4096;
            } else {
                dataBlk = dim;
            }
            qint64 byteRead = this->sock.read(v, dataBlk);
            if (byteRead < 0){
                qDebug() << "errore in socket::read() at COMAND";
                return;
            }
            command.append(v, static_cast<int>(byteRead));
            dim = dim - byteRead;
        }

        leggiXML(command);
    }
}

void MySocket::disconnected()
{
    emit s_disconnected();
}
