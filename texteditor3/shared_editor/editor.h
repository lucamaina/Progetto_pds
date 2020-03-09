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
    double getLocalIndexInsert(int posCursor);
    double getLocalIndexDelete(int posCursor);

    int localPosCursor(double index);

    bool cursorChange(Message msg);
    bool remoteSend(Message msg);

    double insertLocal(double index,char value, QTextCharFormat formato);
    int deleteLocal(double index, char car);

    void updateFormat(double index, QTextCharFormat formato);

    double fmed(double num1, double num2);
    bool fequal(double a, double b);

    QString getTesto();



private:

    // metodi non definiti
    Editor(const Editor & hold) = delete;
    const Editor &operator=(const Editor & hold) = delete;

signals:

public slots:


};

#endif // EDITOR_H
