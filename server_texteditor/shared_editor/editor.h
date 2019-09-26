#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <mutex>
#include <QDebug>

#include "message.h"
#include "symbol.h"

class Editor : public QObject
{
    Q_OBJECT

public:
    explicit Editor(QObject *parent = nullptr);
    explicit Editor(QString nome);
    static Editor& getFile();
    ~Editor();

    /****************************************************************************
     * metodi controllo utenti **************************************************/
    bool addUser(const QString& nomeUser);
    bool removeUser(const QString& nomeUser);  // utente& nomeUser
    bool findUser(const QString &nomeUser);

    /****************************************************************************
     * metodi di insert e delete ************************************************/
    bool localInsert(Symbol sym);
    bool localRemove(Symbol sym);
    bool remoteInsert(Symbol sym);
    bool remoteRemove(Symbol sym);
    bool process(Message& msg);


private:
    bool save();

    // metodi non definiti
    Editor(const Editor & hold) = delete;
    const Editor &operator=(const Editor & hold) = delete;

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
