#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>
#include <QMap>
#include <QTextCharFormat>
#include "Client/cmdstring.h"
#include "QDebug"
#include "Utils/utils.h"
class Editor;

class Symbol
{
public:
    QChar car;
    qint32 index;
    QString userName;           // identifica utente che inserische il carattere
    QTextCharFormat textFormat;
    QByteArray formato;
    QMap<qint32, Symbol> nextLevel;

    // versione con QVector
    QVector<qint32> indici;


friend QDataStream &operator <<(QDataStream& out, const Symbol& sym);
friend QDataStream &operator >>(QDataStream& in, Symbol& sym);

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, qint32 idx, QTextCharFormat format) : car(car), index(idx),  userName(user){ formato = serialize(format);}
    explicit Symbol(QString user, QChar car, QVector<qint32> idx, QTextCharFormat format) :  car(car), userName(user), indici(idx){ formato = serialize(format);}

    explicit Symbol(QString user, QChar car, qint32 idx, QByteArray formato) :  car(car), index(idx), userName(user), formato(formato){}
    QChar getChar(){return this->car;}
    QMap<QString, QString> toMap();

    void setFormat(const QTextCharFormat &format);
    QTextCharFormat getFormat(void) const;
    int getNum() const;
    void setNum(const int &value);
    int getDen() const;
    void setDen(const int &value);
    QString getUserName() const;
    void setUserName(const QString &value);


    QList<Symbol> & read(QList<Symbol> &testo);

     qint32 getLocalIndex(qint32 posCur, QVector<qint32>& vec);

    qint32 getIndex() const;
    void setIndex(const qint32 &value);

signals:
    void s_read(QChar c, QTextCharFormat f, qint32 pos);

};


#endif // SYMBOL_H
