#include "s_thread.h"

s_thread::s_thread(int ID, QObject *parent) : QThread(parent)
{
    this->sockID = ID;
    this->buffer.reserve(4096);
    this->command.reserve(4096);
}


void s_thread::run()
{
    Logger::getLog().write("Nuovo thread in esecuzione, ID = "+ QString::number(this->sockID) );
    qDebug() << "Thread running, ID: " << QString::number(this->sockID);
    this->socket = new QTcpSocket();
    socket->setSocketDescriptor(sockID);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::ConnectionType::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::ConnectionType::DirectConnection);
    qDebug() << "Client connesso";
    this->user = new utente();
    user->setSocket(socket);
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
    if (this->user->isConnected()){
        this->disconnectDB();   }
/*    delete this->conn;
    delete this->user;
    delete this->socket;*/
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
    qDebug() << "Leggo XML";
    QMap<QString, QString> command;

    qDebug() << qb;
    QXmlStreamReader stream(qb);

    while (!stream.atEnd() && !stream.hasError() ){
        QXmlStreamReader::TokenType token = stream.readNext();
        // leggo start document
        if (token == QXmlStreamReader::StartDocument){
            // qDebug() << "start doc: " << QString(token) << " - " << stream.readElementText();
        }
        token = stream.readNext();
        // leggo elemento con nome del comando
        if (token == QXmlStreamReader::StartElement){
            qDebug() << "comando: " << stream.name();
            QString cmd = stream.name().toString();
            command.insert(CMD, cmd);
        }
        token = stream.readNext();
        // leggo elemnto variabile
        while ( token == QXmlStreamReader::StartElement ){
            QString name = stream.name().toString(), text = stream.readElementText();
            command.insert(name, text);
            qDebug() << "start elem: " << name << " val: " << text;
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
void s_thread::dispatchCmd(QMap<QString, QString> cmd){
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
        // this->addFileDB(cmd);
    } else if (comando.value() == BROWS) {
        this->browsFile(cmd);
    } else if (comando.value() == OPEN_FILE) {
        this->openFile(cmd);
    } else if (comando.value() == REM_IN || comando.value() == REM_DEL || comando.value() == CRS) {
    this->sendMsg(cmd);
    }
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
    if (socket->isOpen() && socket->isWritable()){
        if (this->socket->write(data, data.size()) == -1){
            // errore
            qDebug() << "errore scrittura verso client";
        }
    }
    return false;
}

/**
 * @brief s_thread::clientMEX
 * @param comando
 * @return
 * override per mandare messaggi al client
 */
bool s_thread::clientMsg(QMap<QString, QString> comando){
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
    qDebug() << QString(ba);

    return clientMsg(ba);
}


/*********************************************************************************************************
 ************************ metodi di accesso al db ********************************************************
 *********************************************************************************************************/

void s_thread::connectDB()
{
    this->conn = new db(this->sockID);
    if (!this->conn->isOpen()){
        QString logStr;
        QMap<QString,QString> risp;
        if (conn->conn() == false){
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

void s_thread::loginDB(QMap<QString, QString> &comando){
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return;   }
    if (!this->conn->isOpen()){
        qDebug() <<"connessione non aperta: " << conn->isOpen();
        this->connectDB();
    }
    this->user->prepareUtente(comando);
    utente tmp;
    tmp.prepareUtente(comando);

    QMap<QString,QString> risp;
    QString logStr;
    if (this->conn->userLogin(*user) ){
        logStr = QString::number(this->sockID) + " loggato a db con utente: " + user->getUsername();
        this->user->prepareUtente(comando, true);
        risp.insert(CMD, OK);
        risp.insert(MEX,LOGIN_OK);
        risp.insert(NICK, user->getNick());
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX,LOGIN_ERR);
        logStr = QString::number(this->sockID) + " non loggato a db con utente: ";
        this->user = nullptr;
    }
    Logger::getLog().write(logStr);
    clientMsg(risp);
}

void s_thread::logoffDB(QMap<QString, QString> &comando)
{
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return;   }
    if (!this->conn->isOpen()){
        qDebug() <<"connessione non aperta: " << conn->isOpen();
        this->connectDB();
    }
    if (this->user == nullptr)
        return;
    QMap<QString,QString> risp;

    QString logStr;
    if (this->conn->userLogOut(*user) ){
        risp.insert(CMD, OK);
        risp.insert(MEX, LOGOUT_OK);
        logStr = QString::number(this->sockID) + " log out a db con utente: " + user->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX, LOGOUT_ERR);
        logStr = QString::number(this->sockID) + " non log out a db con utente: " + user->getUsername();
    }
    Logger::getLog().write(logStr);
    clientMsg(risp);
}

void s_thread::registerDB(QMap<QString, QString> &comando){
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return;   }
    if (!this->conn->isOpen()){
        qDebug() <<"connessione non aperta: " << conn->isOpen();
        this->connectDB();
    }

    if (this->user->isConnected()){
        QMap<QString, QString> tmpCmd;
        tmpCmd.insert(CMD, LOGOUT);
        tmpCmd.insert(UNAME, user->getUsername());
        tmpCmd.insert(PASS, user->getPass());
        logoffDB(tmpCmd);
    }
    this->user->prepareUtente(comando);
    QMap<QString,QString> risp;

    QString logStr;
    if (this->conn->userReg(*user) ){
        risp.insert(CMD, OK);
        risp.insert(MEX, REG_OK);
        risp.insert(NICK, user->getNick());
        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + user->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX, REG_ERR);
        logStr = QString::number(this->sockID) + " non viene registrato a db con utente: " + user->getUsername();
        this->user->clear();
    }
    Logger::getLog().write(logStr);
    clientMsg(risp);
}

/**
 * @brief s_thread::disconnectDB
 * @param comando
 */
void s_thread::disconnectDB()
{
    if (this->conn->isOpen()){
        QString logStr;
        if (this->conn->disconn(*user) ){
            logStr = QString::number(this->sockID) + " disconnesso a db con utente: " + user->getUsername();
        } else {
            logStr = QString::number(this->sockID) + " errore nella disconessione a db con utente: " + user->getUsername();
        }
        Logger::getLog().write(logStr);
    }
}

/*
void s_thread::addFileDB(QMap<QString, QString> &comando)
{
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return;   }
    if (!this->conn->isOpen()){
        qDebug() <<"connessione non aperta: " << conn->isOpen();
        this->connectDB();
    }
    QMap<QString,QString> risp;
    QString logStr;
    if (this->conn->addFile(*user, "asd") ){

        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + user->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX,"logout fallito");
        logStr = QString::number(this->sockID) + " non viene registrato a db con utente: " + user->getUsername();
    }
    Logger::getLog().write(logStr);
    Network &net = Network::getNetwork();
    net.createEditor(comando.value(DOCID), comando.value(FNAME), *user);
}*/

void s_thread::browsFile(QMap<QString, QString> &comando)
{
    QList<QString> list = {CMD};
    if (!verifyCMD(comando, list)){ return;   }
    if (user == nullptr) return;
    if (!this->conn->isOpen()){
        qDebug() <<"connessione non aperta: " << conn->isOpen();
        this->connectDB();
    }
    QString logStr;
    QMap<QString, QString> map = this->conn->browsFile(*user);

        clientMsg("Browsing effettuato");
        logStr = QString::number(this->sockID) + "Browsing con utente: " + user->getUsername();

    Logger::getLog().write(logStr);
    this->clientMsg(map);
}

/**
 * @brief s_thread::openFile
 * @param comando
 * verifica sia presente l'editor in network
 */
void s_thread::openFile(QMap<QString, QString> &comando)
{
    QList<QString> list = {CMD, DOCID}; // DOCID, FNAME, UNAME
    if (!verifyCMD(comando, list)){ return;   }
    if (user == nullptr) return;
    if (!this->conn->isOpen()){
        qDebug() <<"connessione non aperta: " << conn->isOpen();
        this->connectDB();
    }
    QMap<QString,QString> risp;

    // accedo a db e verifico file apribile dall'utente
    QString logStr;
    if (this->conn->canUserOpenFile(*user, comando.value(DOCID)) ){
        //clientMEX("File apribile dallì'utente");
        risp.insert(CMD, OK);
        risp.insert(MEX, FILE_OK);
        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + user->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX, FILE_ERR);
        logStr = QString::number(this->sockID) + " File non apribile dallì'utente: " + user->getUsername();
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
        net.createEditor(comando.value(DOCID), PATH + comando.value(FNAME), *user);
    } else {
        // aumenta contatore user attivi
        net.addRefToEditor(comando.value(DOCID), *this->user);
    }
    // send Map
    net.getEditor(comando.value(DOCID)).sendMap(this->user->getUsername());
}

void s_thread::loadFile(QMap<QString, QString> &comando)
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    if (!this->user->isConnected()){ return; }
    QList<QString> list = {CMD, FNAME}; // DOCID, FNAME, UNAME
    if (!verifyCMD(comando, list)){ return;   }

    QString nomeF = comando.value(FNAME);
    QString dimF = comando.value(DIMTOT);
    int dim = dimF.toInt();
    char v[4096] = {};

    // aggiungo file al db
    this->conn->addFile(*this->user, nomeF);
    QString docId = this->conn->getDocId(nomeF, user->getUsername());

    if (docId == ""){
        qDebug() << "errore getDocId";
        return;
    }


    // aggiungo editor se file presente nello store ma non aperto da altri utenti
    Network &net = Network::getNetwork();
   /* if (!net.filePresent(comando.value(DOCID))){
        // aggiungi file
        net.createEditor(comando.value(DOCID), PATH + comando.value(FNAME), *user);
    } else {
        // aumenta contatore user attivi
        net.addRefToEditor(comando.value(DOCID), *this->user);
    }
*/
    // leggo file inviato e lo passo a editor
    QFile *file = new QFile();
    file->setFileName(PATH+docId);       // da mettere al posto giusto
    file->open(QIODevice::Append | QIODevice::Text);

    qint64 dimV = file->write(this->buffer, dim);   // lettura da buffer
    dim -= dimV;

    // lettura rimanenze da socket
    while (dim > 0){
        socket->waitForReadyRead();
        if (socket->bytesAvailable() > 0){
            dimV = socket->read(v, 4096);
            if (dimV < 0){
                qDebug() << "errore in socket::read()";
                return;
            }
            dimV = file->write( v );
            if (dimV < 0){
                qDebug() << "errore in file::write()";
                return;
            }
            dim = dim - static_cast<int>(dimV);
        }
    }
    file->close();

    net.createEditor(docId,nomeF,*user);


    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::ConnectionType::DirectConnection);
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
    if (!Msg.prepareMsg(comando, this->user->getUsername())){
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

/**
 * @brief s_thread::readyRead
 * metodo chiamato se il socket ha un buffer da leggere, quando riconosce un intero lo usa come dimensione del
 * file xml da leggere e chiama il metodo per la lettura.
 *
 * |buffer| usato come contenitore sporco,
 * |out|
 *//*
void s_thread::readyRead()
{
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
*/


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
