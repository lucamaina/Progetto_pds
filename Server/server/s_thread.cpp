#include "s_thread.h"

s_thread::s_thread(int ID, QObject *parent) : QThread(parent)
{
    this->sockID = ID;
}


void s_thread::run()
{
    Logger::getLog().write("Nuovo thread in esecuzione, ID = "+ QString::number(this->sockID) );
    qDebug() << "Thread running, ID: " << QString::number(this->sockID);

    sharedSocket = QSharedPointer<MySocket>( new MySocket(sockID));

    connect(sharedSocket.get(), &MySocket::s_disconnected, this, &s_thread::disconnected, Qt::DirectConnection);
    connect(sharedSocket.get(), SIGNAL( s_dispatchCmd(QMap<QString, QString>&) ), this, SLOT( dispatchCmd(QMap<QString, QString>&) ), Qt::DirectConnection);
    connect(sharedSocket.get(), SIGNAL( s_dispatchCmd(Comando&) ), this, SLOT( dispatchCmd(Comando&) ), Qt::DirectConnection);
    qDebug() << "Client connesso nuovo socket";

    this->uniqueUser = std::make_unique<utente>( utente() );

    this->clientNotifyDB();

    this->exec();
}

/**
 * @brief s_thread::disconnected
 * metodo chiamato quando viene disconesso il socket dal client.
 */
void s_thread::disconnected()
{
    qDebug() << endl
             << "(" << Q_FUNC_INFO << ") Client disconnesso: "
             << QString::number(this->sockID)
             << endl;
    this->sharedSocket->deleteLater();
    emit sigDeleteThread(*this);
}

void s_thread::exitThread()
{
    Network::getNetwork().remRefToEditor( this->docID,
                                          this->uniqueUser->getUsername() );
    if (this->uniqueUser.get()->isConnected()){
        this->logOffDB();
        this->disconnectDB();
    }

    qDebug() << Q_FUNC_INFO << "s_thread disconnesso : " << this->sockID;
    this->quit();
    this->wait();
}

/**
 * @brief s_thread::dispatchCmd
 * @param cmd
 * legge nella mappa il nome del comando e richiama le funzioni corrette
 */
void s_thread::dispatchCmd(QMap<QString, QString> &cmd){
    auto comando = cmd.find(CMD);
    if (comando.value() == CONN) {
        this->clientNotifyDB();
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
    } else if (comando.value() == BROWSE) {
        this->browsFile(cmd);
    } else if (comando.value() == OPEN_FILE) {
        this->openFile(cmd);
    } else if (comando.value() == REM_IN || comando.value() == REM_DEL || comando.value() == CRS) {
        this->sendMsgToNetwork(cmd);
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

int s_thread::getSockID() const
{
    return sockID;
}


/**
 * manda messaggio al client
 */
bool s_thread::clientMsg(QByteArray data){
    this->sharedSocket->write(data);
    return false;
}

bool s_thread::clientMsg(QMap<QString, QString> comando){
    this->sharedSocket->write(comando);
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

void s_thread::connectDB(std::shared_ptr<db> sourceSharedDB)
{
    QString logStr;
    QMap<QString,QString> risp;
    this->sharedDB = sourceSharedDB;
}

/**
 * @brief s_thread::loginDB
 * @param comando
 * utente viene segnato come loggato in tabella del db
 */
void s_thread::loginDB(QMap<QString, QString> &comando){
    QList<QString> list = {CMD};
    if (!this->comandCanStart(comando, list)){
        return;
    }
    // preparo utente temporaneo per verificare le informazioni
    utente tmp;
    tmp.prepareUtente(comando);

    QMap<QString,QString> risp;
    QString logStr;
    if (this->sharedDB->userLogin(tmp) ){
        // utente presente nel db e segnato come connesso
        logStr = QString::number(this->sockID) + " loggato a db con utente: " + uniqueUser->getUsername();
        this->uniqueUser.get()->prepareUtente(comando, true);
        risp.insert(CMD, OK);
        risp.insert(MEX,LOGIN_OK);
        risp.insert(NICK, uniqueUser->getNick());
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
    if (!this->comandCanStart(comando, list)){
        return;
    }
    if (this->uniqueUser->getUsername().isEmpty())
        return;
    QMap<QString,QString> risp;

    QString logStr;
    if (this->sharedDB->userLogOut(*uniqueUser) ){
        risp.insert(CMD, OK);
        risp.insert(MEX, LOGOUT_OK);
        logStr = QString::number(this->sockID) + " log out a db con utente: " + uniqueUser->getUsername();

        Network::getNetwork().remRefToEditor(this->docID, this->uniqueUser->getUsername());

    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX, LOGOUT_ERR);
        logStr = QString::number(this->sockID) + " non log out a db con utente: " + uniqueUser->getUsername();
    }
    Logger::getLog().write(logStr);
    clientMsg(risp);
}

void s_thread::logOffDB()
{
    QString logStr;
    if (uniqueUser->isConnected()){
        if (this->sharedDB->userLogOut(*uniqueUser) ){
            logStr = QString::number(this->sockID) + " log out a db con utente: " + uniqueUser->getUsername();
        } else {
            logStr = QString::number(this->sockID) + " non log out a db con utente: " + uniqueUser->getUsername();
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

    if (!this->comandCanStart(comando, list)){
        return;
    }

    if (this->uniqueUser->isConnected()){
        QMap<QString, QString> tmpCmd;
        tmpCmd.insert(CMD, LOGOUT);
        tmpCmd.insert(UNAME, uniqueUser->getUsername());
        tmpCmd.insert(PASS, uniqueUser->getPass());
        logoffDB(tmpCmd);
    }
    // preparo utente temporaneo per verificare le informazioni
    utente tmp;
    tmp.prepareUtente(comando);

    QString logStr;
    if (this->sharedDB->userReg(tmp) ){
        // nuovo utente registrato
        this->uniqueUser.get()->prepareUtente(comando);
        risp.insert(CMD, OK);
        risp.insert(MEX, REG_OK);
        risp.insert(NICK, uniqueUser->getNick());
        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + uniqueUser->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX, REG_ERR);
        logStr = QString::number(this->sockID) + " non viene registrato a db con utente: " + uniqueUser->getUsername();
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

    if (!this->comandCanStart(comando, list)){
        return;
    }

    if (!this->uniqueUser->isConnected())
        return;

    QString nomeFile = comando.take(FNAME);
    int errorCode;
    if (this->sharedDB->addFile(*this->uniqueUser, nomeFile, errorCode)){
        // file inserito
        risp.insert(CMD, OK);
        risp.insert(MEX, ADD_FILE_OK);
    } else if (errorCode == 0){
        // nome presente
        risp.insert(CMD, ERR);
        risp.insert(MEX, ADD_FILE_ERR1);
    } else {
        // errore inserimento
        risp.insert(CMD, ERR);
        risp.insert(MEX, ADD_FILE_ERR2);
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
    if (this->sharedDB->isOpen()){
        QString logStr;
        if (this->sharedDB->disconn(*uniqueUser) ){
            logStr = QString::number(this->sockID) + " disconnesso a db con utente: " + uniqueUser->getUsername();
        } else {
            logStr = QString::number(this->sockID) + " errore nella disconessione a db con utente: " + uniqueUser->getUsername();
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
    if (!this->comandCanStart(comando, list)){
        return;
    }
    if (!uniqueUser->isConnected()){
        return;
    }

    QString logStr;
    QMap<QString, QString> map = this->sharedDB->browsFile(*uniqueUser);
    logStr = QString::number(this->sockID) + "Browsing con utente: " + uniqueUser->getUsername();

    Logger::getLog().write(logStr);
    qDebug() << map.toStdMap();
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
    if (!this->comandCanStart(comando, list)){
        return;
    }
    if (!uniqueUser->isConnected()){
        return;
    }
    if(!docID.isEmpty())
        Network::getNetwork().remRefToEditor(this->docID, this->uniqueUser->getUsername());
    this->docID = comando.value(DOCID);

    QMap<QString,QString> risp;


    // accedo a db e verifico file apribile dall'utente
    QString logStr;
    if (this->sharedDB->canUserOpenFile(*uniqueUser, comando.value(DOCID)) ){
        //clientMEX("File apribile dallì'utente");
        risp.insert(CMD, OK);
        risp.insert(MEX, FILE_OK);
        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + uniqueUser->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX, FILE_ERR);
        logStr = QString::number(this->sockID) + " File non apribile dall'utente: " + uniqueUser->getUsername();
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
                              comando.value(FNAME),
                              *uniqueUser,
                              this->sharedSocket );
        } catch (std::logic_error le) {
            QString s = "Eccezione in apertura di editor" + QString(le.what());
            Logger::getLog().write(s);
            qDebug()  << "eccezione in " << Q_FUNC_INFO;
            risp.insert(CMD, ERR);
            risp.insert(MEX, FILE_ERR);
            clientMsg(risp);
            return;
        }

    } else {
        // aumenta contatore user attivi
        net.addRefToEditor( comando.value(DOCID),
                            *this->uniqueUser,
                            this->sharedSocket );
    }
    QString docid = comando.value(DOCID);
    // send Map
    QByteArray ba = net.getEditor(docid).getSymMap();

    this->sendBody(ba);
}

/**
 * @brief s_thread::exitClientFromFile
 * @param comando
 * chiamato se utente esce dal file, si aggiorna il contatore degli utenti attivi
 */
void s_thread::exitClientFromFile(QMap<QString, QString> &comando)
{
    if (!this->comandCanStart(comando, QList<QString>({CMD, DOCID}))){
        return;
    }
    if (!uniqueUser->isConnected()){
        return;
    }
    QString docid = comando.value(DOCID);
    Network::getNetwork()
            .getEditor(docid)
            .removeUser( uniqueUser->getUsername() );
    this->docID.clear();
}


void s_thread::getUsers(QMap<QString, QString> &comando)
{
    if (!this->uniqueUser->isConnected()){ return; }
    QList<QString> list = {CMD, DOCID }; // DOCID
    if (!this->comandCanStart(comando, list)){
        return;
    }
    QString docId = comando.value(DOCID);
    QString logStr;
    QList<QString> lista = this->sharedDB->getUsers(docId);

//    clientMsg("getUsers effettuato");
    logStr = QString::number(this->sockID) + "getUsers con utente: " + uniqueUser->getUsername();
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
    if (!this->uniqueUser->isConnected()){ return; }
    QList<QString> list = {CMD, DOCID }; // DOCID
    if (!this->comandCanStart(comando, list)){
        return;
    }
    QString docId = comando.value(DOCID);
    QString logStr;
    QString rispOk, rispErr;
    QStringList lista = toQStringList(comando);

    QMap<QString, QString> risp;

    int i = 1;
    for (QString utente : lista){
        if ( this->sharedDB->addUser(*this->uniqueUser, docId, utente) == false ){
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
    if (!this->uniqueUser->isConnected()){ return; }
    QList<QString> list = {CMD, DOCID }; // DOCID
    if (!this->comandCanStart(comando, list)){
        return;
    }
    QString docId = comando.value(DOCID);
    QString logStr;
    QString rispOk, rispErr;
    QStringList lista = toQStringList(comando);
    QMap<QString, QString> risp;

    for (QString utente : lista){
        if ( this->sharedDB->remUser(*this->uniqueUser, docId, utente) == true ){
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
 * invia il messaggio alla coda del network
 */
void s_thread::sendMsgToNetwork(QMap<QString, QString> comando)
{
    Message Msg = Message();
    if (!Msg.prepareMsg(comando, this->uniqueUser->getUsername())){
        return;
    }
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

void s_thread::clientNotifyDB()
{
    QString logStr;
    QMap<QString,QString> risp;

    risp.insert(CMD, OK);
    risp.insert(MEX,CONN_OK);
    clientMsg(risp);
}

bool s_thread::comandCanStart(QMap<QString, QString> &cmd, const QList<QString> &list)
{
    return (this->sharedDB->isOpen() && this->verifyCMD(cmd, list));
}
