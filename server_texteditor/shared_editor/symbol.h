#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>

class Symbol
{

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, uint num, uint den, uint numR, uint denR) :  userName(user), car(car), num(num), den(den), numR(numR), denR(denR){}


private:
    QChar car;
    uint num, den, numR, denR;
    QString userName;           // identifica utente che inserische il carattere
};

#endif // SYMBOL_H
