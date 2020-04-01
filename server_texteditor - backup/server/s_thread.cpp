#include "s_thread.h"

s_thread::s_thread(int ID, QObject *parent) : QThread(parent)
{
    this->sockID = ID;
    this->buffer.reserve(4096*10);
    this->command.reserve(4096);
}


void s_thread::run()
{
    Logger::getLog().write("Nuovo thread in esecuzione, ID = "+ QString::number(this->sockID) );
    qDebug() << "Thread running, ID: " << QString::number(this->sockID);
/*
    this->socket = new QTcpSocket();
    socket->setSocketDescriptor(sockID);
    connect(this->socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::ConnectionType::DirectConnection);
    connect(this->socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::ConnectionType::DirectConnection);
    qDebug() << "Client connesso vecchio socket";
*/

    sp_socket = QSharedPointer<MySocket>( new MySocket(sockID));

    connect(sp_socket.get(), &MySocket::s_disconnected, this, &s_thread::disconnected, Qt::DirectConnection);
    connect(sp_socket.get(), SIGNAL( s_dispatchCmd(QMap<QString, QString>&) ), this, SLOT( dispatchCmd(QMap<QString, QString>&) ), Qt::DirectConnection);
    connect(sp_socket.get(), SIGNAL( s_dispatchCmd(Comando&) ), this, SLOT( dispatchCmd(Comando&) ), Qt::DirectConnection);
    qDebug() << "Client connesso nuovo socket";

    this->up_user = std::make_unique<utente>( utente() );

    this->connectDB();
}

/**
 * @brief s_thread::disconnected
 * metodo chiamato quando viene disconesso il socket dal client.
 */
void s_thread::disconnected()
{
    qDebug() << "(disconnected) client disconnesso "
             << QString::number(this->sockID);
    this->quit();
    emit deleteThreadSig(*this);
}

s_thread::~s_thread()
{
    qDebug() << "Distruttore s_thread.";
    if (this->up_user.get()->isConnected()){
        QMap<QString, QString> cmd;
        cmd.insert(CMD, LOGOUT);
        this->logoffDB(cmd);
        this->disconnectDB();
    }
}
\
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
    } else if (comando.value() == FBODY) {
        this->loadFile(cmd);
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
        net.createEditor( comando.value(DOCID),
                          PATH + comando.value(FNAME),
                          *up_user,
                          this->sp_socket );     // net.createEditor(comando.value(DOCID), PATH + comando.value(FNAME), *user);
    } else {
        // aumenta contatore user attivi
        net.addRefToEditor( comando.value(DOCID),
                            *this->up_user,
                            this->sp_socket );
    }

    // send Map
    QByteArray ba = net.getEditor(comando.value(DOCID)).getSymMap();

    this->sendBody(ba);
    qDebug() << ba;
}

/**
 * @brief s_thread::loadFile
 * @param comando
 * legge file da socket, deserializza la mappa CRDT e la carica.
 * L'editor deve già essere creato e presente in network !!!
 * l'utente deve essere l'unico collegato !!!
 */
void s_thread::loadFile(QMap<QString, QString> &comando)
{
    if (!this->up_user->isConnected()){ return; }
    QList<QString> list = {CMD, FNAME, DOCID }; // DOCID, FNAME, UNAME
    if (!verifyCMD(comando, list)){ return;   }

    QString nomeF = comando.value(FNAME);   // inutile
    QString dimF = comando.value(DIMTOT);
    int dim = dimF.toInt();

    // ricavo editor da network
    Network &net = Network::getNetwork();
    Editor &ed = net.getEditor(comando.value(DOCID));

    // controllo editor

    // leggo file da socket
    QByteArray ba;
    this->readBody(ba, dim);


    ed.deserialise(ba);

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


void s_thread::readBody(QByteArray &ba, int dim)
{
    int dimtmp = 0;
    int blk_dim = 4096;
    char v[4096] = {};
    ba.clear();
    ba.reserve(4096);

    ba.append(this->buffer);

    if (ba.size() >= dim){
        ba.chop(ba.size() - dim);
    } else {
        dimtmp = dim - ba.size();    // dimensione da leggere
        while (dimtmp > 0){
            if (dimtmp < 4096){
                blk_dim = dimtmp;
            }
            this->socket->waitForReadyRead(100);      // finisco di leggere il resto del messaggio
            if (this->socket->read(v, blk_dim) < 0){
                qDebug() << "errore in socket::read()";
            }

            ba.append(v);
            dimtmp = dim - ba.size();

        }
    }
    // togliere dopo debug
    qDebug() << "body: " << ba;
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
bool s_thread::verifyCMD(QMap<QString, QString> &cmd, QList<QString> &list)
{
    QString val;
    foreach (val, list){
        if (!cmd.contains(val)){    return false;   }
    }
    return true;
}


/*********************************************************************************************************
 ************************ public slots *******************************************************************
 *********************************************************************************************************/

/*
void s_thread::readyRead()
{
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
                socket->waitForReadyRead(100);      // finisco di leggere il resto del messaggio
                if (this->socket->read(v, i) < 0){
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
*/