/*
 * Classe implementa l'editor condiviso
 * 
 * prova di git
 */

#include "editor.h"

Editor::Editor(QObject *parent) : QObject(parent)
{
    this->symVec = new QVector<Symbol>();
    this->file = new QFile();
}

Editor::Editor(QString nomeFile)
{
    this->symVec = new QVector<Symbol>();
    this->nomeFile = nomeFile;
    this->file = new QFile("files/"+nomeFile);
    file->open(QIODevice::Append | QIODevice::Text);
    file->write("asd");
    file->close();
}
