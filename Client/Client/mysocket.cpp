#include "mysocket.h"

MySocket::MySocket(int sockId)
{
    this->sockId = sockId;
    this->buffer.reserve(4096*10);
    this->command.reserve(4096);
    // this->sock.open(QIODevice::ReadWrite);
    this->sock.setSocketDescriptor(sockId);
    qDebug() << "buffer size: " << this->sock.readBufferSize();

    this->connect(&this->sock, &QTcpSocket::readyRead, this, &MySocket::readyRead, Qt::DirectConnection);
    this->connect(&this->sock, &QTcpSocket::disconnected, this, &MySocket::disconnected, Qt::DirectConnection);
    this->connect(&this->sock, &QTcpSocket::connected, this, &MySocket::connected, Qt::DirectConnection);

    this->connectToHost();
}

void MySocket::leggiXML(QByteArray data)
{
    qDebug() << endl << " -> Leggo XML from server: " << this->sockId << endl << data;
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
                    qDebug() << " <- To Server: "<< sockId << endl
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
    QByteArray data = Comando(comando).toByteArray();
    return this->write( data );
}

bool MySocket::connectToHost()
{
    this->sock.connectToHost(QHostAddress::LocalHost, 2000);
    if (!sock.waitForConnected(3000)) {
        qDebug() << "Error: " << sock.errorString();
        return false;
    }
    return true;
}

void MySocket::leggiMap(QByteArray &qba, int qbaSize)
{
    disconnect(&sock, &QTcpSocket::readyRead, this, &MySocket::readyRead);
    qba.clear();
    char v[4096];
    qint64 dataBlk, size;
    size = static_cast<qint64>(qbaSize);
    if (sock.bytesAvailable() < 1)
        qDebug() << "errore";
    while (size > 0){
        if (size > 4095){
            dataBlk = 4095;
        } else {
            dataBlk = size;
        }
        qint64 byteRead = this->sock.read(v, dataBlk);
        if (byteRead < 0){
            qDebug() << "errore in socket::read() at COMAND";
            return;
        }
        qba.append(v, static_cast<int>(byteRead));
        size = size - byteRead;
    }

    connect(&this->sock, &QTcpSocket::readyRead, this, &MySocket::readyRead, Qt::DirectConnection);
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

void MySocket::connected()
{
    emit s_connected();
}
