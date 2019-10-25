#include "client.h"
#include <RegisterDialog/registerdialog.h>
#include <LoginDialog/logindialog.h>

Client::Client(QObject *parent) : QObject(parent)
{
    connectedDB=false;
    logged=false;
    username="";
    docID="";
    tempUser="";
    tempPass="";

    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 2000);

    connect(socket, &QTcpSocket::connected, this, &Client::connected);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::ConnectionType::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::ConnectionType::DirectConnection);
    connect(this, SIGNAL(spostaCursSignal(int&,int&,char&,QString&)),this->parent(),SLOT(spostaCursor(int&,int&,char&,QString&)));
    connect(this, SIGNAL(cancellaSignal(int&,int&,char&,QString&)),this->parent(),SLOT(cancellaAtCursor(int&,int&,char&,QString&)));



}



bool Client::sendMsg(QByteArray ba){
    if (socket->isOpen() && socket->isWritable()){
        if (this->socket->write(ba, ba.size()) == -1){
            // errore
            qDebug() << "errore scrittura verso server";
        }
    }

    return false;
}

bool Client::sendMsg(QMap<QString, QString> cmd){
    QByteArray ba;
    QXmlStreamWriter wr(&ba);
    wr.writeStartDocument();
    wr.writeStartElement(cmd.value(CMD));

    cmd.remove(CMD);
    foreach (QString key, cmd.keys()) {
        wr.writeTextElement(key, cmd.value(key));

    }
    wr.writeEndElement();
    wr.writeEndDocument();

    int dim = ba.size();
    QByteArray len;
    len = QByteArray::number(dim, 16);
    len.prepend(8 - len.size(), '0');

    ba.prepend(len);
    ba.prepend(INIT);
    //qDebug() << QString(ba);

    return sendMsg(ba);


}

void Client::leggiXML(QByteArray qb)
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
            QString cmd = stream.name().toString();
            command.insert("cmd", cmd);
        }
        token = stream.readNext();
        // leggo elemento variabile
        while ( token == QXmlStreamReader::StartElement ){
            QString name = stream.name().toString(), text = stream.readElementText();
            command.insert(name, text);
            qDebug() << "start elem: " << name;
            qDebug() << "val elem: " << text;
            token = stream.readNext();
        }
    }

    // TODO in caso di messaggio non corretto riparti da stato corretto
    if (stream.hasError()){
        qDebug() << "finito lettura xml con errore" << stream.errorString();
    } else {
        qDebug() << "finito lettura xml no errori " << stream.errorString();
        this->dispatchCmd(command);
    }
}


void Client::dispatchCmd(QMap<QString, QString> cmd){
     auto comando = cmd.find("cmd");

     if(comando.value()=="CRS"){
        spostaCursori(cmd);
     }

     else if(comando.value()=="BROWS"){
        handleBrowse(cmd);
     }

     else if(comando.value()=="OK"){
        dispatchOK(cmd);
     }

     else if(comando.value()=="ERR"){
        dispatchERR(cmd);
     }

     else if (comando.value() == REM_IN || comando.value() == REM_DEL) {
        inserimento(cmd);

     }
    //TODO
}

void Client::dispatchOK(QMap <QString, QString> cmd){
    auto comando = cmd.find("MEX");

    if(comando.value()==" connesso a db con utente: "){
        this->connectedDB=true;
    }

    else if(comando.value()=="login effettuato"){
        QMessageBox Messaggio;
        Messaggio.information(0,"Login","Logged in successfully");
        Messaggio.setFixedSize(500,200);

        this->logged=true;
    }

    else if(comando.value()=="logout effettuato"){
        QMessageBox Messaggio;
        Messaggio.information(0,"Logout","Logged out successfully");
        Messaggio.setFixedSize(500,200);

        connect(this,SIGNAL(deleteListSig()),this->parent(),SLOT(deleteListSlot()));
        emit deleteListSig();

        this->logged=false;

    }

    else if(comando.value()=="registrazione completata"){
        QMap<QString,QString> LOGCMD;
        LOGCMD.insert(CMD,LOGIN);
        LOGCMD.insert("username",tempUser);
        LOGCMD.insert("password",tempPass);

        this->sendMsg(cmd);

    }
}

void Client::dispatchERR(QMap <QString,QString>cmd){
    auto comando = cmd.find("MEX");

    if(comando.value()=="impossibile connettersi al db"){
        QMessageBox Messaggio;
        Messaggio.critical(0,"DB ERROR",comando.value());
        Messaggio.setFixedSize(500,200);
    }

    if(comando.value()=="login fallito, user o password errati"){
        QMessageBox Messaggio;
        Messaggio.information(0,"Login Error", comando.value());
        Messaggio.setFixedSize(500,200);

        LoginDialog* loginDialog = new LoginDialog( );
        connect( loginDialog, SIGNAL (acceptLogin(QString&,QString&)), this, SLOT (handleLogin(QString&,QString&)) );
        loginDialog->exec();
    }

    if(comando.value()=="logout fallito"){
        QMessageBox Messaggio;
        Messaggio.information(0,"Logout", comando.value());
        Messaggio.setFixedSize(500,200);

    }

    if(comando.value()=="Registrazione annullata, user già in uso o impossibile aggiungerlo"){
        QMessageBox Messaggio;
        Messaggio.information(0,"Logout", comando.value());
        Messaggio.setFixedSize(500,200);

        RegisterDialog* registerdialog = new RegisterDialog( );
        connect( registerdialog, SIGNAL (acceptRegistration(QString&,QString&)), this, SLOT (handleRegistration(QString&,QString&)) );
        registerdialog->exec();

    }
}

void Client::handleBrowse(QMap<QString,QString> cmd){
    BrowseWindow *b= new BrowseWindow();
    b->exec();

    QString delimiter="==>";
    connect(b, SIGNAL(openFileSignal(QString&)),this, SLOT(remoteOpen(QString&)));
    QList<QString> listanomi,listaID;
    int i=0;

    /****** creo una mappa MAP <DOCID, FILENAME> e poi aggiungo la scelta alla finestra come docid+filename,
     ******  in modo che quando mi viene ritornata la scelta ho l'informazione sull id e sul nome
     ******  ed evito di avere duplicati nella tendina della scelta *****/

    foreach(QString s, cmd.keys()){i++; if(s=="filename"+QString::number(i)){ listanomi.push_back(cmd.value(s)); } }
    i=0;
    foreach(QString s, cmd.keys()){i++; if(s=="docID"+QString::number(i)){ listaID.push_back(cmd.value(s)); } }


    for(int j=0; j<i; j++){
        files.insert(listaID[j],listanomi[j]);
    }


    foreach(QString s, cmd.keys()){
        QString temp=s+delimiter+cmd.value(s);
        b->addScelta(temp);
    }

    b->exec();
}

void Client::inserimento(QMap<QString,QString> cmd){
    QString user=cmd.find("username").value();
    int posX=cmd.find("posX").value().toInt();
    int posY=cmd.find("posX").value().toInt();
    char c=cmd.find("char").value().at(0).toLatin1();

    if(user==this->username){ qDebug()<<"Questo messaggio non doveva arrivare a me!!!"; return; }

    if(c=='\x3' || c=='\x7'){
        emit cancellaSignal(posX,posY,c,user);
    }

    emit spostaCursSignal(posX,posY,c,user);
}

void Client::spostaCursori(QMap <QString,QString>cmd){
    QString user=cmd.find("username").value();
    int posX=cmd.find("posX").value().toInt();
    int posY=cmd.find("posX").value().toInt();
    char c='\0';

    if(user==this->username){ qDebug()<<"Questo messaggio non doveva arrivare a me!!!"; return; }

    emit spostaCursSignal(posX,posY,c,user);
}

/*********************************************************************************************************
 ************************ public slots *******************************************************************
 *********************************************************************************************************/

void Client::connected(){
    qDebug()<<"Connesso al server\n";
    QMap<QString, QString> comando;
    comando.insert("CMD", CONN);
    this->sendMsg(comando);
}

void Client::disconnected(){
    qDebug()<<"Disconnesso dal server\n";
    this->connectedDB=false;
}

void Client::handleLogin(QString& username, QString& password){

    int i=1,j=2;
    QString s="pippo";
    char c='\x8';

    emit spostaCursSignal(i,j,c,s);


    QMap<QString, QString> comando;
    if(socket->state() != QTcpSocket::ConnectedState || !connectedDB){
        QMessageBox Messaggio;
        Messaggio.critical(0,"Login Error","User not connected to the server");
        Messaggio.setFixedSize(500,200);
        return;
    }
    this->username=username;
    this->logged=true;

    comando.insert("CMD", LOGIN);
    comando.insert("username", username);
    comando.insert("password", password);
    this->sendMsg(comando);
}

void Client::handleLogout(){

    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){
        QMessageBox Messaggio;
        Messaggio.critical(0,"Logout Error","User not connected or not logged to the server");
        Messaggio.setFixedSize(500,200);
        return;
    }

    QMap<QString, QString> comando;

    comando.insert("CMD", LOGOUT);
    comando.insert("username", username);
    //comando.insert("password", password);
    this->sendMsg(comando);

}

void Client::handleRegistration(QString& username, QString& password){

    if(socket->state() != QTcpSocket::ConnectedState || !connectedDB){
        QMessageBox Messaggio;
        Messaggio.critical(0,"Registration Error","User not connected to the server");
        Messaggio.setFixedSize(500,200);
        return;
    }

    QMap<QString, QString> comando;

    comando.insert("CMD", LOGIN);
    comando.insert("username", username);
    comando.insert("password", password);

    tempUser=username;
    tempPass=password;

    qDebug()<<comando;
    this->sendMsg(comando);
}

void Client::handleMyCursorChange(int& posX,int& posY){

    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){ return; }

    QMap<QString, QString> comando;

    comando.insert("CMD",CRS);
    comando.insert("username",this->username);
    comando.insert("posX", QString::number(posX) );
    comando.insert("posY", QString::number(posY) );
    qDebug()<<comando;
    this->sendMsg(comando);
}

void Client::remoteOpen(QString& name){

    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){
        QMessageBox Messaggio;
        Messaggio.critical(0,"Network Error","User not connected or not logged to the server");
        Messaggio.setFixedSize(500,200);
        return;
    }

    QMap<QString, QString> comando;
    QString delimiter="==>";

    /****** name contiene docID==>filename, qui lo splitto nelle due stringhe e mando il messaggio ********/

    int pos = name.indexOf(delimiter);
    int pos2 = pos+delimiter.size();

    QString docID=name.mid(0,pos);
    QString filename=name.mid( pos2 , name.size()-pos2 );

    comando.insert("CMD","OPEN-FILE");
    comando.insert("username",username);
    comando.insert("doc",docID);
    comando.insert("filename",filename);

    sendMsg(comando);
}

void Client::remoteAdd(QString& name){
    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){
        QMessageBox Messaggio;
        Messaggio.critical(0,"Network Error","User not connected or not logged to the server");
        Messaggio.setFixedSize(500,200);
        return;
    }

    QMap<QString, QString> comando;
    QString delimiter="==>";

    /****** name contiene docID==>filename, qui lo splitto nelle due stringhe e mando il messaggio ********/

    int pos = name.indexOf(delimiter);
    int pos2 = pos+delimiter.size();

    QString docID=name.mid(0,pos);
    QString filename=name.mid( pos2 , name.size()-pos2 );

    comando.insert("CMD","ADD-File");
    comando.insert("username",username);
    comando.insert("filename",filename);

    sendMsg(comando);
}

void Client::remoteInsert(QChar& c, int posx, int posy){

    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){ return; }

    QMap<QString,QString> cmd;
    cmd.insert("CMD", REM_IN);
    cmd.insert("char", c);
    cmd.insert("cursor", QString::number(posx));
    cmd.insert("index", QString::number(posy));
    cmd.insert("username",username);
    cmd.insert("docid",docID);
}

void Client::remoteDelete(QChar& c, int posx, int posy){

    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){ return; }

    QMap<QString,QString> cmd;
    cmd.insert("CMD", REM_DEL);
    cmd.insert("char", c);
    cmd.insert("cursor", QString::number(posx));
    cmd.insert("index", QString::number(posy));
    cmd.insert("username",username);
    cmd.insert("docid",docID);
}

void Client::readyRead(){
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));  // disconnetto lo slot cosi da non avere più chiamate nello stesso

    QByteArray out;
    QByteArray tmp;
    uint dim;
    //buffer.resize(4096);
    out.resize(1024);
    tmp.resize(16);

    while (socket->bytesAvailable() > 0){
        buffer.append(socket->readAll());

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

                tmp = out.left(LEN_NUM);                  // in |tmp| copio 8 byte che descrivono la dimensione del messaggio
                out.remove(0,LEN_NUM);
                dim = tmp.toUInt(nullptr, 16); // |dim| ha dimensione di tmp in decimale

                uint rimane = dim - static_cast<uint>(out.size());
                while (rimane > 0){
                    uint i = dim - static_cast<uint>(out.size());
                    socket->waitForReadyRead(100);      // finisco di leggere il resto del messaggio
                    out.append(socket->read(i));
                    rimane = dim - static_cast<uint>(out.size());
                }

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

