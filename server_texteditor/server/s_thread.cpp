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
    qDebug() << "Client connesso";
}

void s_thread::readyRead()
{
    QByteArray out = this->socket->readAll();
    qDebug() << out;
}
