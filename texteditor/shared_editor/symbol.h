#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>
#include <QMap>
#include <QTextCharFormat>
#include "Client/cmdstring.h"

class Symbol
{
private:
    QChar car;
    int cursor;
    double index;
    QString userName;           // identifica utente che inserische il carattere
    QTextCharFormat formato;

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, double idx) :  userName(user), car(car), index(idx){}
    explicit Symbol(QString user, QChar car, double idx, int cur) :  userName(user), car(car), index(idx), cursor(cur){}
    QChar getChar(){return this->car;}
    QMap<QString, QString> toMap();

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
