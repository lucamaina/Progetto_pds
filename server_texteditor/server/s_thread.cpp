#include "s_thread.h"

s_thread::s_thread(int ID, QObject *parent) : QThread(parent)
{
    this->sockID = ID;
}


void s_thread::run()
{
    Logger::getLog().write("Nuovo thread in esecuzione, ID = "+ QString::number(this->sockID) );
    qDebug() << "Thread running, ID: " << this->sockID;
    this->socket = new QTcpSocket();
    socket->setSocketDescriptor(sockID);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::ConnectionType::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::ConnectionType::DirectConnection);
    qDebug() << "Client connesso";
    this->user = new utente();
/**
    this->conn = new db(this->sockID);
    this->conn->conn();
    QSqlQuery q ;//= conn->query(queryPROVA);

    QVector<QString> cmd;

    cmd.push_back("asd");
    cmd.push_back("1");
    qDebug() << cmd;
    q = this->conn->query(queryLOGIN, cmd);

    if (q.first()){
        QString s = q.value(0).toString();
        qDebug() << s;
    }
    */
}

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
            qDebug() << "start doc: " << token << " - " << stream.readElementText();
        }
        token = stream.readNext();
        // leggo elemento con nome del comando
        if (token == QXmlStreamReader::StartElement){
            qDebug() << "comando: " << stream.name();
            QString cmd = stream.name().toString();
            command.insert("cmd", cmd);
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
    auto comando = cmd.find("cmd");
    if (comando.value() == CONN) {
        this->connectDB();
    } else if (comando.value() == LOGIN) {
        this->loginDB(cmd);
    } else if (comando.value() == LOGOFF) {
        this->logoffDB(cmd);
    } else if (comando.value() == REG) {
        this->registerDB(cmd);
    } else if (comando.value() == REM_IN || comando.value() == REM_DEL) {
        this->sendMsg(cmd);
    } else if (comando.value() == DISC) {
        this->disconnectDB();
    } else if (comando.value() == FILES) {
        Editor* ed = new Editor("file.txt");
    } else if (comando.value() == ADD_FILE) {
        Network &net = Network::getNetwork();
        net.createEditor(cmd.value(FNAME), *user);
    }
}

bool s_thread::clientMsg(QByteArray data){
    if (socket->isOpen() && socket->isWritable()){
        if (this->socket->write(data, data.size()) == -1){
            // errore
            qDebug() << "errore scrittura verso client";
        }
    }
    return false;
}

bool s_thread::clientMsg(QMap<QString, QString> comando){
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
        if (conn->conn() == false){
            // ritorna messaggio al client di fallimento
            clientMsg("impossibile connettersi al db");
            logStr = QString::number(this->sockID) + "non connesso a db con utente: ";
        } else {
            // messaggio di successo al client
            clientMsg("connessione al db riuscita");
            logStr = QString::number(this->sockID) + " connesso a db con utente: ";
        }
        Logger::getLog().write(logStr);
    }
}

void s_thread::loginDB(QMap<QString, QString> comando){
    if (!this->conn->isOpen()){
        qDebug() <<"connessione non aperta: " << conn->isOpen();
        this->connectDB();
    }
    this->user->prepareUtente(comando);

    QString logStr;
    if (this->conn->userLogin(*user) ){
        clientMsg("Login corretto");
        logStr = QString::number(this->sockID) + " loggato a db con utente: " + user->getUsername();
    } else {
        clientMsg("Login fallito");
        logStr = QString::number(this->sockID) + " non loggato a db con utente: " + user->getUsername();
    }
    Logger::getLog().write(logStr);
}

void s_thread::registerDB(QMap<QString, QString> comando){
    if (!this->conn->isOpen()){
        qDebug() <<"connessione non aperta: " << conn->isOpen();
        this->connectDB();
    }
    this->user->prepareUtente(comando);
    QString logStr;
    if (this->conn->userReg(*user) ){
        clientMsg("Registrazione corretta");
        logStr = QString::number(this->sockID) + " viene registrato a db con utente: " + user->getUsername();
    } else {
        clientMsg("Registrazione fallita");
        logStr = QString::number(this->sockID) + " non viene registrato a db con utente: " + user->getUsername();
    }
    Logger::getLog().write(logStr);
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
            clientMsg("Registrazione corretta");
            logStr = QString::number(this->sockID) + " disconnesso a db con utente: " + user->getUsername();
        } else {
            clientMsg("Registrazione fallita");
            logStr = QString::number(this->sockID) + " errore nella disconessione a db con utente: " + user->getUsername();
        }
        Logger::getLog().write(logStr);
    }
}

/*********************************************************************************************************
 ************************ metodi di accesso a Network ********************************************************
 *********************************************************************************************************/

/**
 * @brief s_thread::sendMsg
 * @param comando
 * invia il messaggio alla coda del network
 */
void s_thread::sendMsg(QMap<QString, QString> comando)
{
    Message msg = Message();
    msg.prepareMsg(comando);
    Network &net = Network::getNetwork();
    net.push(msg);
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
             << this->sockID;
    this->socket->deleteLater();
    exit(0);
}

s_thread::~s_thread()
{
    this->disconnectDB();
    delete this->conn;
    delete this->user;
}
