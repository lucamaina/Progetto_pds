#include "s_thread.h"

s_thread::s_thread(int ID, QObject *parent) : QThread(parent)
{
    this->sockID = ID;
}


void s_thread::run()
{
    Logger::getLog().write("Nuovo thread in esecuzione, ID = "+ QString::number(this->sockID) );
    qDebug() << "Thread running, ID: " << QString::number(this->sockID);

    connect(this, &s_thread::finished, this, &s_thread::test, Qt::DirectConnection);

    sp_socket = QSharedPointer<MySocket>( new MySocket(sockID));

    connect(sp_socket.get(), &MySocket::s_disconnected, this, &s_thread::disconnected, Qt::DirectConnection);
    connect(sp_socket.get(), SIGNAL( s_dispatchCmd(QMap<QString, QString>&) ), this, SLOT( dispatchCmd(QMap<QString, QString>&) ), Qt::DirectConnection);
    connect(sp_socket.get(), SIGNAL( s_dispatchCmd(Comando&) ), this, SLOT( dispatchCmd(Comando&) ), Qt::DirectConnection);
    qDebug() << "Client connesso nuovo socket";

    this->up_user = std::make_unique<utente>( utente() );

    this->connectDB();

    this->exec();
}

/**
 * @brief s_thread::disconnected
 * metodo chiamato quando viene disconesso il socket dal client.
 */
void s_thread::disconnected()
{
    qDebug() << endl
             << "(disconnected) client disconnesso "
             << QString::number(this->sockID);
    this->sp_socket->deleteLater();
    emit deleteThreadSig(*this);
}

void s_thread::exitThread()
{
    qDebug() << "sono in " << Q_FUNC_INFO;

    // tolgo utente da editor
    Network::getNetwork().remRefToEditor( this->docID,
                                          this->up_user->getUsername() );
    if (this->up_user.get()->isConnected()){
        this->logOffDB();
        this->disconnectDB();
    }

    qDebug() << "s_thread disconnesso : " << this->sockID;
    this->quit();
    this->wait();
}

/*********************************************************************************************************
 ************************ metodi di connessione con client ***********************************************
 *********************************************************************************************************/

/**
 * @brief s_thread::leggiXML
 * @param dim
 * metodo di lettura del file XML. Gli elementi del file vengono mappati su una map di key, value
 * e quindi mandati a una funzione che esegue distinzione tra i comandi
 */
void s_thread::leggiXML(QByteArray qb)
{
    qDebug().noquote() << endl << endl << "-> Leggo XML from client: " << this->sockID << endl << qb;

    QMap<QString, QString> command;
    QXmlStreamReader stream(qb);

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

    // TODO in caso di messaggio non corretto riparti da stato corretto
    if (stream.hasError()){
        qDebug() << "err in lettura XML" << stream.errorString();
    } else {
        // qDebug() << "finito lettura xml no errori " << stream.errorString();
        this->dispatchCmd(command);
    }
}

/**
 * @brief s_thread::dispatchCmd
 * @param cmd
 * legge nella mappa il nome del comando e richiama le funzioni corrette
 */
void s_thread::dispatchCmd(QMap<QString, QString> &cmd){
    auto comando = cmd.find(CMD);
    if (comando.value() == CONN) {
        this->connectDB();
    } else if (comando.value() == LOGIN) {
        this->loginDB(cmd);
    } else if (comando.value() == LOGOUT) {
        this->logoffDB(cmd);
    } else if (comando.value() == REG) {
        this->registerDB(cmd);
    } else if (comando.value() == DISC) {
        this->disconnectDB();
    } else if (comando.value() == ADD_FILE) {
        this->addFileDB(cmd);
    } else if (comando.value() == BROWS) {
        this->browsFile(cmd);
    } else if (comando.value() == OPEN_FILE) {
        this->openFile(cmd);
    } else if (comando.value() == REM_IN || comando.value() == REM_DEL || comando.value() == CRS) {
        this->sendMsg(cmd);
    } else if (comando.value() == ULIST){
        this->getUsers(cmd);
    } else if (comando.value() == ADD_USER) {
        this->addUsersDB(cmd);
    } else if (comando.value() == REM_USER) {
        this->remUsersDB(cmd);
    } else if (comando.value() == EXFILE) {
        this->exitClientFromFile(cmd);
    }
}

void s_thread::dispatchCmd(Comando &cmd)
{
    QMap<QString, QString> cmdMap;
    (void) cmd;
}

/**
 * @brief s_thread::scriviXML
 * @param comando
 * @return
 * trasforma il comando da mappa in XML inviabile al client
 * legge coppia chiave - valore, la scrive in XML e la cancella dalla mappa
 */
bool s_thread::scriviXML(QMap<QString, QString> comando)
{
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return false;   }
    QByteArray ba;
    QXmlStreamWriter wr(&ba);
    wr.writeStartDocument();
    wr.writeTextElement(comando.value(CMD), "");
    comando.remove(CMD);

    while (!comando.empty()){
        QString elem, val;
        elem = comando.firstKey();
        val = comando.value(elem);
        qDebug() << elem << " : " << val;
        wr.writeTextElement(elem, val);
        comando.remove(elem);
    }
    wr.writeEndDocument();

    int dim = ba.size();
    QByteArray len;
    len = QByteArray::number(dim, 16);
    len.prepend(8 - len.size(), '0');

    ba.prepend(len);
    ba.prepend(INIT);
    qDebug() << QString(ba);
    return false;
}

/**
 * @brief s_thread::clientMEX
 * @param data
 * @return
 * manda messaggio al client
 */
bool s_thread::clientMsg(QByteArray data){
    this->sp_socket.get()->write(data);
    return false;
}

/**
 * @brief s_thread::clientMEX
 * @param comando
 * @return
 * override per mandare messaggi al client
 */
bool s_thread::clientMsg_(QMap<QString, QString> comando){ // vecchia versione
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return false;   }
    QByteArray ba;
    QXmlStreamWriter wr(&ba);
    wr.writeStartDocument();
    wr.writeStartElement(comando.value(CMD));
    comando.remove(CMD);
    foreach (QString key, comando.keys()) {
        wr.writeTextElement(key, comando.value(key));
    }
    wr.writeEndElement();
    wr.writeEndDocument();

    int dim = ba.size();
    QByteArray len;
    len = QByteArray::number(dim, 16);
    len.prepend(8 - len.size(), '0');

    ba.prepend(len);
    ba.prepend(INIT);
    qDebug().noquote() << endl << "<- To client: " << this->sockID << endl << QString(ba);

    return clientMsg(ba);
}

bool s_thread::clientMsg(QMap<QString, QString> comando){
    this->sp_socket->write(comando);
    return true;
}

/**
 * @brief s_thread::sendBody
 * @param ba
 * @return
 * invia messaggio con dimensione della mappa CRDT e in seguito il contenuto
 */
bool s_thread::sendBody(QByteArray &ba)
{
    int dim_body = ba.size();
    QMap<QString, QString> risp;
    risp.insert(CMD, FBODY);
    risp.insert(BODY, QString::number(dim_body));
    this->clientMsg(risp);
    return this->clientMsg(ba);
}


/*********************************************************************************************************
 ************************ metodi di accesso al db ********************************************************
 *********************************************************************************************************/

void s_thread::connectDB()
{
    this->up_conn = std::make_unique<db>( db(this->sockID) );
    if (!this->up_conn->isOpen()){
        QString logStr;
        QMap<QString,QString> risp;
        if (up_conn->conn() == false){
            // ritorna messaggio al client di fallimento
            risp.insert(CMD, ERR);
            risp.insert(MEX,CONN_ERR);
            logStr = QString::number(this->sockID) + "non connesso a db con utente: ";
        } else {
            // messaggio di successo al client
            risp.insert(CMD, OK);
            risp.insert(MEX,CONN_OK);

            logStr = QString::number(this->sockID) + " connesso a db con utente: ";
        }
        Logger::getLog().write(logStr);
        clientMsg(risp);
    }
}

/**
 * @brief s_thread::loginDB
 * @param comando
 * utente viene segnato come loggato in tabella del db
 */
void s_thread::loginDB(QMap<QString, QString> &comando){
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return;   }
    if (!this->up_conn->isOpen()){
        qDebug() <<"connessione non aperta: " << up_conn->isOpen();
        this->connectDB();
    }
    // preparo utente temporaneo per verificare le informazioni
    utente tmp;
    tmp.prepareUtente(comando);

    QMap<QString,QString> risp;
    QString logStr;
    if (this->up_conn->userLogin(tmp) ){
        // utente presente nel db e segnato come connesso
        logStr = QString::number(this->sockID) + " loggato a db con utente: " + up_user->getUsername();
        this->up_user.get()->prepareUtente(comando, true);
        risp.insert(CMD, OK);
        risp.insert(MEX,LOGIN_OK);
        risp.insert(NICK, up_user->getNick());
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX,LOGIN_ERR);
        logStr = QString::number(this->sockID) + " non loggato a db con utente: ";
    }
    Logger::getLog().write(logStr);
    clientMsg(risp);
}

void s_thread::logoffDB(QMap<QString, QString> &comando)
{
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return;   }
    if (!this->up_conn->isOpen()){
        qDebug() <<"connessione non aperta: " << up_conn->isOpen();
        this->connectDB();
    }
    if (this->up_user->getUsername().isEmpty())
        return;
    QMap<QString,QString> risp;

    QString logStr;
    if (this->up_conn->userLogOut(*up_user) ){
        risp.insert(CMD, OK);
        risp.insert(MEX, LOGOUT_OK);
        logStr = QString::number(this->sockID) + " log out a db con utente: " + up_user->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX, LOGOUT_ERR);
        logStr = QString::number(this->sockID) + " non log out a db con utente: " + up_user->getUsername();
    }
    Logger::getLog().write(logStr);
    clientMsg(risp);
}

void s_thread::logOffDB()
{
    QString logStr;
    if (up_user->isConnected()){
        if (this->up_conn->userLogOut(*up_user) ){
            logStr = QString::number(this->sockID) + " log out a db con utente: " + up_user->getUsername();
        } else {
            logStr = QString::number(this->sockID) + " non log out a db con utente: " + up_user->getUsername();
        }
        Logger::getLog().write(logStr);
    }
}

/**
 * @brief s_thread::registerDB
 * @param comando
 * crea nel db nuovo utente
 */
void s_thread::registerDB(QMap<QString, QString> &comando){
    QList<QString> list = {CMD};
    QMap<QString,QString> risp;

    if (!verifyCMD(comando, list)){ return;   }
    if (!this->up_conn->isOpen()){
        qDebug() <<"connessione non aperta: " << up_conn->isOpen();
        this->connectDB();
    }

    if (this->up_user->isConnected()){
        QMap<QString, QString> tmpCmd;
        tmpCmd.insert(CMD, LOGOUT);
        tmpCmd.insert(UNAME, up_user->getUsername());
        tmpCmd.insert(PASS, up_user->getPass());
        logoffDB(tmpCmd);
    }
    // preparo utente temporaneo per verificare le informazioni
    utente tmp;
    tmp.prepareUtente(comando);

    QString logStr;
    if (this->up_conn->userReg(tmp) ){
        // nuovo utente registrato
        this->up_user.get()->prepareUtente(comando);
        risp.insert(CMD, OK);
        risp.insert(MEX, REG_OK);
        risp.insert(NICK, up_user->getNick());
        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + up_user->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX, REG_ERR);
        logStr = QString::number(this->sockID) + " non viene registrato a db con utente: " + up_user->getUsername();
    }
    Logger::getLog().write(logStr);
    clientMsg(risp);
}


/**
 * @brief s_thread::addFileDB
 * @param comando
 * crea tupla in db per nuovo file creato da utente
 */
void s_thread::addFileDB(QMap<QString, QString> &comando)
{
    QList<QString> list = {CMD, FNAME};
    QMap<QString,QString> risp;

    if (!verifyCMD(comando, list)){ return;   }
    if (!this->up_conn->isOpen()){
        qDebug() <<"connessione non aperta: " << up_conn->isOpen();
        this->connectDB();
    }

    if (!this->up_user->isConnected())
        return;

    QString nomeFile = comando.take(FNAME);
    int errorCode;
    if (this->up_conn->addFile(*this->up_user, nomeFile, errorCode)){
        // file inserito
        risp.insert(CMD, OK);
        risp.insert(MEX, "file inserito");
    } else if (errorCode == 0){
        // nome presente
        risp.insert(CMD, ERR);
        risp.insert(MEX, "nome file già presente");
    } else {
        // errore inserimento
        risp.insert(CMD, ERR);
        risp.insert(MEX, "errore in inserimento");
    }
    // TODO stringa di log
    clientMsg(risp);
}

/**
 * @brief s_thread::disconnectDB
 * @param comando
 */
void s_thread::disconnectDB()
{
    if (this->up_conn->isOpen()){
        QString logStr;
        if (this->up_conn->disconn(*up_user) ){
            logStr = QString::number(this->sockID) + " disconnesso a db con utente: " + up_user->getUsername();
        } else {
            logStr = QString::number(this->sockID) + " errore nella disconessione a db con utente: " + up_user->getUsername();
        }
        Logger::getLog().write(logStr);
    }
}

/**
 * @brief s_thread::browsFile
 * @param comando
 * invia all'utente tutti i file che può aprire
 */
void s_thread::browsFile(QMap<QString, QString> &comando)
{
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return;   }
    if (up_user->isConnected() == false) return;
    if (!this->up_conn->isOpen()){
        qDebug() <<"connessione non aperta: " << up_conn->isOpen();
        this->connectDB();
    }
    QString logStr;
    QMap<QString, QString> map = this->up_conn->browsFile(*up_user);
        logStr = QString::number(this->sockID) + "Browsing con utente: " + up_user->getUsername();

    Logger::getLog().write(logStr);
    this->clientMsg(map);
}

/**
 * @brief s_thread::openFile
 * @param comando
 * verifica sia presente l'editor in network e lo aggiunge. Iniva contenuto del file all'utente che lo richiede
 */
void s_thread::openFile(QMap<QString, QString> &comando)
{
    QList<QString> list = {CMD, DOCID};
    if (!verifyCMD(comando, list)){ return;   }
    if (up_user == nullptr) return;
    if (!this->up_conn->isOpen()){
        qDebug() <<"connessione non aperta: " << up_conn->isOpen();
        this->connectDB();
    }

    if(!docID.isEmpty())
        Network::getNetwork().remRefToEditor(this->docID, this->up_user->getUsername());    //  ???
    this->docID = comando.value(DOCID);

    QMap<QString,QString> risp;


    // accedo a db e verifico file apribile dall'utente
    QString logStr;
    if (this->up_conn->canUserOpenFile(*up_user, comando.value(DOCID)) ){
        //clientMEX("File apribile dallì'utente");
        risp.insert(CMD, OK);
        risp.insert(MEX, FILE_OK);
        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + up_user->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX, FILE_ERR);
        logStr = QString::number(this->sockID) + " File non apribile dallì'utente: " + up_user->getUsername();
        Logger::getLog().write(logStr);
        clientMsg(risp);
        return;
    }
    Logger::getLog().write(logStr);
    clientMsg(risp);

    // aggiungo editor se file presente nello store ma non aperto da altri utenti
    Network &net = Network::getNetwork();
    if (!net.filePresent(comando.value(DOCID))){
        // aggiungi file
        try {
            net.createEditor( comando.value(DOCID),
                              PATH + comando.value(FNAME),
                              *up_user,
                              this->sp_socket );
        } catch (...) {
            Logger::getLog().write("Eccezione in apertura di aditor");
        }

    } else {
        // aumenta contatore user attivi
        net.addRefToEditor( comando.value(DOCID),
                            *this->up_user,
                            this->sp_socket );
    }
    QString docid = comando.value(DOCID);
    // send Map
    QByteArray ba = net.getEditor(docid).getSymMap();

    this->sendBody(ba);
    qDebug() << ba;
}

/**
 * @brief s_thread::exitClientFromFile
 * @param comando
 * chiamato se utente esce dal file, si aggiorna il contatore degli utenti attivi
 */
void s_thread::exitClientFromFile(QMap<QString, QString> &comando)
{
    if (!verifyCMD( comando,
                    QList<QString>({CMD, DOCID})) )
    {   return;   }
    if (up_user->isConnected() == false) return;

    QString docid = comando.value(DOCID);
    Network::getNetwork()
            .getEditor(docid)
            .removeUser( up_user->getUsername() );
    this->docID.clear();
}


void s_thread::getUsers(QMap<QString, QString> &comando)
{
    if (!this->up_user->isConnected()){ return; }
    QList<QString> list = {CMD, DOCID }; // DOCID
    if (!verifyCMD(comando, list)){ return;   }
    if (up_user->isConnected() == false) return;
    if (!this->up_conn->isOpen()){
        qDebug() <<"connessione non aperta: " << up_conn->isOpen();
        this->connectDB();
    }
    QString docId = comando.value(DOCID);
    QString logStr;
    QList<QString> lista = this->up_conn->getUsers(docId);

    clientMsg("getUsers effettuato");
    logStr = QString::number(this->sockID) + "getUsers con utente: " + up_user->getUsername();
    Logger::getLog().write(logStr);

    QMap<QString, QString> map;
    map.insert(CMD, ULIST);
    map.insert(DOCID, docId);
    int i = 1;
    for (QString user : lista) {
        map.insert(UNAME + QString::number(i), user);
        i++;
    }
    this->clientMsg(map);

}

// verificare messaggio di risposta
void s_thread::addUsersDB(QMap<QString, QString> &comando)
{
    qDebug() << "sono in s_thread::addUsersDB";
    if (!this->up_user->isConnected()){ return; }
    QList<QString> list = {CMD, DOCID }; // DOCID
    if (!verifyCMD(comando, list)){ return;   }
    if (up_user->isConnected() == false) return;
    if (!this->up_conn->isOpen()){
        qDebug() <<"connessione non aperta: " << up_conn->isOpen();
        this->connectDB();
    }
    QString docId = comando.value(DOCID);
    QString logStr;
    QString rispOk, rispErr;
    QStringList lista = toQStringList(comando);

    QMap<QString, QString> risp;

    int i = 1;
    for (QString utente : lista){
        if ( this->up_conn->addUser(*this->up_user, docId, utente) == false ){
            risp.insert(UNAME+QString::number(i++), utente);
        }
    }

    if (!risp.isEmpty()){
        risp.insert(CMD, ERR);
        risp.insert(MEX, ADD_USER_ERR);
        this->clientMsg(risp);
    }


    // ritorno list autenti aggiornata
    comando.clear();
    comando.insert(CMD, ULIST);
    comando.insert(DOCID, docId);
    this->getUsers(comando);

}

// verificare messaggio di risposta
void s_thread::remUsersDB(QMap<QString, QString> &comando)
{
    qDebug() << "sono in s_thread::remUsersDB = " << Q_FUNC_INFO;
    if (!this->up_user->isConnected()){ return; }
    QList<QString> list = {CMD, DOCID }; // DOCID
    if (!verifyCMD(comando, list)){ return;   }
    if (up_user->isConnected() == false) return;
    if (!this->up_conn->isOpen()){
        qDebug() <<"connessione non aperta: " << up_conn->isOpen();
        this->connectDB();
    }
    QString docId = comando.value(DOCID);
    QString logStr;
    QString rispOk, rispErr;
    QStringList lista = toQStringList(comando);
    QMap<QString, QString> risp;

    for (QString utente : lista){
        if ( this->up_conn->remUser(*this->up_user, docId, utente) == true ){
            rispOk.append(utente+" ; ");
            Network::getNetwork().getEditor(docId).removeUser(utente);
            // mostra agli utenti

        } else {
            rispErr.append(utente+ " ; ");
        }
    }

    if (!risp.isEmpty()){
        risp.insert(CMD, ERR);
        risp.insert(MEX, REM_USER_ERR);
        this->clientMsg(risp);
    }

    // ritorno list autenti aggiornata
    comando.clear();
    comando.insert(CMD, ULIST);
    comando.insert(DOCID, docId);
    this->getUsers(comando);
}


QStringList s_thread::toQStringList(QMap<QString, QString> cmd)
{
    int i = 1;
    QStringList lista;
    for (QString key : cmd.keys()){
        if (key == UNAME + QString::number(i)){
            lista.append(cmd.value(key));
            i++;
        }
    }
    return lista;
}
/*********************************************************************************************************
 ************************ metodi di accesso a Network ********************************************************
 *********************************************************************************************************/

/**
 * @brief s_thread::sendMEX
 * @param comando
 * invia il messaggio alla coda del network
 */
void s_thread::sendMsg(QMap<QString, QString> comando)
{
    Message Msg = Message();
    if (!Msg.prepareMsg(comando, this->up_user->getUsername())){
        return; }
    Network &net = Network::getNetwork();
    net.push(Msg);
}


/**
 * @brief s_thread::verifyCMD
 * @param cmd
 * @param list
 * @return true se tutti i campi di list sono chiavi di cmd
 * verifica presenza dei campi nella mappa dei comandi
 */
bool s_thread::verifyCMD(QMap<QString, QString> &cmd, const QList<QString> &list)
{
    QString val;
    foreach (val, list){
        if (!cmd.contains(val)){    return false;   }
    }
    return true;
}
