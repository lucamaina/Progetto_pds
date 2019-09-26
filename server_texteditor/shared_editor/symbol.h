#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>

class Symbol
{

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, uint num, uint den, uint numR, uint denR) :  userName(user), car(car), num(num), den(den), numR(numR), denR(denR){}
    QChar getChar(){return this->car;}


    uint getNum() const;
    void setNum(const uint &value);

    uint getDen() const;
    void setDen(const uint &value);

    uint getNumR() const;
    void setNumR(const uint &value);

    uint getDenR() const;
    void setDenR(const uint &value);

    QString getUserName() const;
    void setUserName(const QString &value);

    uint getIndex() const;
    void setIndex(const uint &value);

private:
    QChar car;
    uint num, den, numR, denR;
    uint index;
    QString userName;           // identifica utente che inserische il carattere
};

#endif // SYMBOL_H
