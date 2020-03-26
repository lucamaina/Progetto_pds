#include "client.h"
#include <RegisterDialog/registerdialog.h>
#include <LoginDialog/logindialog.h>
#include "../BrowseWindow/browsewindow.h"
#include "../NuovoFileRemotoWindoow/nuovofileremoto.h"

Client::Client(QObject *parent) : QObject(parent)
{
    connectedDB=false;
    logged=false;
    username="";
    docID="";
    filename="";
    tempUser="";
    tempPass="";
    remoteFile = nullptr;
    finestraAddFile = new nuovoFileRemoto();

    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 2000);

    connect(socket, &QTcpSocket::connected, this, &Client::connected);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::ConnectionType::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::ConnectionType::DirectConnection);
//    connect(this, SIGNAL(spostaCursSignal(int&,int&,int&,char&,QString&)),this->parent(),SLOT(spostaCursor(int&,int&,int&,char&,QString&)));
    connect(this, SIGNAL(cancellaSignal(int&,int&,int&,char&,QString&)),this->parent(),SLOT(cancellaAtCursor(int&,int&,int&,char&,QString&)));
    connect(this, SIGNAL(cambiaFile(QString&)),this->parent(),SLOT(loadFile(QString&)));
    connect(this, SIGNAL(addMe()),this->parent(),SLOT(addMeSlot()));
    connect(this, SIGNAL(nuovoStile(QString&,QString&)), this->parent(),SLOT(nuovoStileSlot(QString&,QString&)));
    connect(finestraAddFile, SIGNAL(s_addNewFile(QString&)), this, SLOT(remoteAdd(QString&)));
}



bool Client::sendMsg(QByteArray ba){
    if (socket->isOpen() && socket->isWritable()){
        if (this->socket->write(ba, ba.size()) == -1){
            // errore
            qDebug() << "errore scrittura verso server";
            return false;
        }
    }
    return true;
}

void Client::sendAddUsers(QStringList &lista)
{
    qDebug() << "sono in Client::sendAddUsers >> " << lista;
    QMap<QString, QString> cmd;
    cmd.insert(CMD, ADD_USER);
    cmd.insert(DOCID, this->docID);
    int i = 1;
    for (QString utente: lista){
        cmd.insert(UNAME + QString::number(i), utente);
    }
    this->sendMsg(cmd);
}

void Client::sendRemoveUsers(QStringList &lista)
{
    qDebug() << "sono in Client::sendRemoveUsers >> " << lista;
    QMap<QString, QString> cmd;
    cmd.insert(CMD, REM_USER);
    cmd.insert(DOCID, this->docID);
    int i = 1;
    for (QString utente: lista){
        cmd.insert(UNAME + QString::number(i), utente);
    }
    this->sendMsg(cmd);
}

QString Client::getUsername() const
{
    return username;
}


/**
 * @brief Client::handleNuovoFile
 * @param filename
 * slot per inviare al server addNewFile
 */
void Client::handleNuovoFile()
{  
    qDebug() << "sono in Client::handleNuovoFile";

    finestraAddFile->exec();

    finestraAddFile->close();

}

/**
 * @brief Client::handleAddUser
 * chiama finestra per invitare users
 */
void Client::handleAddUser()
{
    qDebug() << "sono in Client::handleAddUser";

    if (!this->docID.isEmpty()){

        QMap<QString, QString> cmd;
        cmd.insert(CMD, ULIST);
        cmd.insert(DOCID, this->docID);
        this->sendMsg(cmd);

        finestraUsers = new UserManager(this->username);
        connect(this, SIGNAL(s_userList(QList<QString>&)), finestraUsers, SLOT(caricaUsers(QList<QString>&)));
        connect(finestraUsers, SIGNAL(s_addUsers(QStringList&)), this, SLOT(sendAddUsers(QStringList&)));
        connect(finestraUsers, SIGNAL(s_removeUsers(QStringList&)), this, SLOT(sendRemoveUsers(QStringList&)));

        this->finestraUsers->exec();


    } else {
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"Alert","Nessun documento selezionato");
        Messaggio.setFixedSize(500,200);
    }


}

void Client::listUser(QMap<QString, QString> cmd)
{
    qDebug() << "sono in Client::listUser";
    QList<QString> lista;
    int i = 1;
    for (QString key : cmd.keys() ){
        if (key == UNAME + QString::number(i)){
            lista.append(cmd.value(key));
            i++;
        }
    }
    emit this->s_userList(lista);
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

bool Client::leggiXML(QByteArray qb)
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
            command.insert(CMD, cmd);
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
        if (command.contains(OK) && command.contains(MEX)){
            return true;
        } else if (command.contains(ERR) && command.contains(MEX)) {
            return false;
        }
        this->dispatchCmd(command);
    }
    return true;
}

/**
 * @brief Client::dispatchCmd
 * @param cmd
 * riceve comando e chiama funzione correlata
 */
void Client::dispatchCmd(QMap<QString, QString> cmd){
    auto comando = cmd.find(CMD);

    if(comando.value()==CRS){
        spostaCursori(cmd);
    }
    else if(comando.value()==BROWS){
        handleBrowse(cmd);
    }
    else if(comando.value()==OK){
        dispatchOK(cmd);
    }
    else if(comando.value()==ERR){
        dispatchERR(cmd);
    }
    else if(comando.value()=="STILE"){
        dispatchStile(cmd);
    }
    else if (comando.value() == REM_IN){
        inserimento(cmd);
    }
    else if ( comando.value() == REM_DEL) {
        cancellamento(cmd);
    }
    else if (comando.value() == FBODY) {
        loadFile(cmd);
    }
    else if (comando.value() == ULIST) {
        listUser(cmd);
    }
    else if (comando.value() == UP_CRS) {
        this->upCursor(cmd);
    }

    //TODO
}

/**
 * @brief Client::dispatchOK
 * @param cmd
 * mostra ricezione messaggio di OK
 */
void Client::dispatchOK(QMap <QString, QString> cmd){
    auto comando = cmd.find(MEX);

    if(comando.value()== CONN_OK){
        this->connectedDB=true;
        qDebug() << "Connesso";
    }

    else if(comando.value()==LOGIN_OK){
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"Login","Logged in successfully");
        Messaggio.setFixedSize(500,200);

        emit addMe();
        this->logged=true;
    }

    else if(comando.value()==LOGOUT_OK){
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"Logout","Logged out successfully");
        Messaggio.setFixedSize(500,200);

        connect(this,SIGNAL(deleteListSig()),this->parent(),SLOT(deleteListSlot()));
        emit deleteListSig();

        this->logged=false;

    }

    else if(comando.value()==REG_OK){
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"Registration","Registered & Logged in successfully");
        Messaggio.setFixedSize(500,200);

        this->logged=true;
    } else if(comando.value()==FILE_OK){
        emit this->toStatusBar("File opened successfully");
        this->logged=true;
    } else {
        qDebug() << comando.key();
    }


}

/**
 * @brief Client::dispatchERR
 * @param cmd
 * mostra ricezione messaggio di ERR
 */
void Client::dispatchERR(QMap <QString,QString>cmd){
    auto comando = cmd.find(MEX);

    if(comando.value()==CONN_ERR){
        QMessageBox Messaggio;
        Messaggio.critical(nullptr,"DB ERROR",comando.value());
        Messaggio.setFixedSize(500,200);
    }

    if(comando.value()==LOGIN_ERR){
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"Login Error", comando.value());
        Messaggio.setFixedSize(500,200);

        LoginDialog* loginDialog = new LoginDialog( );
        connect( loginDialog, SIGNAL (acceptLogin(QString&,QString&)), this, SLOT (handleLogin(QString&,QString&)) );
        loginDialog->exec();
    }

    if(comando.value()==LOGOUT_ERR){
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"Logout", comando.value());
        Messaggio.setFixedSize(500,200);

    }

    if(comando.value()==REG_ERR){
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"Logout", comando.value());
        Messaggio.setFixedSize(500,200);

        RegisterDialog* registerdialog = new RegisterDialog( );
        connect( registerdialog, SIGNAL (acceptRegistration(QString&,QString&)), this, SLOT (handleRegistration(QString&,QString&)) );
        registerdialog->exec();

    }

    if(comando.value()==FILE_ERR){
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"File open", comando.value());
        Messaggio.setFixedSize(500,200);
    }
    else if (comando.value() == ADD_USER_ERR) {
        this->showUserError(cmd);

    }
}

/**
 * @brief Client::handleBrowse
 * @param cmd
 * gestisce lista con nome dei file apribili dall'utente
 */
void Client::handleBrowse(QMap<QString,QString> cmd){
    BrowseWindow *b= new BrowseWindow();
    //b->exec();

    QString delimiter="==>";
    connect(b, SIGNAL(openFileSignal(QString&, QString&)),this, SLOT(remoteOpen(QString&, QString&)));
    QList<QString> listanomi,listaID;
    QMap<QString, QString> fileMap;     // key docId, value filename
    int i=1;

    /****** creo una mappa MAP <DOCID, FILENAME> e poi aggiungo la scelta alla finestra come docid+filename,
     ******  in modo che quando mi viene ritornata la scelta ho l'informazione sull id e sul nome
     ******  ed evito di avere duplicati nella tendina della scelta *****/
/*
    foreach(QString s, cmd.keys()){i++; if(s=="filename"+QString::number(i)){ listanomi.push_back(cmd.value(s)); } }
    i=0;
    foreach(QString s, cmd.keys()){i++; if(s=="docID"+QString::number(i)){ listaID.push_back(cmd.value(s)); } }
*/

    foreach (QString s, cmd.keys() ){
        if (s == DOCID + QString::number(i)){
            QString codId = cmd.value(s);
            QString fileName = cmd.value(FNAME + QString::number(i));
            fileMap.insert(codId, fileName);
            //b->addScelta(fileName);
            i++;
        }
    }
    b->setFileMap(fileMap);

    b->exec();
    b->close();
/*
    for(int j=0; j<i; j++){
        files.insert(listaID[j],listanomi[j]);
    }


    foreach(QString s, cmd.keys()){
        QString temp=s+delimiter+cmd.value(s);
        b->addScelta(temp);
    }

    b->exec();*/
}

/**
 * @brief Client::nuovoFile
 * @param cmd
 * gestisce apertura di un file ricevuto dal server
 */
void Client::loadFile(QMap<QString,QString> cmd){
    qDebug() << cmd.toStdMap();
    qint64 dim = cmd.find(BODY).value().toInt();

    QByteArray qba;
    char v[4096];
    disconnect(this,SIGNAL(readyRead()),this,SLOT(readyRead()));
    qba.clear();
    qba = this->buffer;
    //lettura socket
    dim = dim - qba.size();
    while (dim > 0){
        if (socket->bytesAvailable() > 0){
            qint64 read = socket->read(v, 4096);
            if ( read < 0){
                qDebug() << "errore in socket::read()";
                return;
            }
            qba.append( v );
            dim = dim - read;
        } else {
            return;
        }
    }
    qDebug () << qba;

    connect(this,SIGNAL(readyRead()),this,SLOT(readyRead()));

    //emit clearEditor();
    this->remoteFile = new Editor(this->docID,this->filename,qba,username);
    QString testo = this->remoteFile->getTesto();
    emit this->s_loadEditor(testo);
}

/**
 * @brief Client::inserimento
 * @param cmd
 * esegue inserimento nella mappa locale ed emette signal per visualizzare il carattere in texteditor
 */
void Client::inserimento(QMap<QString,QString> cmd)
{
    QString user = cmd.find(UNAME).value();
    double index = cmd.find(IDX).value().toDouble();
    QByteArray format = QByteArray::fromHex(cmd.find(FORMAT).value().toUtf8());
    QTextCharFormat charform = deserialize(format);
    char c = cmd.find(CAR).value().at(0).toLatin1();

    double oldIndex = this->remoteFile->insertLocal(index,c, charform);
    if (oldIndex > 0){
        int posCursor = remoteFile->localPosCursor(oldIndex);
        emit s_removeText(posCursor);
    }
    int posCursor = remoteFile->localPosCursor(index);
    emit s_setText(c, charform, posCursor);
}

void Client::cancellamento(QMap<QString, QString> cmd)
{
    double index = cmd.find(IDX).value().toDouble();
    char c = cmd.find(CAR).value().at(0).toLatin1();

    int posCursor = this->remoteFile->deleteLocal(index, c);

    emit s_removeText(posCursor);
}

void Client::spostaCursori(QMap <QString,QString>cmd)
{
    qDebug() << "sono in " << Q_FUNC_INFO;
    QString user=cmd.find(UNAME).value();
    int posX=cmd.find("posX").value().toInt();
    int posY=cmd.find("posX").value().toInt();
    int anchor=cmd.find("anchor").value().toInt();
    char c='\0';

    if(user==this->username){ qDebug()<<"Questo messaggio non doveva arrivare a me!!!"; return; }

    emit spostaCursSignal(posX,posY,anchor,c,user);
}

/**
 * @brief Client::showUserError
 * @param cmd
 * mostra utenti con errore di ADD_USER
 */
void Client::showUserError(QMap<QString, QString> cmd)
{
    int i = 1;
    QString nome, str;
    do {
        if (cmd.contains(UNAME+QString::number(i))){
            nome = cmd.value(UNAME+QString::number(i), "");
            str += nome + "; ";
        }
    } while (nome != "");

    QMessageBox Messaggio;
    Messaggio.setText("Errore inserimento utenti");
    Messaggio.information(nullptr,"Errore: ", str);
    Messaggio.setFixedSize(500,200);
}

void Client::dispatchStile(QMap <QString,QString>cmd){
    QString stile=cmd.find("stile").value();
    QString param=cmd.find("param").value();

    emit nuovoStile(stile,param);
}


/*********************************************************************************************************
 ************************ public slots *******************************************************************
 *********************************************************************************************************/

void Client::connected(){
    qDebug()<<"Connesso al server\n";
    QMap<QString, QString> comando;
    this->connectedDB=false;
    comando.insert(CMD, CONN);
    this->sendMsg(comando);
}

void Client::disconnected(){
    qDebug()<<"Disconnesso dal server\n";
    this->connectedDB=false;
}
void Client::handleStile(QString& stile,QString& param){
    QMap<QString, QString> comando;
    comando.insert(CMD, "STILE");
    comando.insert("stile", stile);
    comando.insert("param", param);

    qDebug()<<comando;

    this->sendMsg(comando);
}

void Client::connectSlot(){
    if(socket->state() != QTcpSocket::ConnectedState || socket == nullptr){
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 2000);
    }
    // TODO richiama CONNECT
    this->connected();
}


/****************************************************************************
 ***************** metodi controllo dell'utente *****************************/

void Client::handleLogin(QString& username, QString& password)
{
    emit addMe(); //PROVA

    QMap<QString, QString> comando;
    if(socket->state() != QTcpSocket::ConnectedState || !connectedDB){
        QMessageBox Messaggio;
        Messaggio.critical(nullptr,"Login Error","User not connected to the server");
        Messaggio.setFixedSize(500,200);

        return;
    }
    this->username=username;
    this->logged=true;

    comando.insert(CMD, LOGIN);
    comando.insert(UNAME, username);
    comando.insert(PASS, password);
    this->sendMsg(comando);
}

void Client::handleLogout(){

    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){
        QMessageBox Messaggio;
        Messaggio.critical(nullptr,"Logout Error","User not connected or not logged to the server");
        Messaggio.setFixedSize(500,200);
        return;
    }

    QMap<QString, QString> comando;

    comando.insert(CMD, LOGOUT);
    comando.insert(UNAME, username);
    //comando.insert("password", password);
    this->sendMsg(comando);

}

void Client::handleRegistration(QString& username, QString& password){

    if(socket->state() != QTcpSocket::ConnectedState || !connectedDB){
        QMessageBox Messaggio;
        Messaggio.critical(nullptr,"Registration Error","User not connected to the server");
        Messaggio.setFixedSize(500,200);
        return;
    }

    QMap<QString, QString> comando;

    comando.insert(CMD, REG);
    comando.insert(UNAME, username);
    comando.insert(NICK, username);
    comando.insert("password", password);

    tempUser=username;
    tempPass=password;

    qDebug()<<comando;
    this->sendMsg(comando);
}

void Client::handleMyCursorChange(int& posX,int& posY, int& anchor)
{
    qDebug() << "sono in " << Q_FUNC_INFO;
    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){ return; }

    QMap<QString, QString> comando;

    comando.insert(CMD,CRS);
    comando.insert(UNAME,this->username);
    comando.insert("posX", QString::number(posX) );
    comando.insert("posY", QString::number(posY) );
    comando.insert("anchor", QString::number(anchor) );
    qDebug()<<comando;
    //this->sendMsg(comando);
}

void Client::pasteSlot(QString& clipboard){
    //if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){ return; }

    QMap<QString, QString> comando;

    comando.insert(CMD,"PASTE");
    comando.insert("clipboard",clipboard);
    qDebug()<<comando;
    this->sendMsg(comando);
}

void Client::remoteOpen(QString& name, QString& docID){

    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){

        QMessageBox Messaggio;
        Messaggio.critical(nullptr,"Network Error","User not connected or not logged to the server");
        Messaggio.setFixedSize(500,200);
        return;
    }

    QMap<QString, QString> comando;
/*    QString delimiter="==>";

    ****** name contiene docID==>filename, qui lo splitto nelle due stringhe e mando il messaggio ********

    int pos = name.indexOf(delimiter);
    int pos2 = pos+delimiter.size();

    this->docID=name.mid(0,pos);
    this->filename=name.mid( pos2 , name.size()-pos2 );
*/
    comando.insert(CMD,OPEN_FILE);
    comando.insert(UNAME,username);
    comando.insert(DOCID,docID);
    comando.insert(FNAME,name);
    this->docID = docID;
    this->filename = name;

    sendMsg(comando);
}

void Client::remoteAdd(QString& name){
    qDebug() << "sono in Client::remoteAdd";
    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){
        QMessageBox Messaggio;
        Messaggio.critical(nullptr, "Network Error","User not connected or not logged to the server");
        Messaggio.setFixedSize(500,200);
        return;
    }

    QMap<QString, QString> comando;
    comando.insert(CMD, ADD_FILE);
    comando.insert(FNAME, name);

    sendMsg(comando);
}

QString Client::serialize(const QTextCharFormat &format)
{
    QByteArray s;
    QDataStream out(&s,QIODevice::ReadWrite);
    out <<format;
    qDebug() << s;
    return QString(s.toHex());
}

QTextCharFormat Client::deserialize(QByteArray &s)
{
    QTextCharFormat frm;
    QDataStream out(&s,QIODevice::ReadWrite);
    out >> frm;
    return frm;
}

/**
 * @brief Client::upCursor
 * @param cmd
 * @return
 * riceve lista di utenti connessi ed emette signal per textEditor
 */
bool Client::upCursor(QMap<QString, QString> cmd)
{
    qDebug() << "sono in " << Q_FUNC_INFO;
    QString tmpDocID, tmpNome;
    if (cmd.value(DOCID) != this->docID)
        return false;

    int i = 1;
    QStringList list;
    for (QString key : cmd.keys()){
        if (key == UNAME+QString::number(i)){
            list.append(cmd.value(key));
            i++;
        }
    }
    qDebug() << list;
    emit s_upCursor(list);
    return true;
}


bool Client::remoteInsert(QChar c, QTextCharFormat format, double index)
{
    QMap<QString,QString> cmd;
    cmd.insert(CMD, REM_IN);
    cmd.insert(CAR, c);
    cmd.insert(IDX, QString::number(index));
    cmd.insert(FORMAT, QString(this->serialize(format)));
    cmd.insert(UNAME,username);
    cmd.insert(DOCID,docID);

    qDebug() << cmd;

    return this->sendMsg(cmd);
}

bool Client::remoteDelete(QChar c, double index)
{
    QMap<QString,QString> cmd;
    cmd.insert(CMD, REM_DEL);
    cmd.insert(CAR, c);
    cmd.insert(IDX, QString::number(index));
    cmd.insert(UNAME,username);
    cmd.insert(DOCID,docID);

    return this->sendMsg(cmd);
}




void Client::readyRead(){
    QByteArray out;
    QByteArray tmp;
    uint dim;
    char v[4096] = {};

    out.reserve(4096);
    tmp.reserve(16);

    if (socket->bytesAvailable() > 0){
        if (socket->read(v, 4096) < 0){
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
                //socket->waitForReadyRead(100);      // finisco di leggere il resto del messaggio
                if (socket->read(v, i) < 0){
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
