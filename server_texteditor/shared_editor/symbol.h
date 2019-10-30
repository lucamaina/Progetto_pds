#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>
#include <QMap>

#include "../server/cmdstring.h"

class Symbol
{
private:
    QChar car;
    int cursor;
    int posX, posY;
    double num, den;
    double index;
    QString userName;           // identifica utente che inserische il carattere

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, uint num, uint den) :  userName(user), car(car), num(num), den(den){}
    explicit Symbol(QString user, QChar car, double idx) :  userName(user), car(car), index(idx){}
    explicit Symbol(QString user, QChar car, double idx, int cur) :  userName(user), car(car), index(idx), cursor(cur){}
    QChar getChar(){return this->car;}
    QMap<QString, QString> toMap();

    double getNum() const;
    void setNum(const double &value);
    double getDen() const;
    void setDen(const double &value);
    QString getUserName() const;
    void setUserName(const QString &value);
    double getIndex() const;
    void setIndex(const double &value);

};


#endif // SYMBOL_H
