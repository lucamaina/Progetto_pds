#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>
#include <QMap>
// #include <QtGui/QTextCharFormat>
#include <QDataStream>

#include "../server/cmdstring.h"

// class QTextCharFormat;


class Symbol
{
private:
    QChar car;
    int posX, posY;
    double index;
    QString userName;           // identifica utente che inserische il carattere
    QByteArray formato;

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, double idx) :  userName(user), car(car), index(idx){}
    explicit Symbol(QString user, QChar car, double idx, int posX, int posY, QByteArray formato) :  userName(user), car(car), index(idx), posX(posX), posY(posY), formato(formato){}

    QChar getChar(){return this->car;}
    QMap<QString, QString> toMap();

    QString getUserName() const;
    void setUserName(const QString &value);
    double getIndex() const;
    void setIndex(const double &value);

    friend QDataStream &operator <<(QDataStream &out, const Symbol &sym);
    friend QDataStream &operator >>(QDataStream &in, Symbol &sym);


    QChar getCar() const;
    void setCar(const QChar &value);
    int getPosX() const;
    void setPosX(int value);
    int getPosY() const;
    void setPosY(int value);
    QByteArray getFormato() const;
    void setFormato(const QByteArray &value);
};



#endif // SYMBOL_H
