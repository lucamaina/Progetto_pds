/*
 * Classe implementa l'editor condiviso
 * 
 * prova di git
 */

#include "editor.h"

Editor::Editor(QObject *parent) : QObject(parent)
{
    this->symVec = new QVector<Symbol>();
    this->userList = new QVector<QString>();
}

Editor::Editor(QString nomeFile)
{
    this->symVec = new QVector<Symbol>();
    this->userList = new QVector<QString>();
    this->nomeFile = "files/"+nomeFile;
    this->file = new QFile(this->nomeFile);
    file->open(QIODevice::Append | QIODevice::Text);
    file->write("asd");
    file->close();
}

Editor::~Editor()
{
    // salva symVec su file
    try {
        save();
    } catch (std::exception& e) {
        qDebug() << e.what();
        // TODO altro
    }
    delete this->symVec;
    delete this->userList;
}

bool Editor::process(Message &msg)
{
    Message::msgType tipo = msg.getTipo();
    if (tipo == Message::msgType::Rem_In){
        // call localIN
    } else if (tipo == Message::msgType::Rem_Del) {
        // call localDel
    } else if (tipo == Message::msgType::Remove_File) {
        // call removeFile
    } else {
        return false;
    }
    return false;
}


/*********************************************************************************************************
 ************************ metodi di gestione di utentu ***************************************************
 *********************************************************************************************************/

bool Editor::addUser(const QString &nomeUser)
{
    if (userList->contains(nomeUser)){
        return false;
    }
    this->userList->append(nomeUser);
    return true;
}

bool Editor::findUser(const QString &nomeUser)
{
    if (this->userList->contains(nomeUser)){
        return true;
    }
    return false;
}


/*********************************************************************************************************
 ************************ metodi di gestione della mappa **************************************************
 *********************************************************************************************************/

bool Editor::localInsert(Symbol sym)
{

    return false;
}

bool Editor::save()
{
    int size = this->symVec->size();
    QChar c;
    QByteArray s;
    int times = size / 4096;
    file->open(QIODevice::WriteOnly | QIODevice::Text);

    for (int t = 0; t< times; t++){
        for (int i=0; i<4096; i++){
            auto a = symVec->at(t*4096 + i);
            c = a.getChar();
            s.append(c);
        }
        file->write(s);
    }
    file->close();
    return true;
}
