#include "s_thread.h"

s_thread::s_thread(int ID, QObject *parent) : QThread(parent)
{
    this->sockID = ID;
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
}


s_thread::~s_thread()
{
    qDebug() << "Distruttore s_thread";
    this->disconnectDB();
    delete this->conn;
    delete this->user;
    this->quit();
    this->requestInterruption();
    this->wait();
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
            qDebug() << "start doc: " << QString(token) << " - " << stream.readElementText();
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
    } else if (comando.value() == LOGOFF) {
        this->logoffDB(cmd);
    } else if (comando.value() == REG) {
        this->registerDB(cmd);
    } else if (comando.value() == DISC) {
        this->disconnectDB();
    } else if (comando.value() == FILES) {
        //
    } else if (comando.value() == ADD_FILE) {
        // this->addFileDB(cmd);
    } else if (comando.value() == BROWS) {
        this->browsFile(cmd);
    } else if (comando.value() == OPEN_FILE) {
        this->openFile(cmd);
    } else if (comando.value() == REM_IN || comando.value() == REM_DEL) {
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
            risp.insert(MEX,"impossibile connettersi al db");
            logStr = QString::number(this->sockID) + "non connesso a db con utente: ";
        } else {
            // messaggio di successo al client
            risp.insert(CMD, OK);
            risp.insert(MEX,"connesso al db");

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
        risp.insert(CMD, OK);
        risp.insert(MEX,"login effettuato");
        logStr = QString::number(this->sockID) + " loggato a db con utente: " + user->getUsername();
        this->user = &tmp;
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX,"login fallito, user o password errati");
        logStr = QString::number(this->sockID) + " non loggato a db con utente: " + user->getUsername();
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
        risp.insert(MEX,"logout effettuato");
        logStr = QString::number(this->sockID) + " log out a db con utente: " + user->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX,"logout fallito");
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
    this->user->prepareUtente(comando);
    QMap<QString,QString> risp;

    QString logStr;
    if (this->conn->userReg(*user) ){
        risp.insert(CMD, OK);
        risp.insert(MEX,"Registrazione completata");
        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + user->getUsername();
    } else {
        risp.insert(CMD, ERR);
        risp.insert(MEX,"Registrazione annullata, user già in uso o impossibile aggiungerlo");
        logStr = QString::number(this->sockID) + " non viene registrato a db con utente: " + user->getUsername();
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
           // clientMEX("Registrazione corretta");
            logStr = QString::number(this->sockID) + " disconnesso a db con utente: " + user->getUsername();
        } else {
          //  clientMEX("Registrazione fallita");
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
    if (!this->conn->isOpen()){
        qDebug() <<"connessione non aperta: " << conn->isOpen();
        this->connectDB();
    }

    // accedo a db e verifico file apribile dall'utente
    QString logStr;
    if (this->conn->canUserOpenFile(*user, comando.value(DOCID)) ){
        //clientMEX("File apribile dallì'utente");
        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + user->getUsername();
    } else {
        clientMsg("File non apribile dallì'utente");
        logStr = QString::number(this->sockID) + " File non apribile dallì'utente: " + user->getUsername();
        Logger::getLog().write(logStr);
        return;
    }
    Logger::getLog().write(logStr);

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
 */
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

/**
 * @brief s_thread::disconnected
 * metodo chiamato quando viene disconesso il socket dal client.
 */
void s_thread::disconnected()
{
    qDebug() << "client disconnesso "
             << QString::number(this->sockID);
    this->socket->deleteLater();
    this->disconnectDB();
    //delete this->socket;
    this->quit();
}


