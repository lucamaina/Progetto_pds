#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QMap>
#include <mutex>
#include <QDebug>
#include <QTcpSocket>
#include <QTextCharFormat>
#include "message.h"
#include "symbol.h"

class Editor : public QObject
{
    Q_OBJECT

private:
    QString username;
    QString nomeFile;
    QString DocId;      // id usato nel db
    QFile *file;
    qint32 dim = 0;
    qint32 maxDim = std::numeric_limits<qint32>::max()/2;

public:
    QVector<Symbol> symVec;

    explicit Editor(QObject *parent = nullptr);
    explicit Editor(QString Id, QString nome, QByteArray &body, QString username);

    static Editor& getFile();
    bool loadMap();

    QString mapToSend();
    ~Editor();

    /****************************************************************************
     * metodi di insert e delete ************************************************/
    QVector<qint32> getLocalIndexInsert(qint32 posCur);
    bool getLocalIndexDelete(int posCursor, Symbol &sym);

    int localPosCursor(QVector<qint32> & index);
    int findLocalPosCursor(QVector<qint32> & index);

    bool cursorChange(Message msg);
    bool remoteSend(Message msg);

    QString getNomeFile() const;

    QString getDocId() const;

private:
    // metodi non definiti
    Editor(const Editor & hold) = delete;
    const Editor &operator=(const Editor & hold) = delete;

signals:
    void s_read();

};

#endif // EDITOR_H
