#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <mutex>
#include "message.h"
#include "symbol.h"

class Editor : public QObject
{
    Q_OBJECT

public:
    explicit Editor(QObject *parent = nullptr);
    Editor (QString nomeFile);
    static Editor& getFile();
    ~Editor(){}
    bool remoteInsert(Symbol sym);
    bool remoteRemove(Symbol sym);
    bool process(Message& msg);
    bool addUser(utente& nomeUser);     // utente& nomeUser
    bool removeUser(QString nomeUser);  // utente& nomeUser

private:


    // metodi non definiti
    Editor(const Editor & hold);
    const Editor &operator=(const Editor & hold);

signals:

public slots:

public:
    // std::mutex m;
    QString nomeFile;
    QFile *file;
    QVector<Symbol> *symVec;
    QVector<QString> *userList;
};

#endif // EDITOR_H
