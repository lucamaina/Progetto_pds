#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QMap>
#include <mutex>
#include <QDebug>
#include <QTcpSocket>
#include "message.h"
#include "symbol.h"

class s_thread;

class Editor : public QObject
{
    Q_OBJECT

private:
    // std::mutex m;
    QString username;
    QString nomeFile;
    QString DocId;      // id usato nel db
    QFile *file;
    // QVector<Symbol> *symVec;
    QMap<double, Symbol> symMap;
    QVector<Symbol>_symbols;


public:
    explicit Editor(QObject *parent = nullptr);
    explicit Editor(QString Id, QString nome, QString body, QString username);

    static Editor& getFile();
    bool loadMap();

    QString mapToSend();
    ~Editor();

    /****************************************************************************
     * metodi di insert e delete ************************************************/
    bool localInsert(Message msg);
    void localDelete(int index);
    bool cursorChange(Message msg);
    bool remoteSend(Message msg);
    int insertLocal(int index,char value);
    QVector<int> fsum(int num1, int den1, int num2, int den2);
    QVector<int> fmed(int num1, int den1, int num2, int den2);




private:

    // metodi non definiti
    Editor(const Editor & hold) = delete;
    const Editor &operator=(const Editor & hold) = delete;

signals:

public slots:


};

#endif // EDITOR_H
