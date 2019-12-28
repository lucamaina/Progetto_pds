#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>
#include <QMap>
#include <QTextCharFormat>
#include "Client/cmdstring.h"
#include "QDebug"
#include "Utils/utils.h"

class Symbol
{
public:
    QChar car;
    double index;
    QString userName;           // identifica utente che inserische il carattere
    QTextCharFormat formato;
    QByteArray qbformat;

friend QDataStream &operator <<(QDataStream& out, const Symbol& sym);
friend QDataStream &operator >>(QDataStream& in, Symbol& sym);

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, double idx, QTextCharFormat format) :  userName(user), car(car), index(idx){qbformat=serialize(format);}
    explicit Symbol(QString user, QChar car, double idx, int cur) :  userName(user), car(car), index(idx){}
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
    int getIndex() const;
    void setIndex(const int &value);

};


#endif // SYMBOL_H
