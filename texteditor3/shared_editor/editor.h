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


public:
    QMap<double, Symbol> symMap;

    explicit Editor(QObject *parent = nullptr);
    explicit Editor(QString Id, QString nome, QByteArray body, QString username);

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
    double insertLocal(double index,char value, QTextCharFormat formato);
    void deleteLocal(double index);
    void updateFormat(double index, QTextCharFormat formato);

    double fmed(double num1, double num2);




private:

    // metodi non definiti
    Editor(const Editor & hold) = delete;
    const Editor &operator=(const Editor & hold) = delete;

signals:

public slots:


};

#endif // EDITOR_H
