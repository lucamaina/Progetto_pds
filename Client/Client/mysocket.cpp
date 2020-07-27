#include "mysocket.h"

#include <QCoreApplication>
#include <QProgressDialog>

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

void MySocket::connectReadyRead(bool set)
{
    if (set) {
        connect(&this->sock, &QTcpSocket::readyRead, this, &MySocket::readyRead, Qt::DirectConnection);
    } else {
        disconnect(&sock, &QTcpSocket::readyRead, this, &MySocket::readyRead);
    }
}

void MySocket::connectFileReadyRead(bool set)
{
    if (set) {
        connect(&this->sock, &QTcpSocket::readyRead, this, &MySocket::fileReadyRead, Qt::DirectConnection);
    } else {
        disconnect(&sock, &QTcpSocket::readyRead, this, &MySocket::fileReadyRead);
    }
}

void MySocket::startLoadFromRemote(int size)
{
    this->connectReadyRead(false);
    this->connectFileReadyRead(true);
    this->fileBuffer.clear();
    this->fileBuffer.reserve(size);
    this->dimFile = size;
}

void MySocket::stopLoadFromRemote(QByteArray& qba)
{
    this->connectFileReadyRead(false);
    this->connectReadyRead(true);
    emit s_loadFile(qba);
}

/**
 * viene eseguita in un altro thread con QtConcurrent::run
 */
QByteArray MySocket::leggiMap(int dataSize)
{
    QByteArray qba;
    qba.reserve(dataSize);
    char v[4096];
    qint64 dataBlk, size, byteRead;
    size = static_cast<qint64>(dataSize);
    if (sock.bytesAvailable() < 0)
        qDebug() << endl << "errore byte available";
    this->sock.waitForReadyRead(1000);
    while (size > 0){
        qDebug() << "loppo qui";
        if (size > 4096){
            dataBlk = 4096;
        } else {
            dataBlk = size;
        }
        sock.waitForReadyRead(100);
        byteRead = this->sock.read(v, dataBlk);
        if (byteRead < 0){
            qDebug() << "errore in socket::read() at COMAND";
            return qba;
        }
        qba.append(v, static_cast<int>(byteRead));
        size = size - byteRead;
    }
    return qba;
}

void MySocket::readyRead()
{
    QByteArray tmp;
    qint64 dim, dataBlk, byteRead;
    char v[4096] = {};
    tmp.reserve(16);

    while (sock.bytesAvailable()){
        // LEGGO <INIT>
        buffer.clear();

        if (this->sock.bytesAvailable() >= INIT_DIM + LEN_NUM){
           if (this->sock.read(v, INIT_DIM) < INIT_DIM){
               qDebug() << "errore in socket::read() at INIT";
               return;
           }
           this->buffer.append(v, INIT_DIM);
           if (!buffer.contains(INIT)){
               return;
           }
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
        while (dim > 0){
            if (sock.bytesAvailable() == 0){
                sock.waitForReadyRead(100);
            }
            if (dim > 4096){
                dataBlk = 4096;
            } else {
                dataBlk = dim;
            }
            byteRead = this->sock.read(v, dataBlk);
            if (byteRead < 0){
                qDebug() << "errore in socket::read() at COMAND";
                return;
            }
            command.append(v, static_cast<int>(byteRead));
            dim = dim - byteRead;
        }
        if (dim == 0){
            leggiXML(command);
        }
    }
}

void MySocket::fileReadyRead()
{
    char v[4096];
    qint64 dataBlk, byteRead;
    while (sock.bytesAvailable() && dimFile > 0){
        dimFile > 4096 ? dataBlk = 4096 : dataBlk = dimFile;
        byteRead = this->sock.read(v, dataBlk);
        if (byteRead < 0){
            qDebug() << "errore in socket::read() at INIT";
            return;
        }
        this->fileBuffer.append(v, static_cast<int>(byteRead));
        dimFile = dimFile - static_cast<int>(byteRead);
    }
    if (dimFile == 0){
        stopLoadFromRemote(fileBuffer);
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
