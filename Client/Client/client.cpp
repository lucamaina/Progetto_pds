#include "client.h"
#include <RegisterDialog/registerdialog.h>
#include <LoginDialog/logindialog.h>
#include "../BrowseWindow/browsewindow.h"
#include "../NuovoFileRemotoWindoow/nuovofileremoto.h"

Client::Client(QObject *parent) : QObject(parent)
{
    connectedDB = false;
    logged = false;
    username = "";
    docID = "";
    filename = "";
    tempUser = "";
    tempPass.clear();
    remoteFile = nullptr;
    finestraAddFile = new nuovoFileRemoto();
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &Client::connected);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::ConnectionType::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::ConnectionType::DirectConnection);
    connect(this, SIGNAL(cancellaSignal(int&,int&,int&,char&,QString&)),this->parent(),SLOT(cancellaAtCursor(int&,int&,int&,char&,QString&)));
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
        cmd.insert(UNAME + QString::number(i++), utente);
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
bool Client::handleNuovoFile()
{  
    qDebug() << "sono in " << Q_FUNC_INFO;
    finestraAddFile->exec();
    return false;
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

void Client::handleFileExit()
{
    qDebug() << "sono in " << Q_FUNC_INFO;
    if (this->docID.isNull()){
        return;
    }
    QMap<QString, QString> cmd;
    cmd.insert(CMD, EXFILE);
    cmd.insert(DOCID, this->docID);
    this->sendMsg(cmd);

    emit s_clearEditor();
    emit s_setVisbleFileActions(true);
    emit s_setVisbleEditorActions(false);
}

void Client::listUser(QMap<QString, QString> cmd)
{
    qDebug() << "sono in " << Q_FUNC_INFO;
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

void Client::connectToHost()
{
    socket->connectToHost(QHostAddress::LocalHost, 2000);
    if (!socket->waitForConnected(3000)) {
        qDebug() << "Error: " << socket->errorString();
        emit this->s_warning("Error: " + socket->errorString());
    }
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

    qDebug().noquote() << " <- To server:" << endl
                       << QString(ba);

    return sendMsg(ba);


}

bool Client::leggiXML(QByteArray qb)
{
    qDebug().noquote() << endl << " -> Leggo XML from Server: " << qb;
    QMap<QString, QString> command;
    QXmlStreamReader stream(qb);

    while (!stream.atEnd() && !stream.hasError() ){
        QXmlStreamReader::TokenType token = stream.readNext();
        token = stream.readNext();
        // leggo elemento con nome del comando
        if (token == QXmlStreamReader::StartElement){
            QString cmd = stream.name().toString();
            command.insert(CMD, cmd);
        }
        token = stream.readNext();
        // leggo elemnto variabile
        while ( token == QXmlStreamReader::StartElement ){
            QString name = stream.name().toString(), text = stream.readElementText();
            command.insert(name, text);
            token = stream.readNext();
        }
    }

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
    if (!cmd.contains(CMD)){
        return;
    }
    auto comando = cmd.find(CMD);
    if(comando.value()==CRS){
        spostaCursori(cmd);
    }
    else if(comando.value()==BROWSE){
        handleBrowse(cmd);
    }
    else if(comando.value()==OK){
        dispatchOK(cmd);
    }
    else if(comando.value()==ERR){
        dispatchERR(cmd);
    }
    else if (comando.value() == REM_IN){
        inserimentoRemoto(cmd);
    }
    else if ( comando.value() == REM_DEL) {
        cancellamentoRemoto(cmd);
    }
    else if (comando.value() == FBODY) {
        loadFile(cmd);
        emit s_changeTitle(this->username, this->remoteFile->getNomeFile(), this->remoteFile->getDocId());
        emit s_setVisbleFileActions(true);
        emit s_setVisbleEditorActions(true);
    }
    else if (comando.value() == ULIST) {
        listUser(cmd);
    }
    else if (comando.value() == UP_CRS) {
        this->upCursor(cmd);
    }
    else if (comando.value() == STILE) {
        this->dispatchStile(cmd);
    }
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

        this->logged=true;
        this->username = tempUser;

        emit s_changeTitle(this->username, "*", "*");
        emit s_setVisbleFileActions(true);
    }
    else if (comando.value() == ADD_FILE_OK) {
        emit s_brows();
    }
    else if(comando.value()==LOGOUT_OK){
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"Logout","Logged out successfully");
        Messaggio.setFixedSize(500,200);

        connect(this,SIGNAL(deleteListSig()),this->parent(),SLOT(deleteListSlot()));
        emit deleteListSig();

        this->logged = false;
        this->username.clear();

        emit Client::s_changeTitle("","","");
        emit s_clearEditor();
        emit s_setVisbleFileActions(false);
        emit s_setVisbleEditorActions(false);
    }
    else if(comando.value()==REG_OK){
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"Registration","Registered successfully");
        Messaggio.setFixedSize(500,200);

        this->logged=true;
//        this->username = tempUser;

//        emit s_changeTitle(this->username, "*", "*");
//        emit s_setVisbleFileActions(true);
    }
    else if(comando.value()==FILE_OK){
        emit this->toStatusBar("File opened successfully");
        this->logged=true;
    }
    else {
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
        this->username = "";
        emit s_changeTitle("", "", "");

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

        emit Client::s_changeTitle(this->username,"","");
        emit s_setVisbleFileActions(false);
        emit s_setVisbleEditorActions(false);
    }
    else if (comando.value() == ADD_USER_ERR) {
        this->showUserError(cmd);
    }
    else if (comando.value() == ADD_FILE_ERR1 || comando.value() == ADD_FILE_ERR2) {
        QMessageBox Messaggio;
        Messaggio.information(nullptr,"File non aggiunto", comando.value());
        Messaggio.setFixedSize(500,200);
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

    foreach (QString s, cmd.keys() ){
        if (s == DOCID + QString::number(i)){
            QString codId = cmd.value(s);
            QString fileName = cmd.value(FNAME + QString::number(i));
            fileMap.insert(codId, fileName);
            i++;
        }
    }
    b->setFileMap(fileMap);

    b->exec();
    b->close();
}

/**
 * @brief Client::nuovoFile
 * @param cmd
 * gestisce apertura di un file ricevuto dal server
 */
void Client::loadFile(QMap<QString,QString> cmd)
{
    qint64 dim = cmd.find(BODY).value().toInt();

    QByteArray qba;
    char v[4096];
    disconnect(this->socket,SIGNAL(readyRead()),this,SLOT(readyRead()));
    qba.clear();
    qba = this->buffer;
    qDebug().noquote() <<qba.length() << endl << qba;

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
            qDebug().noquote() <<qba.length() << endl << qba;

            dim = dim - read;
        } else {
            return;
        }
    }
    qDebug().noquote() << "sono in " << Q_FUNC_INFO
                       << "byte letti: " << qba.length()
                       << endl << qba;

    connect(this->socket,SIGNAL(readyRead()),this,SLOT(readyRead()));

    //emit clearEditor();
    try {
        this->remoteFile = new Editor(this->docID, this->filename, qba,  username);
    } catch (...) {

    }
    emit this->s_clearEditor();

    int pos = 0;
    for( Symbol s : remoteFile->symVec){

        QTextCharFormat formatoDecode;
        QByteArray esadecimale=QByteArray::fromHex(s.formato);
        formatoDecode=deserialize(esadecimale);
        s.textFormat=formatoDecode;
        emit s_setText(s.car, formatoDecode, pos++);
    }
}

/**
 * @brief Client::inserimento
 * @param cmd
 * esegue inserimento nella mappa locale ed emette signal per visualizzare il carattere in texteditor
 */
void Client::inserimentoRemoto(QMap<QString,QString> cmd)
{
    QString user = cmd.find(UNAME).value();
    if (user == this->username)
        return;// ignoro
    QString indici = cmd.find(IDX).value();
    QVector<qint32> index;
    for (QString s : indici.split(";", QString::SkipEmptyParts)){
         index.push_back(s.toInt());
    }
    QByteArray format = QByteArray::fromHex(cmd.find(FORMAT).value().toUtf8());
    QTextCharFormat charform = deserialize(format);

    QChar c;
    if (cmd.contains(CAR) && !cmd.value(CAR).isNull()){
        c = cmd.value(CAR).at(0);
    } else {
        c = QChar();
    }

    int posCursor = remoteFile->localPosCursor(index);
    Symbol newS;
    if (posCursor > -1){
        if (c.isNull()){
            posCursor--;
            this->remoteFile->getLocalIndexDelete(posCursor, newS);
            this->remoteFile->symVec.replace(posCursor, newS);
            emit s_removeText(posCursor);
            emit s_setText(newS.car, charform, posCursor);
        } else {
            newS = Symbol(user, c, index, charform);
            remoteFile->symVec.insert(posCursor, newS);
            emit s_setText(c, charform, posCursor);
        }
    }

}

bool Client::inserimentoLocale(qint32 pos, QChar car, QTextCharFormat format)
{
    // creo nuovo indice
    QVector<qint32> newIndex = this->remoteFile->getLocalIndexInsert(pos);
    if (newIndex.isEmpty()){
        // errore inserimento locale
        return false;
    }
    // creo symbol
    Symbol newSym;
    if (car.isNull()) {
        // update
        this->remoteFile->getLocalIndexDelete(pos, newSym);
        this->remoteFile->symVec.replace(pos, newSym);
        emit s_removeText(pos);
        emit s_setText(newSym.car, format, pos);
        newSym.car = QChar();
        this->remoteInsert(newSym.car, format, newSym.indici);

    } else {
        newSym = Symbol(this->username, car, newIndex, format);
        // inserisco in locale
        this->remoteFile->symVec.insert(pos, newSym);
        // scrivo in editor
        emit s_setText(newSym.car, format, pos);
            // invio al server
        this->remoteInsert(newSym.car, format, newIndex);

    }
    return true;
}

bool Client::cancellamentoLocale(int posCursor)
{
    // trovo symbol alla posizione del cursore
    Symbol oldSym;
    if ( !this->remoteFile->getLocalIndexDelete(posCursor, oldSym)){
        // errore cancellazione locale
        return false;
    }
    // cancello in locale
    this->remoteFile->symVec.remove(posCursor);

    // scrivo in editor
    emit s_removeText(posCursor);

    // invio al server
    this->remoteDelete(oldSym);
    return true;
}

void Client::cancellamentoRemoto(QMap<QString, QString> cmd)
{
    QString user = cmd.find(UNAME).value();
    if (user == this->username)
        return;// ignoro
    QString indici = cmd.find(IDX).value();
    QVector<qint32> index;
    for (QString s : indici.split(";", QString::SkipEmptyParts)){
         index.push_back(s.toInt());
    }

    int posCursor = remoteFile->findLocalPosCursor(index);
    if (posCursor > -1){
        remoteFile->symVec.remove(posCursor);
        emit s_removeText(posCursor);
    }
}

void Client::spostaCursori(QMap <QString,QString>cmd)
{
    qDebug()<<cmd;
    QString user=cmd.find(UNAME).value();
    QString indici=cmd.find(IDX).value();

    int pos=indici.split(";", QString::SkipEmptyParts)[0].toInt();
    qDebug()<<pos;
    int anchor=indici.split(";", QString::SkipEmptyParts)[1].toInt();
    qDebug()<<anchor;

    char c='\0';

    if(user==this->username){ qDebug()<<"Questo messaggio non doveva arrivare a me!!!"; return; } //non lo considero
    //user="lalla";
    emit spostaCursSignal(pos,anchor,c,user);
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
            i++;
        } else {
            nome.clear();
        }
    } while (nome != "");

    QMessageBox Messaggio;
    Messaggio.information(nullptr,"Errore inserimento utenti", "Utente non esiste: " +str );
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
    qDebug()<<"Disconnesso dal server" << endl;
    this->connectedDB=false;
}
void Client::handleStile(QString& stile,QString& param){
    QMap<QString, QString> comando;
    comando.insert(CMD, STILE);
    comando.insert(DOCID, this->docID);
    comando.insert(UNAME, this->username);
    comando.insert(STILE, stile);
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

    qDebug() << "sono in " << Q_FUNC_INFO << "; logged: " << logged << "; username: " << this->username;
    if ( this->logged == true || !this->username.isEmpty()) {
        comando.insert(CMD, LOGOUT);
        comando.insert(UNAME, this->username);
        this->sendMsg(comando);
    }

    this->username = username;
    this->tempUser = username;
    this->logged = true;

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

void Client::handleMyCursorChange(int& pos,int& anchor)
{
    //qDebug() << "sono in " << Q_FUNC_INFO;
    if(socket->state() != QTcpSocket::ConnectedState || !logged || !connectedDB){ return; }

    QMap<QString, QString> comando;

    comando.insert(CMD,CRS);
    comando.insert(UNAME,this->username);
    comando.insert(DOCID, this->docID);
    QString indici=QString::number(pos)+";"+QString::number(anchor);
    comando.insert(IDX, indici );

    //    qDebug()<<"------------------------------------------";
    //qDebug()<<comando;
    this->sendMsg(comando);
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

QByteArray Client::serialize(const QTextCharFormat &format)
{
    QByteArray s;
    QDataStream out(&s,QIODevice::ReadWrite);
    out <<format;
    // qDebug() << s;
    return s;
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
    while ( cmd.contains(UNAME + QString::number(i))){
        list.append(cmd.value( UNAME + QString::number(i)) );
        i++;
    }

    qDebug() << list;
    emit s_upCursor(list);
    return true;
}

bool Client::remoteInsert(QChar c, QTextCharFormat format, QVector<qint32> index)
{
    QMap<QString,QString> cmd;
    cmd.insert(CMD, REM_IN);
    cmd.insert(CAR, c);
    QString indici;
    for (qint32 val : index){
        indici.append(QString::number(val) + ";");
    }
    cmd.insert(IDX, indici);
    qDebug()<<"laaaaaaaa";

    qDebug()<<QString(this->serialize(format).toHex());
    cmd.insert(FORMAT, QString(this->serialize(format).toHex()));
    cmd.insert(UNAME, username);
    cmd.insert(DOCID, docID);

    qDebug() << cmd;
    this->sendMsg(cmd);

    return true;
}

bool Client::remoteInsert(Symbol sym)
{
    return remoteInsert( sym.getCar(),
                         sym.getFormat(),
                         sym.getIndici());
}

bool Client::remoteDelete(QChar c, QVector<qint32> index)
{
    QMap<QString,QString> cmd;
    cmd.insert(CMD, REM_DEL);
    cmd.insert(CAR, c);
    QString indici;
    for (qint32 val : index){
        indici.append(QString::number(val) + ";");
    }
    cmd.insert(IDX, indici);
    cmd.insert(UNAME,username);
    cmd.insert(DOCID,docID);

    return this->sendMsg(cmd);
}

bool Client::remoteDelete(Symbol s)
{
    return remoteDelete( s.getCar(), s.getIndici());
}

void Client::readyRead(){
    QByteArray out;
    QByteArray tmp;
    uint dim;
    char v[4096] = {};
    qint64 dimRead = 0;

    out.reserve(4096);
    tmp.reserve(16);

    if (socket->bytesAvailable() > 0){
        dimRead = socket->read(v, 4096);
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
                //socket->waitForReadyRead(100);      // finisco di leggere il resto del messaggio
                dimRead = socket->read(v, 4096);
                if ( dimRead < 0){
                    qDebug() << "errore in socket::read()";
                }
                this->command.append( v, static_cast<int>(dimRead) );
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

