#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>
#include <QMap>
#include <QtGui/QTextCharFormat>
#include <QJsonObject>
#include <QDataStream>

#include "../server/cmdstring.h"

// class QTextCharFormat;


class Symbol
{
private:
    QChar car;
    double index;
    QString userName;           // identifica utente che inserische il carattere
    QByteArray formato;
    //QTextCharFormat ff;

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, double idx) :  userName(user), car(car), index(idx){}
    explicit Symbol(QString user, QChar car, double idx, QByteArray ba) :  userName(user), car(car), index(idx), formato(ba){}

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

    QJsonObject toJson();
    Symbol fromJson(QJsonObject j);
};



#endif // SYMBOL_H
