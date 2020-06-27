#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>
#include <QDebug>
#include <QMap>
#include <QtGui/QTextCharFormat>
#include <QJsonObject>
#include <QDataStream>

#include "../server/cmdstring.h"


class Symbol
{
private:
    QChar car;
    QVector<qint32> index;
    QString userName;   // identifica utente che inserische il carattere
    QByteArray formato;

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, QVector<qint32> idx) :  userName(user), car(car), index(idx){}
    explicit Symbol(QString user, QChar car, QVector<qint32> idx, QByteArray ba) :  userName(user), car(car), index(idx), formato(ba){}

    QChar getChar(){return this->car;}
    QMap<QString, QString> toMap();

    QString getUserName() const;
    void setUserName(const QString &value);

    friend QDataStream &operator <<(QDataStream &out, const Symbol &sym);
    friend QDataStream &operator >>(QDataStream &in, Symbol &sym);
    bool operator ==(const Symbol &s);

    QChar getCar() const;
    void setCar(const QChar &value);

    QByteArray getFormato() const;
    void setFormato(const QByteArray &value);

    QMap<qint32, Symbol> getNextLevel() const;
    void setNextLevel(const QMap<qint32, Symbol> &value);

    QVector<qint32> getIndex() const;
    void setIndex(const QVector<qint32> &value);
};



#endif // SYMBOL_H
